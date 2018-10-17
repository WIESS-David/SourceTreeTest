/*
****************************************************************************
PROJECT : CSIH driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
CSIH system functions for r7f701412 (dummy, to prepare for d1m2 device)
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

Warranty Disclaimer

Because the Product(s) is licensed free of charge, there is no warranty 
of any kind whatsoever and expressly disclaimed and excluded by Renesas, 
either expressed or implied, including but not limited to those for 
non-infringement of intellectual property, merchantability and/or 
fitness for the particular purpose. 
Renesas shall not have any obligation to maintain, service or provide bug 
fixes for the supplied Product(s) and/or the Application.

Each User is solely responsible for determining the appropriateness of 
using the Product(s) and assumes all risks associated with its exercise 
of rights under this Agreement, including, but not limited to the risks 
and costs of program errors, compliance with applicable laws, damage to 
or loss of data, programs or equipment, and unavailability or 
interruption of operations.

Limitation of Liability

In no event shall Renesas be liable to the User for any incidental, 
consequential, indirect, or punitive damage (including but not limited 
to lost profits) regardless of whether such liability is based on breach 
of contract, tort, strict liability, breach of warranties, failure of 
essential purpose or otherwise and even if advised of the possibility of 
such damages. Renesas shall not be liable for any services or products 
provided by third party vendors, developers or consultants identified or
referred to the User by Renesas in connection with the Product(s) and/or the 
Application.

****************************************************************************
*/


/*******************************************************************************
  Title: r7f701412 CSIH Support Functions

  Implementation of the CSIH support functions for the 
  device r7f701412.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_csih_api.h"
#include "r_csih_sys.h"
#include "r_config_csih.h"
#include "r_dev_api.h"

/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: _InterruptAdr

  Stores the Addresses of the Interrupt control registers for each unit
  The order of the adresses for each interrupt type schould be the same 
  as defined in csih_api. See <r_csih_Interrupt_t> 
   
*/

static r_dev_IntSel_t loc_IntAddress[R_CSIH_MACRO_NUM][R_CSIH_INT_NUM] = {
/*                CSIHnRE               CSIHnR	            CSIHnC	            CSIHnJC */    
/* unit 0 */    { R_DEV_INT_CSIH0RE,    R_DEV_INT_CSIH0R,   R_DEV_INT_CSIH0C,   R_DEV_INT_CSIH0JC},
/* unit 1 */    { R_DEV_INT_CSIH1RE,    R_DEV_INT_CSIH1R,   R_DEV_INT_CSIH1C,   R_DEV_INT_CSIH1JC},
}; 

/*******************************************************************************
  Section: Global Functions
*/


/*******************************************************************************
  Function: R_SYS_CSIH_GetCsihBaseAddr
*/

uint32_t R_SYS_CSIH_GetCsihBaseAddr(uint32_t Unit) 
{
    uint32_t addr;

    switch(Unit) 
    {
        case 0:
            addr = 0xFFDA8000u;
            break;
        case 1:
            addr = 0xFFDAA000u;
            break;
        default:
            addr = (uint32_t)-1;
            break;
    }
    return addr;
}


/*******************************************************************************
  Function: R_SYS_CSIH_GetChbaBaseAddr
*/

uint32_t R_SYS_CSIH_GetChbaBaseAddr(uint32_t Unit) 
{
    uint32_t addr;
    
    switch(Unit) 
    {
        case 0:
        case 1:
    /* The CSIH driver already uses two different base offsets in its register configuration.
       See: R_CSIH_USER_OFFSET, R_CSIH_OS_OFFSET.
       This is why this value is equal to CsihBaseAddr. */
            addr = R_SYS_CSIH_GetCsihBaseAddr(Unit);
            break;
        default:
            addr = (uint32_t)-1;
            break;
    }
    return addr;
}


/*******************************************************************************
  Function: R_SYS_CSIH_GetCsihHwVariant
*/

