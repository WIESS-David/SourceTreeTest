/*
****************************************************************************
PROJECT : RLIN3 device driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
RLIN3 system functions for upd703532
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
  Title: upd703532 RLIN3 Support Functions

  Implementation of the RLIN3 support functions for the 
  upd703532 device.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_rlin3_api.h"
#include "r_dev_api.h"
#include "r_rlin3_sys.h"
#include "r_config_rlin3.h"

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif




/*******************************************************************************
  Section: Local Defines
*/


/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Constant: loc_BaseAddress

  Base address table of Rlin3 macros
*/
static const uint32_t loc_BaseAddress[R_RLIN3_MACRO_NUM] = 
{
/*rlin30*/     0xFFCE0000u,
/*rlin31*/     0xFFCE1000u,
/*rlin32*/     0xFFCE2000u,
/*rlin33*/     0xFFCE3000u
};

/*******************************************************************************
  Constant: loc_ClkSelId

  Clock selector ID for each UART unit  
*/
static const r_dev_ClkSel_t loc_ClkSelId[R_RLIN3_MACRO_NUM] = 
{
/*rlin30*/    R_DEV_CKS_RLIN,
/*rlin31*/    R_DEV_CKS_RLIN,
/*rlin32*/    R_DEV_CKS_RLIN,
/*rlin33*/    R_DEV_CKS_RLIN
};


/*******************************************************************************
  Section: UART Macro Driver Support Functions
  
  Comments see: <rlin3_sys.h>
*/


/*******************************************************************************
  Function: R_SYS_RLIN3_SysGetClockHz

  See: r_sys_rlin3.h for details    

*/

uint32_t R_SYS_RLIN3_SysGetClockHz(uint32_t Unit) 
{
    uint32_t val;
    val = R_DEV_ClkFrequencyHz(loc_ClkSelId[Unit]); 
    return val;
}


/*******************************************************************************
  Function: R_SYS_RLIN3_BaseAddr

  See: r_uerte_sys.h for details    

*/
uint32_t R_SYS_RLIN3_BaseAddr(uint32_t Unit) 
{
    uint32_t addr;
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
        return R_BAD_ADDRESS; 
    }
    addr = loc_BaseAddress[Unit];
    return addr;
}

/*******************************************************************************
  Function: R_SYS_RLIN3_Init
  
  See: r_uerte_sys.h for details    
*/

void R_SYS_RLIN3_Init(uint32_t Unit)
{
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable (R_DEV_INT_RLIN30S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN30R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN30T, 1);         
        break;
    case 1:
        R_DEV_IntEnable (R_DEV_INT_RLIN31S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN31R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN31T, 1);         
        break;
    case 2:
        R_DEV_IntEnable (R_DEV_INT_RLIN32S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN32R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN32T, 1);         
        break;
    case 3:
        R_DEV_IntEnable (R_DEV_INT_RLIN33S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN33R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN33T, 1);         
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RLIN3: We do not have this Unit");      
    break; 
    }    
}
    

/*******************************************************************************
  Function: R_SYS_RLIN3_DeInit
  
  See: r_uerte_sys.h for details    
*/

void R_SYS_RLIN3_DeInit(uint32_t Unit)
{
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable(R_DEV_INT_RLIN30S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN30R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN30T, 0);
        break;
    case 1:
        R_DEV_IntEnable(R_DEV_INT_RLIN31S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN31R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN31T, 0);
        break;
    case 2:
        R_DEV_IntEnable(R_DEV_INT_RLIN32S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN32R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN32T, 0);
        break;
    case 3:
        R_DEV_IntEnable(R_DEV_INT_RLIN33S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN33R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN33T, 0);
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RLIN3: We do not have this Unit");      
        break; 
    }    
}
    

/*******************************************************************************
  Function: R_SYS_RLIN3_EnableInt
*/

void R_SYS_RLIN3_EnableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt) 
{
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable (R_DEV_INT_RLIN30S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN30R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN30T, 1);         
        break;
    case 1:
        R_DEV_IntEnable (R_DEV_INT_RLIN31S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN31R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN31T, 1);         
        break;
    case 2:
        R_DEV_IntEnable (R_DEV_INT_RLIN32S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN32R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN32T, 1);         
        break;
    case 3:
        R_DEV_IntEnable (R_DEV_INT_RLIN33S, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN33R, 1); 
        R_DEV_IntEnable (R_DEV_INT_RLIN33T, 1);         
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RLIN3: We do not have this Unit");      
        break; 
    }    
}


/*******************************************************************************
  Function: R_SYS_RLIN3_DisableInt
*/

void R_SYS_RLIN3_DisableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt) {
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable(R_DEV_INT_RLIN30S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN30R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN30T, 0);
        break;
    case 1:
        R_DEV_IntEnable(R_DEV_INT_RLIN31S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN31R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN31T, 0);
        break;
    case 2:
        R_DEV_IntEnable(R_DEV_INT_RLIN32S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN32R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN32T, 0);
        break;
    case 3:
        R_DEV_IntEnable(R_DEV_INT_RLIN33S, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN33R, 0); 
        R_DEV_IntEnable(R_DEV_INT_RLIN33T, 0);
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RLIN3: We do not have this Unit");      
        break; 
    }    
}

