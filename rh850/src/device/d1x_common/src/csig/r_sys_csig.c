/*
****************************************************************************
PROJECT : CSIG driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
CSIG system functions for D1M2(H)
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2014
                                  by 
                       Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing

DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/



/*******************************************************************************
  Title: D1M2(H) CSIG Support Functions

  Implementation of the CSIG support functions for the
  device D1M2(H).
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_csig_api.h"
#include "r_csig_sys.h"
#include "r_config_csig.h"
#include "r_dev_api.h"

#if defined (R_DBG_PRINT_CSIG_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#else
    #define R_DBG_PRINT(lvl, txt) 
#endif


/*******************************************************************************
  Section: Local Defines
*/

#define CSIG0_FCLACTL0                  0xFF4161E0u
#define CSIG0_FCLACTL1                  0xFF4161E4u
#define CSIG0_FCLACTL2                  0xFF4161E8u
#define CSIG0_FCLACTL3                  0xFF4161ECu
#define CSIG1_FCLACTL4                  0xFF4161F0u
#define CSIG1_FCLACTL5                  0xFF4161F4u
#define CSIG1_FCLACTL6                  0xFF4161F8u
#define CSIG1_FCLACTL7                  0xFF4161FCu
#define CSIG2_FCLACTL0                  0xFF416200u
#define CSIG2_FCLACTL1                  0xFF416204u
#define CSIG2_FCLACTL2                  0xFF416208u

#define CSIG3_FCLACTL0                  0xbadu
#define CSIG3_FCLACTL1                  0xbadu
#define CSIG3_FCLACTL2                  0xbadu

/*******************************************************************************
  Variable: loc_IntAddress

  Stores the Addresses of the Interrupt control registers for each unit
  The order of the adresses for each interrupt type schould be the same
  as defined in csig_api. See <r_csig_Interrupt_t>

*/

static r_dev_IntSel_t loc_IntAddress[R_CSIG_MACRO_NUM][R_CSIG_INT_NUM] = 
{
/*                CSIGnTIREIC   CSIGnTIRIC  CSIGnTICIC */
/* unit 0 */    { R_DEV_INT_CSIG0RE,   R_DEV_INT_CSIG0R, R_DEV_INT_CSIG0IC},
/* unit 1 */    { R_DEV_INT_CSIG1RE,   R_DEV_INT_CSIG1R, R_DEV_INT_CSIG1IC},
/* unit 2 */    { R_DEV_INT_CSIG2RE,   R_DEV_INT_CSIG2R, R_DEV_INT_CSIG2IC},
/* unit 3 */    { R_DEV_INT_CSIG3RE,   R_DEV_INT_CSIG3R, R_DEV_INT_CSIG3IC},
};

/*******************************************************************************
  Section: Global Functions
*/


/*******************************************************************************
  Function: R_SYS_CSIG_GetCsigBaseAddr
*/

uint32_t R_SYS_CSIG_GetCsigBaseAddr(uint32_t Unit) 
{
    uint32_t addr;

    switch(Unit) 
    {
    case 0:
        addr = 0xFFDA0000u;
        break;
    case 1:
        addr = 0xFFDA2000u;
        break;
    case 2:
        addr = 0xFFDA4000u;
        break;
    case 3:
        addr = 0xFFDA6000u;
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit");   
        break;
    }

    return addr;
}


/*******************************************************************************
  Function: R_SYS_CSIG_GetChbbBaseAddr
*/

uint32_t R_SYS_CSIG_GetChbbBaseAddr(uint32_t Unit) 
{
    uint32_t addr;

    addr = R_SYS_CSIG_GetCsigBaseAddr(Unit);
    addr += 0x1000;
    return addr;
}


/*******************************************************************************
  Function: R_SYS_CSIG_IntCInit

*/
void R_SYS_CSIG_IntCInit(uint32_t Unit) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }

    /* Enable the interrupts */
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIC],  1);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIR],  1);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIRE], 1);
}


/*******************************************************************************
  Function: R_SYS_CSIG_IntCDeInit

*/
void R_SYS_CSIG_IntCDeInit(uint32_t Unit) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }

    /* Enable the interrupts */
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIC],  0);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIR],  0);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIG_INT_TIRE], 0);
}



