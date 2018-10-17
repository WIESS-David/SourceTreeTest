/*
****************************************************************************
PROJECT : VLIB macro device level driver
FILE    : $Id$
============================================================================ 
DESCRIPTION
TAUB system functions for d1x_scit
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
  Title: dev_taub TAUB device functions

  Implementation of the TAUB support functions for the 
  device dev_taub.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_taub_api.h"
#include "r_taub_sys.h"
#include "r_config_taub.h"


/***********************************************************
  Section: Local Defines
*/


/*******************************************************************************
  Section: Local Constants
*/

/*******************************************************************************
  Section: Global Functions

*/

/*******************************************************************************
  Function: R_TAUB_SYS_BaseAddr
  
  See: r_taub_sys.h for details
*/

uint32_t R_TAUB_SYS_BaseAddr(uint32_t Unit) 
{
    uint32_t retval = 0; 
    switch (Unit)
    {
    case 0:
	    retval = R_TAUB0_BASE;
	    break; 

    case 1:
	    retval = R_TAUB1_BASE;
	    break; 

    case 2:
	    retval =  R_TAUB2_BASE;
	    break; 

    default:
	    retval = R_TAUB_ERR_INSTANCE; 
	break; 
    }
    return retval;     
}

/*******************************************************************************
  Function: R_TAUB_SYS_HardwareInit
  
  See: r_taub_sys.h for details
*/

r_taub_Error_t R_TAUB_SYS_HardwareInit(uint32_t Unit) 
{
    if (Unit >= R_TAUB_MACRO_NUM) 
      {
        return R_TAUB_ERR_RANGE; 
      }
    else 
      {
        return R_TAUB_ERR_OK;
      } 
}
/*******************************************************************************
  Function: R_TAUB_SYS_HardwareDeInit
  
  See: r_taub_sys.h for details
*/

r_taub_Error_t R_TAUB_SYS_HardwareDeInit(uint32_t Unit) 
{
    if (Unit >= R_TAUB_MACRO_NUM) 
      {
        return R_TAUB_ERR_RANGE; 
      }
    else 
      {
        return R_TAUB_ERR_OK;
      } 
}

/*******************************************************************************
  Function: R_TAUB_SYS_EnableInt
*/

void R_TAUB_SYS_EnableInt(uint32_t Unit, r_taub_Interrupt_t Interrupt) 
{
    if  (Unit == 1)
    {
        /* enable interrupt of unit TAUB1 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB1I0 + Interrupt) ,1);
    }
    else if  (Unit == 2)
    {
        /* enable interrupt of unit TAUB2 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB2I0 + Interrupt) ,1);
    }
    else
    {
        /* enable interrupt of unit TAUB0 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB0I0 + Interrupt) ,1);
    } 
}


/*******************************************************************************
  Function: R_TAUB_SYS_DisableInt
*/

void R_TAUB_SYS_DisableInt(uint32_t Unit, r_taub_Interrupt_t Interrupt) 
{
    if  (Unit == 1)
    {
        /* enable interrupt of unit TAUB1 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB1I0 + Interrupt) ,0);
    }
    else if  (Unit == 2)
    {
        /* enable interrupt of unit TAUB2 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB2I0 + Interrupt) ,0);
    }
    else
    {
        /* enable interrupt of unit TAUB0 */
        R_DEV_IntEnable((r_dev_IntSel_t)(R_DEV_INT_TAUB0I0 + Interrupt) ,0);
    } 
}