uint32_t R_SYS_CSIH_GetCsihHwVariant(uint32_t Unit) 
{
    uint32_t variant;

    switch(Unit) 
    {
        case 0:
            variant = (R_CSIH_VARIANT_ORIGINAL);
            break;
        case 1:
            variant = (R_CSIH_VARIANT_ALTERNATIVE);
            break;
        default:
            variant = (uint32_t)-1;
            break;
    }
    return variant;
}


/*******************************************************************************
  Function: R_SYS_CSIH_GetNbChipSelect
*/

int32_t R_SYS_CSIH_GetNbChipSelect(uint32_t Unit) 
{
    if (Unit >= R_CSIH_MACRO_NUM)
    { 
        return -1; 
    }
    /* 1 CS is I/O mapped. The other CS can still be configured for instance to use different clocks. */
    return 8;
}



/*******************************************************************************
  Function: R_SYS_CSIH_IntCInit

*/
void R_SYS_CSIH_IntCInit(uint32_t Unit) 
{
    /* Enable the interrupts */
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIH_INT_TIC],  1);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIH_INT_TIJC], 1);   
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIH_INT_TIR],  1);
    R_DEV_IntEnable(loc_IntAddress[Unit][R_CSIH_INT_TIRE], 1); 
}


/*******************************************************************************
  Function: R_SYS_CSIH_EnableInt
*/
void R_SYS_CSIH_EnableInt(uint32_t Unit, r_csih_Interrupt_t Interrupt) 
{
    R_DEV_IntEnable(loc_IntAddress[Unit][Interrupt],1); 
}

/*******************************************************************************
  Function: R_SYS_CSIH_DisableInt
*/
void R_SYS_CSIH_DisableInt(uint32_t Unit, r_csih_Interrupt_t Interrupt) 
{
   R_DEV_IntEnable(loc_IntAddress[Unit][Interrupt],0); 
}

/*******************************************************************************
  Function: R_SYS_CSIH_GetIntMask
*/
uint8_t R_SYS_CSIH_GetIntMask(uint32_t Unit, r_csih_Interrupt_t Interrupt) 
{
   return R_DEV_IntGetMask(loc_IntAddress[Unit][Interrupt]); 
}


/*******************************************************************************
  Function: R_SYS_CSIH_ForceClearInt
*/

void R_SYS_CSIH_ForceClearInt(uint32_t Unit, r_csih_Interrupt_t Interrupt, uint8_t Set) 
{
    if(0 != Set) 
    {
        R_DEV_IntSetFlag(loc_IntAddress[Unit][Interrupt]);
    }
    else
    {
        R_DEV_IntClearFlag(loc_IntAddress[Unit][Interrupt]);
    }
}

/*******************************************************************************
  Function: R_SYS_CSIH_SetFilterBypass
*/
#if 0
uint32_t R_SYS_CSIH_SetFilterBypass(uint32_t Unit)  
{
    uint32_t val = 0;

    if(Unit == 0)
    {
        R_CSIH_WRITE_REG(8, CSIH0_FCLACTL0, 0x80);
        R_CSIH_WRITE_REG(8, CSIH0_FCLACTL1, 0x80);
        R_CSIH_WRITE_REG(8, CSIH0_FCLACTL2, 0x80);
        R_CSIH_WRITE_REG(8, CSIH0_FCLACTL3, 0x80);
    
    }
    else if(Unit == 1)
    {
        R_CSIH_WRITE_REG(8, CSIH1_FCLACTL0, 0x80);
        R_CSIH_WRITE_REG(8, CSIH1_FCLACTL1, 0x80);
        R_CSIH_WRITE_REG(8, CSIH1_FCLACTL2, 0x80);
        R_CSIH_WRITE_REG(8, CSIH1_FCLACTL3, 0x80);
    
    }
    else 
    {
        val = 1;
    }
    return val;
}
#endif
/*******************************************************************************
  Function: R_SYS_CSIH_GetClock
*/
uint32_t R_SYS_CSIH_GetClock(uint32_t Unit) 
{ 
    uint32_t val;                                 
                                                                                       
    if(Unit == 0)
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIH);
    }
    else if (Unit == 1)
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_CSIH);
    }
    else
    {
        val = 0;
    }
    return val;                                   
}
