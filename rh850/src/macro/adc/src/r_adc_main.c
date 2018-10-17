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

/*******************************************************************************
  Title: ADC7010 Main Module

  Implementation of the macro driver ADC main functions 
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_dev_api.h"
#include "r_adc7010_regs.h"
#include "r_config_adc.h"
#define ADCA_MAIN_GLOBALS_
#include "r_adc_api.h"
#include "r_adc_main.h"
#undef ADCA_MAIN_GLOBALS_
#include "r_adc_sys.h"

/*******************************************************************************
  Section: Local Macros
*/
/*******************************************************************************
  Macro: R_ADC_ERROR 

  Display an additional error message (in case od activeated Debug output)
  And call the ADCA error handler  
  
 */
#define R_ADC_ERROR(Unit, AdcErr, Txt)  {R_DISPLAY_ERROR(Txt);R_ADC_ErrorHandler(Unit,\
                                         (r_adc_Error_t)AdcErr);} 

/*******************************************************************************
  Section: Local Variables
*/

/*******************************************************************************
  Variable: loc_ErrorHandler

  A function pointer to a user callback (Applications error handler)  
  
  The value is assigned by the application using the driver 
*/

static void (*loc_ErrorHandler )(uint32_t Unit, r_adc_Error_t Error) = 0;


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_ADC_Init
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_Init(uint32_t Unit, r_adc_Config_t * Config) 
{
    r_adc_Group_t      ScanGroup;
    uint8_t            VChannel;
    r_adc_ULLimitCfg_t Ull_Num;
    r_adc_Error_t      ret_val    = R_ADC_ERR_NG;
    
    if (r_adc_InitFlag[Unit] == R_ADC_NO_INIT)
    {
        if (Unit < R_ADC_MACRO_NUM)
        {
            r_adc_Unit[Unit].BaseAddr = R_SYS_ADC_GetBaseAddr(Unit); 
            
            /* common config */
            ret_val       = R_ADC_Config(Unit, Config);
            if (ret_val != R_ADC_ERR_OK)
            {
                R_ADC_ERROR(Unit, R_ADC_ERR_NG, "ADC: General configuration error occurs");
            }
            
            for (Ull_Num = R_ADC_ULL_0; Ull_Num < R_ADC_ULL_0; Ull_Num++)
            {
                ret_val      = R_ADC_AssignULL(Unit, Ull_Num, Config);
                if (ret_val != R_ADC_ERR_OK)
                {
                    R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Upper-lower limit configeration error occurs");
                }
            }
            
            
            /* group config */
            for (VChannel = 0; VChannel < R_SYS_ADC_MaxChannelNum(Unit); VChannel ++)
            {
                ret_val   = R_ACD_ConfigVC (Unit, VChannel, Config);
                if (ret_val != R_ADC_ERR_OK)
                {
                    R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Virtual cahnnel configeration error occurs");
                }
            }
            
            for (ScanGroup = R_ADC_CG_1; ScanGroup < R_ADC_CG_PWM; ScanGroup ++)
            {
                ret_val   = R_ADC_ConfigSG(Unit, ScanGroup, Config);
                if (ret_val != R_ADC_ERR_OK)
                {
                    R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Scan group configeration error occurs");
                }
                 
            }
            R_SYS_ADC_Init(Unit);
            r_adc_InitFlag[Unit]      = R_ADC_INIT;
            ret_val                   = R_ADC_ERR_OK;       
        }
        else
        {
            R_ADC_ErrorHandler (Unit, R_ADC_ERR_RANGE);
        }
    }
    else
    {
        R_ADC_ErrorHandler (Unit, R_ADC_ALREADY_INITIALIZED);
    }
    return ret_val; 
}


