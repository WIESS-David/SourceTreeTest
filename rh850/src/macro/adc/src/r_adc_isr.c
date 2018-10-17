/*
****************************************************************************
PROJECT : ADC driver
FILE    : $Id$
============================================================================
DESCRIPTION
Driver for ADC macro
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
  Title: ADC7010 Interrupt Functions

  Implementation of the macro driver ADC interrupt support functions 
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_adc_api.h"
#include "r_config_adc.h"
#include "r_dev_api.h"

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif

static void         (*IsrCallback[R_ADC_MACRO_NUM][R_ADC_INT_NUM])(void) =
{
    {0,0,0,0},
    {0,0,0,0}
}; 

/*******************************************************************************
  Section: Global API Functions
  
  For comments see file ADC_api.h
*/

/*******************************************************************************
  Function: R_ADC_SetIsrCallback
  
  See: <ADC_api.h> for details
*/

r_adc_Error_t R_ADC_SetIsrCallback(uint32_t Unit, r_adc_Interrupt_t IntType, void (*Isr)(void)) 
{
    if (Unit >= R_ADC_MACRO_NUM) 
    {
        return R_ADC_ERR_RANGE; 
    } 
    IsrCallback[Unit][IntType] = Isr;
    return R_ADC_ERR_OK;
}

/********************************************************************************
  Function: R_ADC_IsrErr
  
  See: <ADC_api.h> for details
*/

void R_ADC_IsrTsn(uint32_t Unit) 
{
    if (Unit >= R_ADC_MACRO_NUM) 
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "The Unit in question does not exist!");
    }
    else
    {
        if (IsrCallback[Unit][R_ADC_INT_TSN]) 
        {
            IsrCallback[Unit][R_ADC_INT_TSN]();
        }
    }
}

/********************************************************************************
  Function: R_ADC_Isr0
  
  See: <ADC_api.h> for details
*/

void R_ADC_Isr0(uint32_t Unit) 
{
    if (Unit >= R_ADC_MACRO_NUM) 
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "The Unit in question does not exist!");
    }
    else
    {
        if (IsrCallback[Unit][R_ADC_INT_CG0]) 
        {
            IsrCallback[Unit][R_ADC_INT_CG0]();
        }
    }
}

/********************************************************************************
  Function: R_ADC_Isr1
  
  See: <ADC_api.h> for details
*/

void R_ADC_Isr1(uint32_t Unit) 
{
    if (Unit >= R_ADC_MACRO_NUM) 
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "The Unit in question does not exist!");
    }
    else
    {
        if (IsrCallback[Unit][R_ADC_INT_CG1]) 
        {
            IsrCallback[Unit][R_ADC_INT_CG1]();
        }
    }
}

/********************************************************************************
  Function: R_ADC_Isr2
  
  See: <ADC_api.h> for details
*/

void R_ADC_Isr2(uint32_t Unit) 
{
    if (Unit >= R_ADC_MACRO_NUM) 
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "The Unit in question does not exist!");
    }
    else
    {
        if (IsrCallback[Unit][R_ADC_INT_CG2]) 
        {
            IsrCallback[Unit][R_ADC_INT_CG2]();
        }
    }
}



/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
