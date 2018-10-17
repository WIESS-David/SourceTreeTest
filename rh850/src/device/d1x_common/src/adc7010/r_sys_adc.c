/*
****************************************************************************
PROJECT : ADC driver
FILE    : $Id$
============================================================================
DESCRIPTION
ADC driver system functions for r7f7010352
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2015
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
  Title: ADC7010 Support Functions

  Implementation of the macro driver ADC support functions  for the 
  device r7f701412.
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_adc_api.h"
#include "r_adc_sys.h"
#include "r_dev_api.h"
#include "r_config_adc.h"

#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)
   #include <stdio.h> 
   #include "r_dbg_api.h"
#endif


/*******************************************************************************
  Section: Local types 
*/

typedef struct 
{
    r_dev_Device_t Device; 
    uint8_t        AdceUnit;
    uint8_t        AdceChan; 
} loc_AdcDevConfig_t;  

/*******************************************************************************
  Section: Local Constants 
*/


/*******************************************************************************
  Constant: loc_DevConfig
  
  For each device list the ADC configuration  
*/
static const loc_AdcDevConfig_t loc_AdcDevConfig[] = {

    /* Note: due to the physical channel ID 37 for the TSN the number of channels reported 
       is a bit misleading as the are large gaps but the ADCE driver required this
    */

    /*  info                 Device               ADCE unit   ADCE channels                 */
    {/* D1L1_QFP100 */       R_DEV_R7F701400,     0,          38 }, /* 12 channels plus TSN */
    {/* D1L1        */       R_DEV_R7F701401,     0,          38 }, /* 16 channels plus TSN */
    {/* D1L2        */       R_DEV_R7F701402,     0,          38 }, /* 16 channels plus TSN */
    {/* D1L2H       */       R_DEV_R7F701403,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M1_3.75M  */       R_DEV_R7F701404,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M1_5M     */       R_DEV_R7F701405,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M1H_3.75M */       R_DEV_R7F701406,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M1H_5M    */       R_DEV_R7F701407,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M2_3.75M  */       R_DEV_R7F701408,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M2_5M     */       R_DEV_R7F701410,     0,          38 }, /* 16 channels plus TSN */
    {/* D1M2H_3.75M */       R_DEV_R7F701411,     0,          38 }, /* 20 channels plus TSN */
    {/* D1M2H_5M    */       R_DEV_R7F701412,     0,          38 }, /* 20 channels plus TSN */
    {/* D1M1A_4M    */       R_DEV_R7F701441,     0,          38 }, /* 20 channels plus TSN */
    {/* R7F701460   */       R_DEV_R7F701460,     0,          38 }, /* 20 channels plus TSN */
    {/* error */             R_DEV_UNKNOWN_DEVICE,0,           0 }  
};


/*******************************************************************************
  Function: R_SYS_ADC_GetDeviceIndex
*/
static uint8_t loc_AdcGetDeviceIndex(const r_dev_Device_t Id)
{
    int32_t i;

    for(i = 0u; i < sizeof(loc_AdcDevConfig) / sizeof(loc_AdcDevConfig_t); i++)
    {
        if(loc_AdcDevConfig[i].Device == Id)
        {
            return i;
        }
        else if(loc_AdcDevConfig[i].Device == R_DEV_UNKNOWN_DEVICE)
        {
            return i;
        }
    }    
    
    /* config number of R_DEV_UNKNOWN_DEVICE */
    return ((sizeof(loc_AdcDevConfig) / sizeof(loc_AdcDevConfig_t)) - 1);
}


/*******************************************************************************
  Section: Global Functions  
*/

/*******************************************************************************
  Function: R_SYS_ADC_GetBaseAddr
  
  See: <r_adc_sys.h> for details
*/

uint32_t R_SYS_ADC_GetBaseAddr(uint32_t Unit) 
{
    uint32_t addr = 0;
    
    switch (Unit)
    {
    case 0:
        addr = 0xfff20000u;  /* base address of Adc A instance 0 */ 
        break;
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: We don't have that unit");  
    }
    
    return addr;
}

/*******************************************************************************
  Function: R_SYS_ADC_Init
  
  See: <r_adc_sys.h> for details
*/

void R_SYS_ADC_Init(uint32_t Unit) 
{
    /* Enable interrupts */
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable (R_DEV_INT_ADC0SG1,  1); 
        R_DEV_IntEnable (R_DEV_INT_ADC0SG2,  1); 
        R_DEV_IntEnable (R_DEV_INT_ADC0SG3,  1);
        break;
        
    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: We do not have this Unit");      
    }    
}
    

/*******************************************************************************
  Function: R_SYS_ADC_DeInit
  
  See: <r_adc_sys.h> for details    
*/

void R_SYS_ADC_DeInit(uint32_t Unit)
{
    switch (Unit)
    {
    case 0:
        R_DEV_IntEnable (R_DEV_INT_ADC0SG1,  0); 
        R_DEV_IntEnable (R_DEV_INT_ADC0SG2,  0); 
        R_DEV_IntEnable (R_DEV_INT_ADC0SG3,  0);
        break;

    default:
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: We do not have this Unit");      
    }    
}
    

