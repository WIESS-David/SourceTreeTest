/*
****************************************************************************
PROJECT :ADC driver
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


#ifndef ADCA_MAIN_H
#define ADCA_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: ADC7010 Internal Interface

  Only the driver modules shall include this header.
*/

/*******************************************************************************
  Section: Global Defines
*/

#undef R_EXTERN
#ifndef  ADCA_MAIN_GLOBALS_
#define R_EXTERN extern 
#else
#define R_EXTERN
#endif


/*******************************************************************************
  Section: Luke add channel define
*/
#define _ADC_KEY_CHANNEL    0x1C
#define _ADC_PANNEL_TEMP    0x1D
#define _ADC_SYS_TEMP       0x1E


/*******************************************************************************
  Section: Global constants 
*/

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Type: r_adc_Dev_t

  Device instance specific data.
  All instance specific data is kept within that structure.

  Members:
  BaseAddr      - Basis address
  CG0           - Channel Group 0
  CG1           - Channel Group 1
  CG2           - Channel Group 2
  IsrCallback   - Pointer to Isr callback functions

*/

typedef struct
{
    uint32_t            BaseAddr;
    uint8_t             CG0[R_ADC_CHANNEL_MAX_NUM];
    uint8_t             CG1[R_ADC_CHANNEL_MAX_NUM];
    uint8_t             CG2[R_ADC_CHANNEL_MAX_NUM]; 
    uint32_t            ChVal[R_ADC_CHANNEL_MAX_NUM];
} r_adc_Dev_t;


/*******************************************************************************
  Section: Global Variables
*/

/*******************************************************************************
  Variable: r_adc_Unit

  Array for device instance specific data.
*/

R_EXTERN r_adc_Dev_t r_adc_Unit[R_ADC_MACRO_NUM];


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_ADC_ErrorHandler

  Central error function. 
  
  When the driver detects any error, it will call this function.
  In case there is a registered error callback user function, 
  it  will be called. If a user handler is missing an endless loop is entered. 
     
  Parameters:
  Unit       - Instance number
  Error      - Error code, see: <r_adc_Error_t>
  
  Returns:
  void
*/

void R_ADC_ErrorHandler(uint32_t Unit, r_adc_Error_t Error);



#ifdef __cplusplus
}
#endif

#endif /* _ADCA_MAIN_H_  */
