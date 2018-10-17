/*
****************************************************************************
PROJECT : VLIB - BSP
FILE    : $Id: r_bsp_d1m1.c 13578 2017-05-29 13:31:52Z matthias.nippert $
============================================================================
DESCRIPTION
BSP Implementation D1M1 / D1M1H MANGO Board
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
Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

/*
  Section: BSP Description

    The "D1M1-Mango Board" Support Package is intended for the
    following board combination:
    
    BSP Board Name: "D1M1-MANGO Board"
        + D1M1 or D1M1H MANGO Adapter board
        + D1x MANGO Main board
*/


/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */

#include "r_gpio_api.h"
#include "r_dev_api.h"
#include "r_tick_api.h"
#include "r_bsp_api.h"


#ifdef USE_BSP_STDIO
#include "r_bsp_stdio_api.h"
#endif

#ifdef USE_BSP_HMI
#include "r_bsp_hmi_api.h"
#endif

#ifdef USE_BSP_GFX
#include "r_bsp_gfx_api.h"
#include "r_bsp_gfx_disp_init.h"    /* CSI init for NEC display */
#include "r_bsp_gfx_vi_saa71_init.h"    /* SAA71xx video input converter init */
#include "r_bsp_gfx_hdmi_out.h"     /* init HDMI output converters */
#include "r_bsp_gfx_iis_audio_dac.h"    /* IIS Audio Digital Analog converter init */
#endif /* USE_BSP_GFX */


/*******************************************************************************
  Section: Local Defines
*/

/*******************************************************************************
  Section: Local Constants 
*/

/*******************************************************************************
  Array loc_ClkSelectionCfg 
  
  contains the clock selectors configuration 
*/
static const r_dev_ClkSelConfig_t loc_ClkSelectionCfgD1M1[] = 
/*   Cks                   SrcId                Div  StpReqMsk      */
{
    {R_DEV_CKS_OSTM ,    R_DEV_CKS_SRC_MOSC,   2,   0u},
    {R_DEV_CKS_CLKJIT ,  R_DEV_CKS_SRC_DIV,    6,   0u},
    {R_DEV_CKS_PLLFIX ,  R_DEV_CKS_SRC_PLL1,   1,   0u},
    {R_DEV_CKS_CLKFIX ,  R_DEV_CKS_SRC_DIV,    6,   0u},
    {R_DEV_CKS_CPU    ,  R_DEV_CKS_SRC_PLL0,   3,   0u},
    {R_DEV_CKS_SFMA ,    R_DEV_CKS_SRC_PLL1,   3,   1u},    
    /* delimiter - do not remove */
    {R_DEV_CKS_LAST  ,     R_DEV_CKS_SRC_MOSC,  2,   0u}
}; 

static const r_dev_ClkSelConfig_t loc_ClkSelectionCfgD1M1H[] = 
/*   Cks                   SrcId                Div  StpReqMsk      */
{
    {R_DEV_CKS_OSTM ,    R_DEV_CKS_SRC_MOSC,   2,   0u},
    {R_DEV_CKS_CLKJIT ,  R_DEV_CKS_SRC_DIV,    5,   0u},
    {R_DEV_CKS_PLLFIX ,  R_DEV_CKS_SRC_PLL1,   1,   0u},
    {R_DEV_CKS_CLKFIX ,  R_DEV_CKS_SRC_DIV,    6,   0u},
    {R_DEV_CKS_CPU    ,  R_DEV_CKS_SRC_PLL0,   2,   0u},
#ifdef BSP_USE_SFMA_RAW_100MHZ    
    {R_DEV_CKS_SFMA ,    R_DEV_CKS_SRC_PLL0,   2,   0u},
#else
    {R_DEV_CKS_SFMA ,    R_DEV_CKS_SRC_PLL1,   3,   1u},    
#endif
    /* delimiter - do not remove */
    {R_DEV_CKS_LAST  ,     R_DEV_CKS_SRC_MOSC,  2,   0u}
}; 

static const r_dev_ClkSelConfig_t loc_ClkSelectionCfgD1M1A[] = 
/*   Cks                   SrcId                Div  StpReqMsk      */
{
    {R_DEV_CKS_OSTM ,    R_DEV_CKS_SRC_MOSC,   2,   0u},
    {R_DEV_CKS_CLKJIT ,  R_DEV_CKS_SRC_DIV,    6,   0u},
    {R_DEV_CKS_PLLFIX ,  R_DEV_CKS_SRC_PLL1,   1,   0u},
    {R_DEV_CKS_CLKFIX ,  R_DEV_CKS_SRC_DIV,    6,   0u},
    {R_DEV_CKS_CPU    ,  R_DEV_CKS_SRC_PLL0,   2,   0u},
    {R_DEV_CKS_SFMA ,    R_DEV_CKS_SRC_PLL0,   3,   0u},
    /* delimiter - do not remove */
    {R_DEV_CKS_LAST  ,     R_DEV_CKS_SRC_MOSC,  2,   0u}
};

