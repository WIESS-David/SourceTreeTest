/*
****************************************************************************
PROJECT : RLIN3
FILE    : $Id: r_config_rlin3.h 3952 2014-09-24 09:52:47Z golczewskim $
============================================================================ 
DESCRIPTION
RLIN3 configuration for r7f7010352
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

#ifndef CONFIG_RLIN3_H_
#define CONFIG_RLIN3_H_

#if defined (R_DBG_PRINT_RLIN3_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif

/*******************************************************************************
  Title: RLIN3 Configuration
*/

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Constant: R_RLIN3_MACRO_NUM

  Number of RLIN3 macros in the device.
*/

#define R_RLIN3_MACRO_NUM         4

/*******************************************************************************
  Constant: R_RLIN3_TXBUF_SIZE

  Size of write buffer.
*/

#define R_RLIN3_TXBUF_SIZE        257

/*******************************************************************************
  Constant: R_RLIN3_RXBUF_SIZE

  Size of read buffer.
*/

#define R_RLIN3_RXBUF_SIZE        257

/*******************************************************************************
  Section: Global Macros
*/

/*******************************************************************************
  Macro: R_RLIN3_WRITE_REG

  Write RLIN3 register with predefined size.
  SIZE allowed values are 8, 16, 32
*/
#ifdef R_DBG_PRINT_RLIN3_REGS
    #define R_RLIN3_WRITE_REG(SIZE, ADDR, VALUE) R_DBG_PrintRegCmd((SIZE), (ADDR), (VALUE));  *((volatile uint##SIZE##_t*)(ADDR)) = (VALUE)
#else
    #define R_RLIN3_WRITE_REG(SIZE, ADDR, VALUE)  (*((volatile uint##SIZE##_t*)(ADDR)) = (VALUE))
#endif

#define R_RLIN3_WRITE_REG_SILENT(SIZE, ADDR, VALUE)  {(*((volatile uint##SIZE##_t*)(ADDR)) = (VALUE));}


/*******************************************************************************
  Macro: R_RLIN3_READ_REG

  Read RLIN3 register with predefined size.
  SIZE allowed values are 8, 16, 32
*/

#ifdef R_DBG_PRINT_RLIN3_REGS
    #define R_RLIN3_READ_REG(SIZE, ADDR) R_DBG_PrintRegRead##SIZE##(ADDR)
#else
    #define R_RLIN3_READ_REG(SIZE, ADDR)  (*((volatile uint##SIZE##_t*)((ADDR)))) 
#endif


#endif /* CONFIG_RLIN3_H_ */
