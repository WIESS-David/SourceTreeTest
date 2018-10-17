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


/*******************************************************************************
  Title: RLIN3 Interrupt Functions

  Handle the RLIN3 interrupts.
*/

/*******************************************************************************
  Section: Includes
*/
#include "r_typedefs.h"
#include "r_cdi_api.h"
#include "r_config_rlin3.h"
#include "r_rlin3_regs.h"
#include "r_rlin3_api.h"
#include "r_rlin3_main.h"
#include "r_rlin3_sys.h"

  
/*******************************************************************************
  Section: Global API Functions
  
  For comments see file rlin3_api.h
*/

/*******************************************************************************
  Function: R_RLIN3_SetIsrCallback
*/

 r_rlin3_Error_t R_RLIN3_SetIsrCallback(
    uint32_t                Unit, 
    r_rlin3_Interrupt_t IntType,     
    void                (*Isr)(void)) 
{

    if (Unit >= R_RLIN3_MACRO_NUM) {
        return R_RLIN3_ERR_RANGE; 
    } 
    
    r_rlin3_Dev[Unit].IsrCallback[IntType] = Isr;
    return R_RLIN3_ERR_OK;
}

/********************************************************************************
  Function: R_RLIN3_IsrRE
*/

void R_RLIN3_IsrRE(uint32_t Unit) {
    volatile uint8_t  value;
    if (Unit >= R_RLIN3_MACRO_NUM) {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    } else {
        r_rlin3_Dev[Unit].TxOngoing = 0;
        if (r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RE] != 0) {
            r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RE]();
        }
        else {
            R_RLIN3_ReadChar(Unit);
        }
    }
}


/*******************************************************************************
  Function: R_RLIN3_IsrRx
*/

void R_RLIN3_IsrRx(uint32_t Unit) {
    uint8_t  value;
    uint32_t len;

    if (Unit >= R_RLIN3_MACRO_NUM) {
        R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    } else {    
        if (0 == r_rlin3_Dev[Unit].DrvExtension) 
        {
            if (r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RX] != 0) 
            {
                r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RX]();
            } 
            else 
            {
                value = R_RLIN3_ReadChar(Unit);
                len   = R_CDI_RbWrite(&r_rlin3_Dev[Unit].RxBuf, &value, 1);
                if (0 == len) 
                {
                    R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_BUFOVERRUN);
                }
            }
        } 
        else 
        { 
            if (r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RX] != 0) 
            {
                r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_RX]();
            }    
            value = R_RLIN3_ReadChar(Unit);
            len   = R_CDI_RbWrite(&r_rlin3_Dev[Unit].RxBuf, &value, 1);
            if (0 == len) 
            {
                R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_BUFOVERRUN);
            }
        }  
    }
}


/*******************************************************************************
  Function: R_RLIN3_IsrTx
*/

void R_RLIN3_IsrTx(uint32_t Unit) 
{
    uint32_t len;
    uint8_t  txdata;
    
    len = 0;
    if (Unit >= R_RLIN3_MACRO_NUM) 
    {
       R_RLIN3_ErrorHandler(Unit, R_RLIN3_ERR_RANGE);
    } 
    else 
    {    
        r_rlin3_Dev[Unit].TxOngoing = 0;
        if (0 == r_rlin3_Dev[Unit].DrvExtension) 
        {

            if (r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_TX] != 0) 
            {
                r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_TX]();
            } 
            else 
            {
                len = R_CDI_RbRead(&r_rlin3_Dev[Unit].TxBuf, &txdata, 1);
                if (0 != len) 
                { 
                    R_RLIN3_WriteChar(Unit, txdata);
                } 
                else 
                {
                    r_rlin3_Dev[Unit].TxRemain = 0;
                }
            }
        } 
        else 
        {
            if (r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_TX] != 0) 
            {
                r_rlin3_Dev[Unit].IsrCallback[R_RLIN3_INT_TX]();
            }
            len = R_CDI_RbRead(&r_rlin3_Dev[Unit].TxBuf, &txdata, 1);
            if (0 != len) 
            {                                
                R_RLIN3_WriteChar(Unit, txdata);
            } 
            else 
            {
                r_rlin3_Dev[Unit].TxRemain = 0;
            }
        }             
    }
}


/*******************************************************************************
  Function: R_RLIN3_EnableInt
*/

void R_RLIN3_EnableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt) 
{
    R_SYS_RLIN3_EnableInt(Unit, Interrupt); 
}


/*******************************************************************************
  Function: R_RLIN3_DisableInt
*/

void R_RLIN3_DisableInt(uint32_t Unit, r_rlin3_Interrupt_t Interrupt) 
{
    R_SYS_RLIN3_DisableInt(Unit, Interrupt);    
}
