/*
****************************************************************************
PROJECT : RLIN3 driver
FILE ID : $Id$
============================================================================ 
DESCRIPTION
Driver for RLIN3 macro
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

#ifndef RLIN3_DEV_H_
#define RLIN3_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: RLIN3 Support Functions

  The generic RLIN3 driver uses these functions. They have
  to be implemented within the driver library for a concrete
  device.
*/

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_SYS_RLIN3_BaseAddr

  Gives back base address of dedicated macro channel.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  Macro base address
*/

uint32_t R_SYS_RLIN3_BaseAddr(uint32_t Unit);

/*******************************************************************************
  Function: R_SYS_RLIN3_SysGetClockHz

  Gives back clock frequency for the selected instance. 
  
  Parameters:
  Unit      - Instance number
  
  Returns:
  Clock frequency
*/

uint32_t R_SYS_RLIN3_SysGetClockHz(uint32_t Unit);


/*******************************************************************************
  Function: R_SYS_RLIN3_Init

  Initialize the given Unit on device level 
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_SYS_RLIN3_Init(uint32_t Unit);

/*******************************************************************************
  Function: R_SYS_RLIN3_DeInit

  De-Initialize the given Unit on device level 
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_SYS_RLIN3_DeInit(uint32_t Unit);



/*******************************************************************************
  Function: R_SYS_RLIN3_EnableInt

  Enable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_rlin3_Interrupt_t>
  
  Returns:
  void
*/

void R_SYS_RLIN3_EnableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt);


/*******************************************************************************
  Function: R_SYS_RLIN3_DisableInt

  Disable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_rlin3_Interrupt_t>
  
  Returns:
  void
*/

void R_SYS_RLIN3_DisableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt);


#ifdef __cplusplus
}
#endif


#endif /* RLIN3_DEV_H_ */