/*******************************************************************************
  Function: R_ADC_ResetCfgParams
  
  see: <r_adc_api.h> for details
*/
void R_ADC_ResetCfgParams (r_adc_Config_t * Config)
{
    r_adc_Group_t      ScanGroup;
    uint8_t            VChannel;
    uint8_t            Unit;
    r_adc_ULLimitCfg_t Ull_Num;
    Config->Res                            = R_ADC_12BIT_RES;
    Config->ResTreat                       = R_ADC_RESULT_RETAIN;
    Config->Align                          = R_ADC_RIGHT_ALIGNED;
    Config->SuspendMode                    = R_ADC_SYNC_SUSPEND;
    Config->SDVol_En                       = R_ADC_SD_OFF; 
    Config->SmpTime                        = 24;
    Config->ULErrInfo                      = R_ADC_DONT_GENERATE_ERR; 
    Config->OWErrInfo                      = R_ADC_DONT_GENERATE_ERR;
    for (ScanGroup = R_ADC_CG_1; ScanGroup < R_ADC_CG_PWM; ScanGroup ++)
    {
        Config->Group[ScanGroup].VcStart   = 0; 
        Config->Group[ScanGroup].VcEnd     = 2;
        Config->Group[ScanGroup].Mode      = R_ADC_MULTICYCLE_MODE;
        Config->Group[ScanGroup].SGNum     = R_ADC_SGCONV_ONCE;   
        Config->Group[ScanGroup].Trigger   = R_ADC_SW_TRIGGER;
        Config->Group[ScanGroup].Int_SGEnd = R_ADC_COVEND_IGNORED;
        Config->Group[ScanGroup].ConvNum   = R_ADC_CONVERT_ONCE;
        for (VChannel = 0; VChannel < R_ADC_CHANNEL_MAX_NUM; VChannel ++)
        {
            r_adc_MpxNum[ScanGroup][VChannel] = 0;
        }        
    }
    for (VChannel = 0; VChannel < R_ADC_CHANNEL_MAX_NUM; VChannel ++)
    {
        Config->VCh[VChannel].MPXAddr      = 0;
        Config->VCh[VChannel].phyChannel   = 0;
        Config->VCh[VChannel].MPXEnable    = R_ADC_NO_MPX;
        Config->VCh[VChannel].INT_VCEnd    = R_ADC_COVEND_IGNORED;
        Config->VCh[VChannel].ULLCheck     = R_ADC_NO_ULL;
        Config->VCh[VChannel].phyChPD      = R_ADC_PULLDN_OFF; 
    }
    for (Ull_Num = R_ADC_ULL_0; Ull_Num < R_ADC_ULL_LAST; Ull_Num++)
    {
        Config->ULL[Ull_Num].Upper_Limit                   = 0xfff;
        Config->ULL[Ull_Num].Lower_Limit                   = 0;
    }
    for (Unit = 0; Unit < R_ADC_MACRO_NUM; Unit ++)
    {
        for (ScanGroup = R_ADC_CG_1; ScanGroup < R_ADC_CG_PWM; ScanGroup ++)
        {
            for (VChannel = 0; VChannel < R_ADC_CHANNEL_MAX_NUM; VChannel ++)
            {
                r_adc_CovResult[Unit][ScanGroup][VChannel] = 0;
                r_adc_VChannel[Unit][ScanGroup][VChannel]  = 0;
            }
        }
        for (VChannel = 0; VChannel < R_ADC_CHANNEL_MAX_NUM; VChannel ++)
        {
            r_adc_UllErr[Unit][VChannel]                   = 0;
            r_adc_OwErr[Unit][VChannel]                    = 0;
        }
    }
}


/*******************************************************************************
  Function: R_ADC_ResetSDCfgParams
  
  see: <r_adc_api.h> for details
*/
void R_ADC_ResetSDCfgParams (r_adc_Config_t * Config)
{
    uint8_t            VChannel;
    for (VChannel = 0; VChannel < R_ADC_CHANNEL_MAX_NUM; VChannel ++)
    { 
        Config->VCh[VChannel].SDparams.SDmode  = R_ADC_SD_HOLD;
        Config->VCh[VChannel].SDparams.SDinput = R_ADC_SD_ANIN;
        Config->VCh[VChannel].SDparams.volmode = 0;
    }
}