/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: loc_InitClocksD1M1, loc_InitClocksD1M1H, loc_InitClocksD1M1A

  This function just contains the starting (global) clock config for the BSP. 
  Its purpose is to write the given setting in the structure passed as parameter.  
  
  As the clock config is done in low init, the globally stored data is cleared 
  afterwards. So, this function has to be called during and after the low init 
  in order to have the right settings afterwards. And becasue we want to keep data 
  consistant and do not have two configs, this funbction is used.  
  
  Parameters:
  ClkGenCfg - pointer to the Clock generator configuration, 
              see <r_dev_ClkGenConfig_t>
  
  Returns:
  none

*/
const r_dev_ClkSelConfig_t* loc_InitClocksD1M1(r_dev_ClkGenConfig_t * Config);
const r_dev_ClkSelConfig_t* loc_InitClocksD1M1(r_dev_ClkGenConfig_t * Config)
{
    /* Init Device Clocks  */
    Config->MainOsc.FrequencyHz          = R_DEV_MOSC_FRQ_HZ;
    Config->MainOsc.StabiTimeNs          = 400000;
    Config->MainOsc.Gain                 = 3;
    Config->MainOsc.Running              = 1;
    Config->MainOsc.StpReqMsk            = 1;
    Config->SubOsc.FrequencyHz           = 32768;
    Config->SubOsc.Running               = 1;
    Config->RoscStpReqMsk                = 1;                   
                  
    Config->Pll[0].Mode                  = R_DEV_PLL_SSCG;
    Config->Pll[0].DithMode              = R_DEV_PLL_DITH_CENTER;
    Config->Pll[0].DithRange             = 3;
    Config->Pll[0].ModFreq               = 50;
    Config->Pll[0].FrequencyHz           = 480 * 1000 * 1000;
    Config->Pll[0].StabiTimeNs           = 40000;
    Config->Pll[0].StpReqMsk             = 0;
                                            
    Config->Pll[1].Mode                  = R_DEV_PLL_ON;
    Config->Pll[1].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[1].DithRange             = 0;
    Config->Pll[1].ModFreq               = 0;
    Config->Pll[1].FrequencyHz           = 480 * 1000 * 1000;
    Config->Pll[1].StabiTimeNs           = 40000;
    Config->Pll[1].StpReqMsk             = 0;
                                            
    Config->Pll[2].Mode                  = R_DEV_PLL_OFF;
    Config->Pll[2].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[2].DithRange             = 0;
    Config->Pll[2].ModFreq               = 0;
    Config->Pll[2].FrequencyHz           = 0;
    Config->Pll[2].StabiTimeNs           = 0;
    Config->Pll[2].StpReqMsk             = 0;
    
    Config->BusEnable[R_DEV_CLK_XCC]     = 1;
    Config->BusEnable[R_DEV_CLK_PLL0]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL1]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL2]    = 0;
    Config->BusEnable[R_DEV_CLK_PLL0PIX] = 1;
    Config->BusEnable[R_DEV_CLK_SDRB]    = 0;
    Config->BusEnable[R_DEV_CLK_ETNBP]   = 1;
    Config->BusEnable[R_DEV_CLK_ETNBXCC] = 1;
    Config->BusEnable[R_DEV_CLK_MLBP]    = 0;
    Config->BusEnable[R_DEV_CLK_MLBXCC]  = 0;
    Config->BusEnable[R_DEV_CLK_RSCANP]  = 0;

    return (loc_ClkSelectionCfgD1M1);
}