/*******************************************************************************
  Function: R_SYS_ADC_EnableInt
  
  See: <r_adc_sys.h> for details
*/

void R_SYS_ADC_EnableInt(uint32_t Unit, r_adc_Interrupt_t Interrupt) 
{
    r_dev_IntSel_t Int;
    if (Unit == 0)
    {
        switch (Interrupt)
        {
            case R_ADC_INT_CG0: Int = R_DEV_INT_ADC0SG1;
                                break;
            case R_ADC_INT_CG1: Int = R_DEV_INT_ADC0SG2;
                                break;
            case R_ADC_INT_CG2: Int = R_DEV_INT_ADC0SG3;
                                break;
            default           : R_DEV_ERROR((uint32_t)Interrupt, R_DEV_ERR_INTR, "ADC0: Invalid interrupt configuration!");                            
        }
        R_DEV_IntEnable (Int, 1);
    }
    else
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: Invalid Unit configuration!");
    }
}


/*******************************************************************************
  Function: R_SYS_ADC_DisableInt
  
  See: <r_adc_sys.h> for details
*/

void R_SYS_ADC_DisableInt(uint32_t Unit, r_adc_Interrupt_t Interrupt)
{
    r_dev_IntSel_t Int;
    if (Unit == 0)
    {
        switch (Interrupt)
        {
            case R_ADC_INT_CG0: Int = R_DEV_INT_ADC0SG1;
                                break;
            case R_ADC_INT_CG1: Int = R_DEV_INT_ADC0SG2;
                                break;
            case R_ADC_INT_CG2: Int = R_DEV_INT_ADC0SG3;
                                break;
            default           : R_DEV_ERROR((uint32_t)Interrupt, R_DEV_ERR_INTR, "ADC0: Invalid interrupt configuration!");                            
        }
        R_DEV_IntEnable (Int, 0);
    }
    else
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: Invalid Unit configuration!");
    }    
}


/*******************************************************************************
  Function: R_SYS_ADC_MUXAvailable
  
  See: <r_adc_sys.h> for details
*/

r_adc_Error_t R_SYS_ADC_MUXAvailable(uint32_t Unit, uint16_t MPXAddr)
{
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    
    /* mux not supported in this device */
    ret_val = R_ADC_PARAM_CONFIG;
    
    return ret_val;
}


/*******************************************************************************
  Function: R_SYS_ADC_SmpTimeAvailable
  
  See: <r_adc_sys.h> for details
*/

r_adc_Error_t R_SYS_ADC_SmpTimeAvailable(uint32_t Unit, uint16_t SmpTime)
{
    uint16_t      CheckSmpTime = 0;
    r_adc_Error_t ret_val      = R_ADC_ERR_NG;
    
    CheckSmpTime = SmpTime % 6;
    if (CheckSmpTime == 0 && SmpTime >= 24)
    {
        ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_PARAM_CONFIG;
        R_DEV_ERROR(Unit, R_DEV_ERR_NG, "ADC: Sampling time is invalid");
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_SYS_ADC_MaxChannelNum
  
  See: <r_adc_sys.h> for details
*/

uint8_t R_SYS_ADC_MaxChannelNum(uint32_t Unit)
{
    uint8_t       Max_ChNum;
    r_dev_Device_t Device;
    uint8_t        Index; 

    /* get the current device */
    Device = R_DEV_GetDev();
    
    /* get an index to the configuration */
    Index = loc_AdcGetDeviceIndex(Device);

    /* check its valid */
    if(R_DEV_UNKNOWN_DEVICE == loc_AdcDevConfig[Index].Device)
    {
        R_DEV_ERROR(Unit, R_DEV_ERR_IDX, "Device: Invalid!");
    }

    switch (loc_AdcDevConfig[Index].AdceUnit)
    {
        case 0 :  Max_ChNum = loc_AdcDevConfig[Index].AdceChan;

                  break;
        default:  R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: Invalid Unit occurs!");
    }
    return        Max_ChNum;
}


/*******************************************************************************
  Function: R_SYS_ADC_HwTrgNum
  
  See: <r_adc_sys.h> for details
*/

uint8_t R_SYS_ADC_HwTrgNum(uint32_t Unit)
{
    uint8_t       Hw_TrgNum;
    switch (Unit)
    {
        case 0 :  Hw_TrgNum = R_ADC_CG_HW_TRIG_NUM(0);
                  break;
        default:  R_DEV_ERROR(Unit, R_DEV_ERR_UNIT, "ADC: Invalid Unit occurs!");
    }
    return        Hw_TrgNum;
}


/*******************************************************************************
  Function: R_SYS_ADC_DbgSVSTOP
  
  see: <r_adc_sys.h> for details
*/

void R_SYS_ADC_DbgSVSTOP (r_adc_Emu_t ADC_Emu)
{
    if (ADC_Emu == R_ADC_SVSTOP_EFECTIVE)
    {
#if defined (R_DBG_PRINT_DEV_REGS) || defined (R_DBG_PRINT_MSG)    	
        R_DBG_InitSVSTOP(R_DBG_SVSTOP_EFECTIVE);
#endif        
    }
}