/*******************************************************************************
  Function: R_ADC_DeInit
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_DeInit(uint32_t Unit)
{
    r_adc_Group_t      ScanGroup;
    uint8_t            VChannel;
    r_adc_ULLimitCfg_t Ull_Num;
    r_adc_Config_t     ResetCfg;
    r_adc_Error_t      ret_val = R_ADC_ERR_NG;
    
    if (Unit < R_ADC_MACRO_NUM)
    {
        R_ADC_Halt(Unit);
        R_ADC_ResetCfgParams(&ResetCfg);
        
        ret_val     = R_ADC_Config(Unit, &ResetCfg);
        if (ret_val != R_ADC_ERR_OK)
        {
            R_ADC_ERROR(Unit, R_ADC_ERR_NG, "ADC: General Reset error occurs");
        }
        
        for (Ull_Num = R_ADC_ULL_0; Ull_Num < R_ADC_ULL_LAST; Ull_Num++)
        {
            ret_val      = R_ADC_AssignULL(Unit, Ull_Num, &ResetCfg);
            if (ret_val != R_ADC_ERR_OK)
            {
                R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Upper-lower limit Reset error occurs");
            }
        }
        
        
        /* group config */
        for (VChannel = 0; VChannel < R_SYS_ADC_MaxChannelNum(Unit); VChannel ++)
        {
            ret_val   = R_ACD_ConfigVC (Unit, VChannel, &ResetCfg);
            if (ret_val != R_ADC_ERR_OK)
            {
                R_ADC_ERROR(Unit, R_ADC_ERR_NG, "ADC: Virtual channel Reset error occurs");
            }
        }
        
        for (ScanGroup = R_ADC_CG_1; ScanGroup < R_ADC_CG_PWM; ScanGroup ++)
        {
            ret_val     = R_ADC_ConfigSG(Unit, ScanGroup, &ResetCfg);
            if (ret_val != R_ADC_ERR_OK)
            {
                R_ADC_ERROR(Unit, R_ADC_ERR_NG, "ADC: Scan group Reset error occurs");
            }   
        }
        R_SYS_ADC_DeInit(Unit);
        r_adc_InitFlag[Unit]      = R_ADC_NO_INIT;
        ret_val                   = R_ADC_ERR_OK;
    }
    else
    {
        ret_val                   = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ACD_Config
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_Config(uint32_t Unit, r_adc_Config_t *Config)
{
    uint32_t      addr    = 0;
    uint32_t      reg_val = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;

    if (Unit < R_ADC_MACRO_NUM)
    {
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_ADCR;
        reg_val = (Config->Align << 5u) | (Config->Res << 4u) | (Config->SuspendMode);
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        if (R_SYS_ADC_SmpTimeAvailable(Unit, (Config->SmpTime)) == R_ADC_ERR_OK)
        {
            addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SMPCR;
            reg_val = Config->SmpTime;
            R_ADC_WRITE_REG(16, addr, reg_val);
        }
        
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SFTCR;
        reg_val = (Config->ResTreat << 4u) | (Config->ULErrInfo << 3u) | (Config->OWErrInfo << 2u);
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        r_adc_OWInitFlag[Unit] = Config->OWErrInfo;
              
        ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ACD_ConfigVC
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ACD_ConfigVC (uint32_t    Unit, 
                              uint8_t          Channel, 
                              r_adc_Config_t * ChCfg)
{
    uint32_t      addr    = 0;
    uint32_t      reg_val = 0;
    uint32_t      temp = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
        addr = r_adc_Unit[Unit].BaseAddr + R_ADC_VCR(Channel);
        if (Channel < R_SYS_ADC_MaxChannelNum(Unit))
        {
            if ((ChCfg->VCh[Channel]).MPXEnable)
            {
                if (R_SYS_ADC_MUXAvailable(Unit, ((ChCfg->VCh[Channel]).MPXAddr)) == R_ADC_ERR_OK)
                {
                    reg_val = ((ChCfg->VCh[Channel]).MPXEnable << 15u) | ((ChCfg->VCh[Channel]).MPXAddr << 12u);
                }
                else
                {
                    R_ADC_ERROR(Unit, R_ADC_ERR_NG, "ADC: Multiplexer configeration error occurs");
                }
            }

            if ((ChCfg->VCh[Channel]).phyChannel < R_SYS_ADC_MaxChannelNum(Unit))
            {
                reg_val |= ((ChCfg->VCh[Channel]).INT_VCEnd << 8u) | ((ChCfg->VCh[Channel]).ULLCheck << 6u) | ((ChCfg->VCh[Channel]).phyChannel);
                R_ADC_WRITE_REG(32, addr, reg_val);
                r_adc_ChEndFlag[Unit] = ((ChCfg->VCh[Channel]).INT_VCEnd);
                addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_PDCTL2;
                /* enable PD */
                if ((ChCfg->VCh[Channel]).phyChPD)
                {
                    if ((ChCfg->VCh[Channel]).phyChannel <= 15u)
                    {
                        reg_val = (ChCfg->VCh[Channel]).phyChPD << (ChCfg->VCh[Channel]).phyChannel + 16;
                        temp = R_ADC_READ_REG(32, addr);
                        R_ADC_WRITE_REG(32, addr, reg_val | temp);
                    }
                    else if ((ChCfg->VCh[Channel]).phyChannel >= 16)
                    {
                        reg_val = (ChCfg->VCh[Channel]).phyChPD << ((ChCfg->VCh[Channel]).phyChannel - 16);
                        temp = R_ADC_READ_REG(32, addr);
                        R_ADC_WRITE_REG(32, addr, reg_val | temp);
                    }
                    else
                    {}

                    ret_val = R_ADC_ERR_OK;
                }
                else /* disable PD */
                {
                    if ((ChCfg->VCh[Channel]).phyChannel <= 15u)
                    {
                        reg_val = 1 << (ChCfg->VCh[Channel]).phyChannel + 16;
                        temp = R_ADC_READ_REG(32, addr);
                        reg_val = (~reg_val & 0xFFFFFFFFul) & temp;
                        R_ADC_WRITE_REG(32, addr, reg_val);
                    }
                    else if ((ChCfg->VCh[Channel]).phyChannel >= 16)
                    {
                        reg_val = 1 << (ChCfg->VCh[Channel]).phyChannel - 16;
                        temp = R_ADC_READ_REG(32, addr);
                        reg_val = (~reg_val & 0xFFFFFFFFul) & temp;
                        R_ADC_WRITE_REG(32, addr, reg_val);
                    }
                    else
                    {}

                    ret_val = R_ADC_ERR_OK;
                }
            }
            else
            {
                ret_val = R_ADC_PARAM_CONFIG;
            }
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_AssignULL
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_AssignULL(uint32_t              Unit, 
                              r_adc_ULLimitCfg_t Ull_Num,
                              r_adc_Config_t *   Config)
{
    uint32_t            addr    = 0;
    uint32_t            reg_val = 0;
    r_adc_Error_t       ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    { 
        if (Ull_Num < R_ADC_ULL_LAST)
        {
            if ((Config->ULL[Ull_Num]).Upper_Limit == 0xfff && (Config->ULL[Ull_Num]).Lower_Limit == 0)
            {
                ret_val = R_ADC_ERR_OK;    /* do nothing, use the intial value if the Upper-Lower Limit is not set */
            }
            else
            {
                if ((Config->ULL[Ull_Num]).Upper_Limit > (Config->ULL[Ull_Num]).Lower_Limit)
                {
                    reg_val  = ((Config->ULL[Ull_Num]).Upper_Limit << 20) | ((Config->ULL[Ull_Num]).Lower_Limit << 4);
                    switch (Ull_Num)
                    {
                        case R_ADC_ULL_0: addr = r_adc_Unit[Unit].BaseAddr + R_ADC_ULLMTBR0;
                                          R_ADC_WRITE_REG(32, addr, reg_val);
                                          ret_val  = R_ADC_ERR_OK;
                                          break;
                        case R_ADC_ULL_1: addr = r_adc_Unit[Unit].BaseAddr + R_ADC_ULLMTBR1;
                                          R_ADC_WRITE_REG(32, addr, reg_val);
                                          ret_val  = R_ADC_ERR_OK;
                                          break;
                         case R_ADC_ULL_2: addr = r_adc_Unit[Unit].BaseAddr + R_ADC_ULLMTBR2;
                                          R_ADC_WRITE_REG(32, addr, reg_val);
                                          ret_val  = R_ADC_ERR_OK;
                                          break;
                        default         : ret_val = R_ADC_PARAM_CONFIG;
                    }
                }
                else
                {
                    ret_val  = R_ADC_PARAM_CONFIG;
                }
            }
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
} 


/*******************************************************************************
  Function: R_ADC_ConfigSG
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_ConfigSG(uint32_t    Unit,
                             r_adc_Group_t    ScanGroup,
                             r_adc_Config_t*  Config)
{
    uint32_t      addr    = 0;
    uint32_t      reg_val = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
        if (R_ADC_CG_1 <= ScanGroup && ScanGroup < R_ADC_CG_PWM)
        {
            addr    =  r_adc_Unit[Unit].BaseAddr + R_ADC_SGCR((ScanGroup));
            reg_val = ((Config->Group[ScanGroup]).Mode << 5) | ((Config->Group[ScanGroup]).Int_SGEnd  << 4) | ((Config->Group[ScanGroup]).ConvNum << 2) | ((Config->Group[ScanGroup]).Trigger);
            R_ADC_WRITE_REG(32, addr, reg_val);
        
            if (!(Config->Group[ScanGroup]).Mode)
            {
                addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGMCYCR((ScanGroup));
                reg_val = (Config->Group[ScanGroup]).SGNum;
                R_ADC_WRITE_REG(32, addr, reg_val);
            }
            
            if ((Config->Group[ScanGroup]).VcEnd < R_SYS_ADC_MaxChannelNum(Unit) && (Config->Group[ScanGroup]).VcStart <= (Config->Group[ScanGroup]).VcEnd)
            {
                addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGVCSP((ScanGroup));
                reg_val = (Config->Group[ScanGroup]).VcStart;
                R_ADC_WRITE_REG(32, addr, reg_val);
            
                addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGVCEP((ScanGroup));
                reg_val = (Config->Group[ScanGroup]).VcEnd;
                R_ADC_WRITE_REG(32, addr, reg_val);
                
                ret_val = R_ADC_ERR_OK;
            }
            else
            {
                ret_val = R_ADC_PARAM_POINTER;
            }
        }
        else
        {
            ret_val = R_ADC_PARAM_GROUP;
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
} 


/*******************************************************************************
  Function: R_ADC_StartGroupConversion
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_StartGroupConversion(uint32_t       Unit,
                                         r_adc_Group_t  ChGr)
{
    uint32_t      addr    = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;

    if (Unit < R_ADC_MACRO_NUM)
    {
        if (r_adc_InitFlag[Unit])
        {
            if (r_adc_StrFlag[Unit])
            {
                ret_val = R_ADC_BUSY;
            }
            else
            {
                switch (ChGr)
                {
                    case R_ADC_CG_1  :
                    case R_ADC_CG_2  :
                    case R_ADC_CG_3  : 
                    case R_ADC_CG_PWM: addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGSTCR(ChGr);
                                       R_ADC_WRITE_REG(8, addr, 0x1);                                      
                                       r_adc_StrFlag[Unit] = R_ADC_STR;
                                       ret_val = R_ADC_ERR_OK;
                                       break;                                                       
                    default          : ret_val = R_ADC_WRONG_TRIGG_SRC;
                }
            }
        }
        else
        {
            ret_val = R_ADC_UNINIT;
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_Halt
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_Halt(uint32_t Unit)
{
    uint32_t      addr    = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
        if (r_adc_StrFlag[Unit])
        {
            addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_ADHALTR;
            R_ADC_WRITE_REG(32, addr, 0x1);
            r_adc_StrFlag[Unit] = R_ADC_NO_STR;
            ret_val = R_ADC_ERR_OK;
        }
        else
        {
            ret_val = R_ADC_IDLE;
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_EnableHwTrigger
  
  see: <r_adc_api.h> for details
*/
void R_ADC_EnableHwTrigger(uint32_t          Unit,
                           r_adc_Group_t     ChGr,
                           r_adc_HWTrigger_t HwTrg)
{
    uint32_t       addr    = 0;
    uint32_t       reg_val = 0;
    
    if (Unit < R_ADC_MACRO_NUM)
    {
        addr    =  r_adc_Unit[Unit].BaseAddr + R_ADC_SGCR(ChGr);
        reg_val =  R_ADC_READ_REG(32, addr) | 0x01;
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        if (HwTrg < R_SYS_ADC_HwTrgNum(Unit))
        {
            switch (ChGr)
            {
                case R_ADC_CG_0  :
                case R_ADC_CG_1  :
                case R_ADC_CG_2  :
                case R_ADC_CG_3  :
                     addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGTSEL(ChGr);
                     reg_val = 0x01 << HwTrg;
                     R_ADC_WRITE_REG(32, addr, reg_val);
                     break;
                default          : 
                    R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, 
                                "ADC: Invalid Scan Group for Hardware Trigger");
                    break; 
            }
        }
    }
    else
    {
        R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Invalid ADC Unit for Hardware Trigger");
    }
}


/*******************************************************************************
  Function: R_ADC_DisableHwTrigger
  
  see: <r_adc_api.h> for details
*/
void R_ADC_DisableHwTrigger(uint32_t          Unit,
                            r_adc_Group_t     Group)
{
    uint32_t       addr    = 0;
    uint32_t       reg_val = 0;

    if (Unit < R_ADC_MACRO_NUM)
    {
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGCR(Group);
        reg_val = R_ADC_READ_REG(32, addr) & (~0x01);
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGTSEL(Group);
        R_ADC_WRITE_REG(32, addr, 0x0);
    }
    else
    {
        R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Invalid ADC Unit for Hardware Trigger");
    }
}


/*******************************************************************************
  Function: R_ADC_SDiagInit
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ACD_SDiagInit (uint32_t    Unit, 
                               r_adc_Config_t * SDCfg)
{
    uint32_t          addr    = 0;
    volatile uint32_t reg_val = 0;
    uint8_t           Channel = 0;
    r_adc_Error_t ret_val     = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
        addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_ADCR;
        reg_val      = R_ADC_READ_REG(32, addr);
        if (SDCfg->SDVol_En)
        {
            reg_val |= 0x80;
            R_ADC_WRITE_REG(32, addr, reg_val);
        }
        
        for (Channel = 0; Channel < R_SYS_ADC_MaxChannelNum(Unit); Channel ++)
        {
            /* this is device dependant let the config decide if its a diag channel */
            if (SDCfg->VCh[Channel].SDparams.SDinput == R_ADC_SD_DIAG)
            {
                /* Hold value mode is only for virtual channel 33-35 */
                if (Channel >= 33)
                {
                    addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_VCR(Channel);
                    reg_val      = R_ADC_READ_REG(32, addr) | ((SDCfg->VCh[Channel]).SDparams.SDmode << 9);
                    if (reg_val != 0)
                    {
                    R_ADC_WRITE_REG(32, addr, reg_val);
                    }
                }
                
                /* select voltage mode and function input, if the voltage circuit is selected on */
                if (SDCfg->SDVol_En)
                {
                    addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_DGCTL0;
                    reg_val  = (SDCfg->VCh[Channel]).SDparams.volmode;
                    R_ADC_WRITE_REG(32, addr, reg_val);
                    
                    if ((SDCfg->VCh[Channel]).SDparams.SDinput)
                    {
                        addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_DGCTL1;
                        reg_val  = R_ADC_READ_REG(32, addr);

                        /* there are only 16 diagnostics channels but the physical channels can map
                           where ADCEnIO0 is 16, ADCEnIO1 is 17, so cater for this */                           
                        if( ((SDCfg->VCh[Channel]).phyChannel) >= 16)
                        {
                            reg_val |= (0x1 << (((SDCfg->VCh[Channel]).phyChannel) - 16) );
                        }
                        else
                        {
                            reg_val |= (0x1 << ((SDCfg->VCh[Channel]).phyChannel));
                        }

                        R_ADC_WRITE_REG(32, addr, reg_val);
                    }
                }
                
                ret_val = R_ADC_ERR_OK;
            }
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val; 
}


/*******************************************************************************
  Function: R_ADC_SDiagDeInit
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_SDiagDeInit(uint32_t Unit)
{
    uint32_t           addr    = 0;
    volatile uint32_t  reg_val = 0;
    uint8_t            Channel = 0;
    r_adc_Error_t      ret_val = R_ADC_ERR_NG;
    r_adc_Config_t     ResetSDiag;

        if (Unit < R_ADC_MACRO_NUM)
    {
        R_ADC_ResetSDCfgParams(&ResetSDiag);
        addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_ADCR;
        reg_val      = R_ADC_READ_REG(32, addr);

        reg_val &= 0x7f;
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        for (Channel = 33; Channel < R_SYS_ADC_MaxChannelNum(Unit); Channel ++)
        {
            addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_VCR(Channel);
            reg_val      = R_ADC_READ_REG(32, addr);
            reg_val     &= ~(0x1 << 9);
            R_ADC_WRITE_REG(32, addr, reg_val);
        }
                
        /* select voltage mode and function input, if the voltage circuit is selected on */
        addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_DGCTL0;
        R_ADC_WRITE_REG(32, addr, 0x0);
        
        addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_DGCTL1;
        R_ADC_WRITE_REG(32, addr, 0x0);
        
        ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val; 
}


/***********************************************************
  Function: R_ADC_EnablePwmTrg
  
  see: <r_adc_api.h> for details
*/
void R_ADC_EnablePwmTrg (uint32_t Unit, r_adc_PwmTrigger_t PWM_En)
{
    uint32_t       addr    = 0;
    uint32_t       reg_val = 0;
    if (Unit < R_ADC_MACRO_NUM)
    {
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_PWDSGCR;
        reg_val = PWM_En;
        R_ADC_WRITE_REG(32, addr, reg_val);
    }
    else
    {
        R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Invalid ADC Unit for Self Diagnose");
    }
}


/*******************************************************************************
  Function: R_ADC_ConfigTH
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_ConfigTH(uint32_t    Unit,
                             r_adc_THGroup_t  THGroup, 
                             r_adc_THCfg_t *  THCfg)
{
    uint32_t       addr    = 0;
    uint32_t       reg_val = 0;
    r_adc_Error_t  ret_val = R_ADC_ERR_NG;
    
    if (Unit == 0)
    {
        addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THCR;
        reg_val  = THCfg->TH_SmpMsk;
        R_ADC_WRITE_REG(32, addr, reg_val);
        
        addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THER;
        reg_val  = (THCfg->TH[THGroup]).TH_E;
        if ((THCfg->TH[THGroup]).TH_E == R_ADC_TH_ENABLE)
        {
            reg_val  |= 0x1 << ((THCfg->TH[THGroup]).TH_Ch);
            R_ADC_WRITE_REG(32, addr, reg_val);
            
            switch (THGroup)
            {
                case R_ADC_THG_A: addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THGSR;
                                  reg_val &= ~(0x01 << (THCfg->TH[THGroup]).TH_Ch);
                                  R_ADC_WRITE_REG(32, addr, reg_val);
                              
                                  addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THACR;
                                  reg_val  = ((THCfg->TH_EndTrg) << 5) | ((THCfg->TH_HldTrg) << 4) | ((THCfg->TH_SgGroup));
                                  R_ADC_WRITE_REG(32, addr, reg_val);
                                  
                                  ret_val = R_ADC_ERR_OK;
                                  break;
                               
                case R_ADC_THG_B: addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THGSR;
                                  reg_val |= 0x01 << (THCfg->TH[THGroup]).TH_Ch;
                                  R_ADC_WRITE_REG(32, addr, reg_val);
                              
                                  addr     = r_adc_Unit[Unit].BaseAddr + R_ADC_THBCR;
                                  reg_val  = ((THCfg->TH_EndTrg) << 5) | ((THCfg->TH_HldTrg) << 4) | ((THCfg->TH_SgGroup));
                                   R_ADC_WRITE_REG(32, addr, reg_val);
                                   
                                   ret_val = R_ADC_ERR_OK;
                                  break;
                default         : ret_val = R_ADC_ERR_RANGE;
            }
        }
        else
        {
            reg_val  &= ~(0x1 << ((THCfg->TH[THGroup]).TH_Ch));
            R_ADC_WRITE_REG(32, addr, reg_val);
            ret_val = R_ADC_ERR_OK;
        }  
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_THDisable
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_THDisable(uint32_t Unit)
{
    uint32_t       addr    = 0;
    r_adc_Error_t  ret_val = R_ADC_ERR_NG;
    
    if (Unit == 0)
    {
        addr = r_adc_Unit[Unit].BaseAddr + R_ADC_THER;
        R_ADC_WRITE_REG(32, addr, 0x0);
        ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_StartTrgTH
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_StartTrgTH(uint32_t Unit)
{
    uint32_t       addr    = 0;
    r_adc_Error_t  ret_val = R_ADC_ERR_NG;
    
    if (Unit == 0)
    {
        addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_THSMPSTCR;
        R_ADC_WRITE_REG(32, addr, 0x1);
        ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_HoldTrgTH
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_HoldTrgTH(uint32_t    Unit,
                              r_adc_THGroup_t  THGroup)
{
    uint32_t       addr    = 0;
    r_adc_Error_t  ret_val = R_ADC_ERR_NG;
    
    if (Unit == 0)
    {
        switch (THGroup)
        {
            case R_ADC_THG_A: addr = r_adc_Unit[Unit].BaseAddr + R_ADC_THAHLDSTCR;
                              R_ADC_WRITE_REG(32, addr, 0x1);
                              ret_val = R_ADC_ERR_OK;
                              break;
            case R_ADC_THG_B: addr = r_adc_Unit[Unit].BaseAddr + R_ADC_THBHLDSTCR;
                              R_ADC_WRITE_REG(32, addr, 0x1);
                              ret_val = R_ADC_ERR_OK;
                              break;
            default         : ret_val = R_ADC_ERR_RANGE;
        }
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_ReadRes
  
  see: <r_adc_api.h> for details
*/
void R_ADC_ReadRes (uint32_t Unit, r_adc_ResParams_t* ConvRes)
{
    uint32_t          addr    = 0;
    uint32_t          reg_val = 0;
    uint8_t           MPX_E;
    uint8_t           ScanGroupSP;
    uint8_t           ScanGroupEP;
    uint8_t           Ch_Num;
    volatile uint16_t New_Result;
    
    if (ConvRes->ScanGroup == R_ADC_CG_LAST || ConvRes->ScanGroup == R_ADC_CG_PWM)
    {}
    else
    {
        addr             = r_adc_Unit[Unit].BaseAddr + R_ADC_SGVCSP(ConvRes->ScanGroup);
        ScanGroupSP      = R_ADC_READ_REG(8, addr);
        addr             = r_adc_Unit[Unit].BaseAddr + R_ADC_SGVCEP(ConvRes->ScanGroup);
        ScanGroupEP      = R_ADC_READ_REG(8, addr);
        Ch_Num           = ScanGroupSP;
        if (r_adc_ChEndFlag[Unit])
        {
            do
            {
                addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_DIR(Ch_Num);
                reg_val      = R_ADC_READ_REG(32, addr);
                New_Result   = reg_val & 0xffff;
                if (New_Result != 0)
                {
                    break;
                }
                Ch_Num++;
            }while(Ch_Num <= ScanGroupEP);
            ConvRes->PhyChannel                       = (reg_val >> 16) & 0x3f;
            r_adc_VChannel[Unit][ConvRes->ScanGroup][ConvRes->PhyChannel] = Ch_Num;
            ConvRes->Channel[ConvRes->PhyChannel]     = Ch_Num;
            r_adc_CovResult[Unit][ConvRes->ScanGroup][ConvRes->PhyChannel]  = New_Result;
            if (Unit == 0)
            {
                MPX_E                                 = (reg_val >> 31) & 0x1;
                if (MPX_E)
                {
                    r_adc_MpxNum[ConvRes->ScanGroup][ConvRes->PhyChannel] = (reg_val >> 28) & 0x7;
                }
                else
                {
                    r_adc_MpxNum[ConvRes->ScanGroup][ConvRes->PhyChannel] = 0x8;
                }
            }
        }
        else
        {
            do
            {
                addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_DIR(Ch_Num);
                reg_val      = R_ADC_READ_REG(32, addr);
                ConvRes->PhyChannel                       = (reg_val >> 16) & 0x3f;
                r_adc_VChannel[Unit][ConvRes->ScanGroup][ConvRes->PhyChannel] = Ch_Num;
                ConvRes->Channel[ConvRes->PhyChannel]     = Ch_Num;
                New_Result   = reg_val & 0xffff;
                r_adc_StrFlag[Unit] = R_ADC_NO_STR;
                if (New_Result != 0)
                {
                    r_adc_CovResult[Unit][ConvRes->ScanGroup][ConvRes->PhyChannel]  = New_Result;
                }
                if (Unit == 0)
                {
                    MPX_E                                 = (reg_val >> 31) & 0x1;
                    if (MPX_E)
                    {
                        r_adc_MpxNum[ConvRes->ScanGroup][ConvRes->PhyChannel]       = (reg_val >> 28) & 0x7;
                    }
                    else
                    {
                        r_adc_MpxNum[ConvRes->ScanGroup][ConvRes->PhyChannel]       = 0x8;
                    }
                }
                 Ch_Num++;
            }while(Ch_Num <= ScanGroupEP);
        }
    }    
}


/*******************************************************************************
  Function: R_ADC_ReadPWM
  
  see: <r_adc_api.h> for details
*/
uint16_t R_ADC_ReadPWM (uint32_t Unit)
{
    uint32_t         addr    = 0;
    uint32_t         reg_val = 0;
    uint16_t         PWM_Result;

    if (Unit < R_ADC_MACRO_NUM)
    {
        addr         = r_adc_Unit[Unit].BaseAddr + R_ADC_PWDDIR;
        reg_val      = R_ADC_READ_REG(32, addr);
        PWM_Result   = reg_val & 0xffff;
    }
    return PWM_Result;
}


/***********************************************************
  Function: R_ADC_ReadError
  
  see: <r_adc_api.h> for details
*/
void R_ADC_ReadError (uint32_t Unit, r_adc_ResParams_t* ConvRes)
{
    uint32_t          addr    = 0;
    uint32_t          reg_val = 0;
    uint8_t           Ch_Num  = 0;
    
    addr                      = r_adc_Unit[Unit].BaseAddr + R_ADC_ULER;
    reg_val                   = R_ADC_READ_REG(32, addr);
    ConvRes->UlStatus         = (reg_val >> 14) & 0x3;
    if (ConvRes->UlStatus)  
    {
        ConvRes->ErrChannel   = reg_val & 0x3f;
        addr                  = r_adc_Unit[Unit].BaseAddr + R_ADC_ECR;
        R_ADC_WRITE_REG(32, addr, 0x8);
    }
    
    addr                      = r_adc_Unit[Unit].BaseAddr + R_ADC_OWER;
    reg_val                   = R_ADC_READ_REG(32, addr);
    ConvRes->OwStatus         = (reg_val >> 7) & 0x1;
    if (ConvRes->OwStatus)
    {
        ConvRes->OwChannel   = reg_val & 0x3f;
        do
        {
            if (ConvRes->Channel[Ch_Num] == ConvRes->OwChannel)
            {
                ConvRes->OwChannel       = Ch_Num;
                break;
            }
            Ch_Num ++;
        }while(Ch_Num < R_SYS_ADC_MaxChannelNum(Unit));
        addr                             = r_adc_Unit[Unit].BaseAddr + R_ADC_ECR;
        R_ADC_WRITE_REG(32, addr, 0x4);
    }
}


/*******************************************************************************
  Function: R_ADC_DbgSVSTOP
  
  see: <r_adc_api.h> for details
*/
void R_ADC_DbgSVSTOP (uint32_t Unit, r_adc_Emu_t ADC_Emu)
{
    uint32_t          addr    = 0;    
    if (Unit < R_ADC_MACRO_NUM)
    {
        addr = r_adc_Unit[Unit].BaseAddr + R_ADC_EMUCR;
        switch (ADC_Emu)
        {
            case R_ADC_SVSTOP_EFECTIVE: R_ADC_WRITE_REG(8, addr, 0x0);
                                        R_SYS_ADC_DbgSVSTOP (ADC_Emu);
                                        break;
            case R_ADC_SVSTOP_IGNORED : R_ADC_WRITE_REG(8, addr, 0x1);
                                        break;
            default                   : R_ADC_ERROR(Unit, R_ADC_ERR_RANGE, "ADC: Invalid Configuration for SVSTOP");
        }
    }
}


/*******************************************************************************
  Function: R_ADC_MaxChannelNum
  
  see: <r_adc_api.h> for details
*/

uint8_t R_ADC_MaxChannelNum(uint32_t Unit)
{
    return   R_SYS_ADC_MaxChannelNum(Unit);
}


/*******************************************************************************
  Function: R_ADC_SetErrorCallback
*/

void R_UARTE_SetErrorCallback(void (*ErrorCallback)(uint32_t Unit, r_adc_Error_t Error))
{
    loc_ErrorHandler = ErrorCallback; 
}


/*******************************************************************************
  Section: Global Functions
*/

/*******************************************************************************
  Function: R_ADC_ErrorHandler
  
  see: <r_adc_main.h> for details
*/

void R_ADC_ErrorHandler(uint32_t Unit, r_adc_Error_t Error) 
{
    if (loc_ErrorHandler != 0) 
    {      /* we've got a registered error handler here ...*/
        loc_ErrorHandler(Unit, Error);      /* ... and we gonna use it!*/
    } 
    else 
    {
        switch (Error)
        {
        case R_ADC_ERR_OK              :
            break; 
        case R_ADC_ERR_NG              : 
            break;                                         
        case R_ADC_ERR_RANGE           : 
            R_DISPLAY_ERROR("ADCA: Range error occours");
            break;                                         
        case R_ADC_UNINIT              : 
            R_DISPLAY_ERROR("ADCA: API service called without module initialization");
        case R_ADC_BUSY                : 
            R_DISPLAY_ERROR("ADCA: Start conversion called when the conversion is already running");
            break;                                         
        case R_ADC_IDLE                : 
            R_DISPLAY_ERROR("ADCA: Stop called while no conversion was running");
            break;                                         
        case R_ADC_ALREADY_INITIALIZED : 
            R_DISPLAY_ERROR("ADCA: Init called while ADC is already initialized");
            break;                                         
        case R_ADC_PARAM_CONFIG        : 
            R_DISPLAY_ERROR("ADCA: Init called with incorrect configuration");
            break;                                         
        case R_ADC_PARAM_POINTER       : 
            R_DISPLAY_ERROR("ADCA: Wrong or empry parameter pointer");              
            break;                                         
        case R_ADC_PARAM_GROUP         : 
            R_DISPLAY_ERROR("ADCA: Wrong group configuration");
            break;                                         
        case R_ADC_WRONG_TRIGG_SRC     : 
            R_DISPLAY_ERROR("ADCA: Trigger service function does not match configuration");                                                                  
            break;                                         
        case R_ADC_NOTIF_CAPABILITY    : 
            R_DISPLAY_ERROR("ADCA: No ISR assigned for the current group");              
            break;                                         
        case R_ADC_BUFFER_UNINIT       : 
            R_DISPLAY_ERROR("ADCA: Uninitialised result buffer");
        default                        :
            R_DISPLAY_ERROR("ADCA: Unknown Error");
           break;                                         
        }

        for (;;) 
        {                          /* we don't have a handler */
        }                          /* so, let's wait here */
    }
}



/*******************************************************************************
  Function: R_ADC_ScanGroupProirtyOrder
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_ScanGroupProirtyOrder(uint32_t Unit, uint32_t Order)
{
    uint32_t      addr    = 0;
    uint32_t      val     = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
           addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_SGPRCR;
           if(0x43210u == Order)
           {
                val = Order;
           }
           else if (0x32401u == Order)
           {
                val = Order;
           }  
           else
           {
               /* error */
               ret_val = R_ADC_PARAM_CONFIG;
           } 

           R_ADC_WRITE_REG(32, addr, val);                                      
           ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/* support for the temperature sensor */


/*******************************************************************************
  Function: R_ADC_StartGroupConversionTsn
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_StartGroupConversionTsn(uint32_t Unit)
{
    uint32_t      addr    = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
           addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_TSNSGCR;
           R_ADC_WRITE_REG(8, addr, 0x1);                                      
           ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
  Function: R_ADC_GetTemperatureTsn
  
  see: <r_adc_api.h> for details
*/
uint16_t R_ADC_GetTemperatureTsn(uint32_t Unit)
{
    uint32_t      addr    = 0;
    uint32_t      ret_val = 0;

    if (Unit < R_ADC_MACRO_NUM)
    {
           addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_PWDTSNDR;
           ret_val = R_ADC_READ_REG(32, addr);                                      
    }
    else
    {
        /* error */
    }

    /* only the lower 16 bits required */
    return (uint16_t) (ret_val & 0x0000FFFFu);
}


/*******************************************************************************
  Function: R_ADC_ConfigLimitsTsn
  
  see: <r_adc_api.h> for details
*/
r_adc_Error_t R_ADC_ConfigLimitsTsn(uint32_t Unit, r_adc_ULLTableCfg_t Config)
{
    uint32_t      addr    = 0;
    uint32_t      val     = 0;
    r_adc_Error_t ret_val = R_ADC_ERR_NG;
    if (Unit < R_ADC_MACRO_NUM)
    {
           addr    = r_adc_Unit[Unit].BaseAddr + R_ADC_TSNVCR;
           if(R_ADC_SEL_ULLMTBR0 == Config)
           {
                val = 0;
           }
           else if (R_ADC_SEL_ULLMTBR0 == Config)
           {
                val = (uint8_t) 0x01u  << 6;
           }
           else if (R_ADC_SEL_ULLMTBR1 == Config)
           {
                val = (uint8_t) 0x02u << 6;
           }
           else if (R_ADC_SEL_ULLMTBR2 == Config)
           {
                val = (uint8_t) 0x03u << 6;
           }           
           else
           {
               /* error */
               ret_val = R_ADC_PARAM_CONFIG;
           } 

           R_ADC_WRITE_REG(32, addr, val | 0x25u);                                      
           ret_val = R_ADC_ERR_OK;
    }
    else
    {
        ret_val = R_ADC_ERR_RANGE;
    }
    return ret_val;
}


/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