const r_dev_ClkSelConfig_t* loc_InitClocksD1M1H(r_dev_ClkGenConfig_t * Config);
const r_dev_ClkSelConfig_t* loc_InitClocksD1M1H(r_dev_ClkGenConfig_t * Config)
{
    /* Init Device Clocks  */
    Config->MainOsc.FrequencyHz          = R_DEV_MOSC_FRQ_HZ;
    Config->MainOsc.StabiTimeNs          = 400000;
    Config->MainOsc.Gain                 = 3;
    Config->MainOsc.Running              = 1;
    Config->MainOsc.StpReqMsk            = 1;
    Config->SubOsc.FrequencyHz           = 32768;
    Config->SubOsc.Running               = 1;
    Config->RoscStpReqMsk                = 1;                   
                  
    Config->Pll[0].Mode                  = R_DEV_PLL_SSCG;
    Config->Pll[0].DithMode              = R_DEV_PLL_DITH_CENTER;
    Config->Pll[0].DithRange             = 3;
    Config->Pll[0].ModFreq               = 50;
    Config->Pll[0].FrequencyHz           = 400 * 1000 * 1000;
    Config->Pll[0].StabiTimeNs           = 40000;
    Config->Pll[0].StpReqMsk             = 0;
                                            
    Config->Pll[1].Mode                  = R_DEV_PLL_ON;
    Config->Pll[1].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[1].DithRange             = 0;
    Config->Pll[1].ModFreq               = 0;
    Config->Pll[1].FrequencyHz           = 480 * 1000 * 1000;
    Config->Pll[1].StabiTimeNs           = 40000;
    Config->Pll[1].StpReqMsk             = 0;
                                            
    Config->Pll[2].Mode                  = R_DEV_PLL_OFF;
    Config->Pll[2].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[2].DithRange             = 0;
    Config->Pll[2].ModFreq               = 0;
    Config->Pll[2].FrequencyHz           = 0;
    Config->Pll[2].StabiTimeNs           = 0;
    Config->Pll[2].StpReqMsk             = 0;
    
    Config->BusEnable[R_DEV_CLK_XCC]     = 1;
    Config->BusEnable[R_DEV_CLK_PLL0]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL1]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL2]    = 0;
    Config->BusEnable[R_DEV_CLK_PLL0PIX] = 1;
    Config->BusEnable[R_DEV_CLK_SDRB]    = 1;
    Config->BusEnable[R_DEV_CLK_ETNBP]   = 1;
    Config->BusEnable[R_DEV_CLK_ETNBXCC] = 1;
    Config->BusEnable[R_DEV_CLK_MLBP]    = 0;
    Config->BusEnable[R_DEV_CLK_MLBXCC]  = 0;
    Config->BusEnable[R_DEV_CLK_RSCANP]  = 0;

    return (loc_ClkSelectionCfgD1M1H);
}

const r_dev_ClkSelConfig_t* loc_InitClocksD1M1A(r_dev_ClkGenConfig_t * Config);
const r_dev_ClkSelConfig_t* loc_InitClocksD1M1A(r_dev_ClkGenConfig_t * Config)
{
    /* Init Device Clocks  */
    Config->MainOsc.FrequencyHz          = R_DEV_MOSC_FRQ_HZ;
    Config->MainOsc.StabiTimeNs          = 400000;
    Config->MainOsc.Gain                 = 3;
    Config->MainOsc.Running              = 1;
    Config->MainOsc.StpReqMsk            = 1;
    Config->SubOsc.FrequencyHz           = 32768;
    Config->SubOsc.Running               = 1;
    Config->RoscStpReqMsk                = 1;
    
    /* Note: D1M1A only allows SSCG with down-spread (at 480MHz) */
    Config->Pll[0].Mode                  = R_DEV_PLL_SSCG;
    Config->Pll[0].DithMode              = R_DEV_PLL_DITH_DOWN;
    Config->Pll[0].DithRange             = 3;
    Config->Pll[0].ModFreq               = 50;
    Config->Pll[0].FrequencyHz           = 480 * 1000 * 1000;
    Config->Pll[0].StabiTimeNs           = 40000;
    Config->Pll[0].StpReqMsk             = 0;
    
    Config->Pll[1].Mode                  = R_DEV_PLL_ON;
    Config->Pll[1].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[1].DithRange             = 0;
    Config->Pll[1].ModFreq               = 0;
    Config->Pll[1].FrequencyHz           = 480 * 1000 * 1000;
    Config->Pll[1].StabiTimeNs           = 40000;
    Config->Pll[1].StpReqMsk             = 0;
    
    Config->Pll[2].Mode                  = R_DEV_PLL_OFF;
    Config->Pll[2].DithMode              = R_DEV_PLL_DITH_FIXED;
    Config->Pll[2].DithRange             = 0;
    Config->Pll[2].ModFreq               = 0;
    Config->Pll[2].FrequencyHz           = 0;
    Config->Pll[2].StabiTimeNs           = 0;
    Config->Pll[2].StpReqMsk             = 0;
    
    Config->BusEnable[R_DEV_CLK_XCC]     = 1;
    Config->BusEnable[R_DEV_CLK_PLL0]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL1]    = 1;
    Config->BusEnable[R_DEV_CLK_PLL2]    = 0;
    Config->BusEnable[R_DEV_CLK_PLL0PIX] = 1;
    Config->BusEnable[R_DEV_CLK_SDRB]    = 1;
    Config->BusEnable[R_DEV_CLK_ETNBP]   = 1;
    Config->BusEnable[R_DEV_CLK_ETNBXCC] = 1;
    Config->BusEnable[R_DEV_CLK_MLBP]    = 0;
    Config->BusEnable[R_DEV_CLK_MLBXCC]  = 0;
    Config->BusEnable[R_DEV_CLK_RSCANP]  = 1;

    return (loc_ClkSelectionCfgD1M1A);
}
