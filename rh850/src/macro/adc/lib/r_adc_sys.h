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
/******************************************************************************/

#ifndef R_SYS_ADC_H
#define R_SYS_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: ADC7010 API Support Functions (Device Dependant Part)
*/

/*******************************************************************************
  Section: Global Functions  
*/

/*******************************************************************************
  Function: R_SYS_ADC_Init

  Driver init function.

  Parameters:
    Unit       - Instance number
 
  Returns:
  void
*/

void R_SYS_ADC_Init(uint32_t Unit);


/*******************************************************************************
  Function: R_SYS_ADC_DeInit

  Driver deinit function.

  Parameters:
    Unit       - Instance number

  Returns:
  void
*/

void R_SYS_ADC_DeInit(uint32_t Unit);

/*******************************************************************************
  Function: R_SYS_ADC_GetBaseAddr

  Returns the base address of the dedicated macro .
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  Macro base address
*/

uint32_t R_SYS_ADC_GetBaseAddr(uint32_t Unit);

/*******************************************************************************
  Function: R_SYS_ADC_EnableInt

  Enable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_adc_Interrupt_t>
  
  Returns:
  void
*/

void R_SYS_ADC_EnableInt(uint32_t Unit, r_adc_Interrupt_t Interrupt);


/*******************************************************************************
  Function: R_SYS_ADC_DisableInt

  Disable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  Interrupt  - Interrupt type see: <r_adc_Interrupt_t>
  
  Returns:
  void
*/

void R_SYS_ADC_DisableInt(uint32_t Unit, r_adc_Interrupt_t Interrupt);


/*******************************************************************************
  Function: R_SYS_ADC_MUXAvailable

  Disable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  MPXAddr    - Multiplexer address
  
  Returns:
  r_adc_Error_t
*/
r_adc_Error_t R_SYS_ADC_MUXAvailable(uint32_t Unit, uint16_t MPXAddr);


/*******************************************************************************
  Function: R_SYS_ADC_SmpTimeAvailable

  Disable the interrupt controller for 'Interrupt'.
  
  Parameters:
  Unit       - Instance number
  SmpTime    - Sampling Time
  
  Returns:
  r_adc_Error_t
*/
r_adc_Error_t R_SYS_ADC_SmpTimeAvailable(uint32_t Unit, uint16_t SmpTime);


/*******************************************************************************
  Function: R_SYS_ADC_MaxChannelNum

  Get the Maximum ADC Channel Number for each Unit.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  uint8_t
*/
uint8_t R_SYS_ADC_MaxChannelNum(uint32_t Unit);


/*******************************************************************************
  Function: R_SYS_ADC_HwTrgNum

  Get the Maximum ADC Haedware Trigger Number for each Unit.
  
  Parameters:
  Unit       - Instance number
  
  Returns:
  uint8_t
*/
uint8_t R_SYS_ADC_HwTrgNum(uint32_t Unit);


/*******************************************************************************
  Function: R_SYS_ADC_DbgSVSTOP
  
  Enable the SVSTOP
  
  Returns:
  void
*/
void R_SYS_ADC_DbgSVSTOP (r_adc_Emu_t ADC_Emu);



#ifdef __cplusplus
}
#endif

#endif /* R_SYS_ADC_H */