/*******************************************************************************
  Function: R_SYS_CSIG_EnableInt
*/
void R_SYS_CSIG_EnableInt(uint32_t Unit, r_csig_Interrupt_t Interrupt) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }
    if (Interrupt >= R_CSIG_INT_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_INTR, "CSIG: We do not have this kind of interrupt"); 
    }

    R_DEV_IntEnable(loc_IntAddress[Unit][Interrupt],1);
}

/*******************************************************************************
  Function: R_SYS_CSIG_GetIntMask
*/
uint8_t R_SYS_CSIG_GetIntMask(uint32_t Unit, r_csig_Interrupt_t Interrupt) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }
    if (Interrupt >= R_CSIG_INT_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_INTR, "CSIG: We do not have this kind of interrupt"); 
    }

    return (R_DEV_IntGetMask(loc_IntAddress[Unit][Interrupt]));
}


/*******************************************************************************
  Function: R_SYS_CSIG_DisableInt
*/
void R_SYS_CSIG_DisableInt(uint32_t Unit, r_csig_Interrupt_t Interrupt) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }
    if (Interrupt >= R_CSIG_INT_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_INTR, "CSIG: We do not have this kind of interrupt"); 
    }

   R_DEV_IntEnable(loc_IntAddress[Unit][Interrupt],0);
}


/*******************************************************************************
  Function: R_SYS_CSIG_ForceClearInt
*/

void R_SYS_CSIG_ForceClearInt(uint32_t Unit, r_csig_Interrupt_t Interrupt, uint8_t Set) 
{
    if (Unit >= R_CSIG_MACRO_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "CSIG: We do not have this Unit"); 
    }
    if (Interrupt >= R_CSIG_INT_NUM)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_INTR, "CSIG: We do not have this kind of interrupt"); 
    }


    if(0 != Set)
    {
        R_DEV_IntSetFlag(loc_IntAddress[Unit][Interrupt]);
    }else 
    {
        R_DEV_IntClearFlag(loc_IntAddress[Unit][Interrupt]);
    }
}

/*******************************************************************************
  Function: R_SYS_CSIG_SetFilterBypass
*/
uint32_t R_SYS_CSIG_SetFilterBypass(uint32_t Unit)  
{
    uint32_t val = 0u;
    R_DBG_PRINT(R_DBG_MSG_CODE, "CSIG Set input filter bypass"); 
    if (Unit >= R_CSIG_MACRO_NUM){
        val = 1u;
    }
    else{
        if(Unit == 0u){
            R_CSIG_WRITE_REG(8, CSIG0_FCLACTL0, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG0_FCLACTL1, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG0_FCLACTL2, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG0_FCLACTL3, 0x80u);
        
        }
        else if (Unit == 1){
            R_CSIG_WRITE_REG(8, CSIG1_FCLACTL4, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG1_FCLACTL5, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG1_FCLACTL6, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG1_FCLACTL7, 0x80u);
        }
        else if (Unit == 2){
            R_CSIG_WRITE_REG(8, CSIG2_FCLACTL0, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG2_FCLACTL1, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG2_FCLACTL2, 0x80u);   
        }
        else if (Unit == 3){
            R_CSIG_WRITE_REG(8, CSIG3_FCLACTL0, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG3_FCLACTL1, 0x80u);
            R_CSIG_WRITE_REG(8, CSIG3_FCLACTL2, 0x80u);   
        }
        else {
            val = 1u;
        }
    }
    return val;
}

/*******************************************************************************
  Function: R_SYS_CSIG_GetClock
*/
uint32_t R_SYS_CSIG_GetClock(uint32_t Unit) 
{ 

    uint32_t val;                                 
                                                                                       
    if(Unit == 0u)
    {
         val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIG);
    }
    else if (Unit == 1)
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIG);
    }
    else if (Unit == 2)
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIG);
    }        
    else if (Unit == 3)
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIG);
    }        
    else
    {
        val = 0u;
    }
    return val;                                   
}


/*******************************************************************************
  Function: R_SYS_CSIG_EnableDTSInt
*/
void R_SYS_CSIG_EnableDTSInt (uint32_t Unit)
{
    if (Unit == 0u)
    {
        
    }
}
