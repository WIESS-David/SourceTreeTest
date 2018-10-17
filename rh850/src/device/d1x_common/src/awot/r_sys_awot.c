/*
****************************************************************************
PROJECT : AWOT driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
Driver for AWOT macro
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2012
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
  Title: uPD70F3532 AWOT Support Functions

  Implementation of the AWOT support functions for the
  uPD70F3532 (DN4-H) device.
*/


/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_awot_api.h"
#include "r_awot_sys.h"
#include "r_config_awot.h"
#include "r_dev_api.h"


/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Constant: loc_BaseAddress

  Base address table of AWOT macros
*/
static const uint32_t loc_BaseAddress[R_AWOT_MACRO_NUM] = 
{
/*awot0*/     R_AWOT0_BASE_ADDR,
};


/*******************************************************************************
  Section: Local Types
*/


/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Section: AWOT Macro Driver Support Functions

  Comments see: <awot_sys.h>
*/


/*******************************************************************************
  Function: R_SYS_AWOT_GetBaseAddr
*/

uint32_t R_SYS_AWOT_GetBaseAddr(uint32_t Unit)
{
    uint32_t addr;
    addr = loc_BaseAddress[Unit];
    return addr;
}


/*******************************************************************************
  Function: R_SYS_AWOT_GetClock
*/

uint32_t R_SYS_AWOT_GetClock(uint32_t Unit) 
{
    uint32_t val;

    if(Unit == 0u) 
    {
        val = R_DEV_ClkFrequencyHz(R_DEV_CKS_AWOT);
    } else {
        val = 0u;
    }
    return val;
}



/*******************************************************************************
  Function: R_SYS_AWOT_EnableInt
*/

void R_SYS_AWOT_EnableInt(uint32_t Unit, r_awot_Interrupt_t Interrupt) 
{
        R_DEV_IntEnable(R_DEV_INT_AWOTM0, 1);
}

/*******************************************************************************
  Function: R_SYS_AWOT_DisableInt
*/

void R_SYS_AWOT_DisableInt(uint32_t Unit, r_awot_Interrupt_t Interrupt)
{
    R_DEV_IntEnable(R_DEV_INT_AWOTM0, 0);
}


