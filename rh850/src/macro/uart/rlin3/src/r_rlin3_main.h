/*
****************************************************************************
PROJECT : RLIN3 driver
FILE    : $Id$
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


#ifndef RLIN3_MAIN_H_
#define RLIN3_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Title: Main Module Internal Interface

  Only the driver modules shall include this header.
*/

/*******************************************************************************
  Section: Global Defines
*/

#undef R_EXTERN
#ifndef RLIN3_MAIN_GLOBALS
#define R_EXTERN extern 
#else
#define R_EXTERN
#endif

/*******************************************************************************
  Section: Global Macros
*/

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Type: r_rlin3_Dev_t

  Device instance specific data.
  
  All instance specific data is kept within that structure.

  Members:
  BaseAddr      - Basis address
  RxBuf         - Receive buffer control structure
  TxBuf         - Transmit buffer control structure  
  IsrCallback   - Pointer table to Isr callback functions
  RxBuffer      - Receive buffer
  TxBuffer      - Transmit buffer
  DrvExtension  - If set, alternative UART handling (for test purposes and 
                  extended configuration) is applied 
  
*/

typedef struct {
    uint32_t       BaseAddr;
    r_cdi_RBuf_t   RxBuf;
    r_cdi_RBuf_t   TxBuf;
    uint8_t        TxOngoing;
    uint8_t        RxOngoing;
    uint8_t        TxRemain;
    uint8_t        RxRemain; 
    void         (*IsrCallback[R_RLIN3_INT_NUM])(void);
    uint8_t        RxBuffer[R_RLIN3_RXBUF_SIZE];
    uint8_t        TxBuffer[R_RLIN3_TXBUF_SIZE];
    uint8_t      DrvExtension; 
} r_rlin3_Dev_t;


/*******************************************************************************
  Variable: r_rlin3_Dev

  Array for device instance specific data.
*/

R_EXTERN r_rlin3_Dev_t r_rlin3_Dev[R_RLIN3_MACRO_NUM];


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_RLIN3_ErrorHandler

  Central error function. 
  
  When the driver detects any error, it will call this function.
  In case there is a registered error callback user function, 
  it  will be called. If a user handler is missing an endless loop is entered. 
     
  Parameters:
  Unit       - Instance number
  Error      - Error code, see: <r_rlin3_Error_t>
  
  Returns:
  void
*/

void R_RLIN3_ErrorHandler(uint32_t Unit, r_rlin3_Error_t Error);


#ifdef __cplusplus
}
#endif

#endif /* RLIN3_MAIN_H_  */
