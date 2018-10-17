/*
****************************************************************************
PROJECT : VLIB macro device level driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
RTCA system functions for d1x_scit
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
  Title: dev_rtca RTCA device functions

  Implementation of the RTCA support functions for the 
  device dev_rtca.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_rtca_api.h"
#include "r_rtca_sys.h"
#include "r_config_rtca.h"


/*******************************************************************************
  Section: Local Constants
*/

/*******************************************************************************
  Section: Global Functions

*/

/*******************************************************************************
  Function: R_RTCA_SYS_BaseAddr
  
  See: r_rtca_sys.h for details
*/

uint32_t R_RTCA_SYS_BaseAddr(uint32_t Unit) 
{
    uint32_t retval = 0; 
    switch (Unit)
    {
        case 0:
	    retval = R_RTCA0_BASE;
	    break; 

        default:
	    retval = R_RTCA_ERR_INSTANCE; 
	    break; 
    }

    return retval;     
}


/*******************************************************************************
  Function: R_RTCA_SYS_HardwareInit
  
  See: r_rtca_sys.h for details
*/

r_rtca_Error_t R_RTCA_SYS_HardwareInit(uint32_t Unit) 
{
    if (Unit >= R_RTCA_MACRO_NUM) 
    {
        return R_RTCA_ERR_RANGE; 
    }
    else 
    {
        return R_RTCA_ERR_OK;
    } 
}


/*******************************************************************************
  Function: R_RTCA_SYS_HardwareDeInit
  
  See: r_rtca_sys.h for details
*/

r_rtca_Error_t R_RTCA_SYS_HardwareDeInit(uint32_t Unit) 
{
    if (Unit >= R_RTCA_MACRO_NUM) 
    {
        return R_RTCA_ERR_RANGE; 
    }
    else 
    {
        return R_RTCA_ERR_OK;
    } 
}


/*******************************************************************************
  Function: R_RTCA_SYS_EnableInt
*/

void R_RTCA_SYS_EnableInt(uint32_t Unit, r_rtca_Interrupt_t Interrupt) 
{
    switch (Unit)
    {
        case 0:
            R_DEV_IntEnable((r_dev_IntSel_t) (R_DEV_INT_RTCA01S + Interrupt), 1); 
            break;

        default:
            R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RTCA: This Unit does not exist!");      
            break; 
    }    
}


/*******************************************************************************
  Function: R_RTCA_SYS_DisableInt
*/

void R_RTCA_SYS_DisableInt(uint32_t Unit, r_rtca_Interrupt_t Interrupt) 
{
    switch (Unit)
    {
        case 0:
            R_DEV_IntEnable((r_dev_IntSel_t) (R_DEV_INT_RTCA01S + Interrupt), 0); 
            break;

        default:
            R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "RTCA: This Unit does not exist!");      
            break; 
    }    
}



