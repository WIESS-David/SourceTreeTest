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

#ifndef  ADC_API_H
#define  ADC_API_H



#ifdef __cplusplus
extern "C" {
#endif

/* This is a temporary solution for some golbal variables used by many modules 
   The goal shuld be to provide read/write functions instead */
#undef R_EXTERN
#ifndef  ADCA_MAIN_GLOBALS_
#define R_EXTERN extern 
#else
#define R_EXTERN
#endif


#include "r_config_adc.h"

/*******************************************************************************
  Title: ADC7010 API Functions
*/

/*******************************************************************************
  Section: Global constants
*/
/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Section: Global Types
*/

/*******************************************************************************
  Enum: r_adc_Error_t

  ADCA macro driver error code.

  Values:
   R_ADC_ERR_OK              - No error
   R_ADC_ERR_NG              - Unspecified error   
   R_ADC_ERR_RANGE           - (Unit) Range error
   R_ADC_UNINIT              - API service called without module initialization
   R_ADC_BUSY                - start conversion called when the conversion is already running
   R_ADC_IDLE                - stop called while no conversion was running
   R_ADC_ALREADY_INITIALIZED - Init called while ADC is already initialized
   R_ADC_PARAM_CONFIG        - Init called with incorrect configuration
   R_ADC_PARAM_POINTER       - wrong or empry parameter pointer
   R_ADC_PARAM_GROUP         - wrong group
   R_ADC_WRONG_TRIGG_SRC     - trigger service function does not match configuration
   R_ADC_NOTIF_CAPABILITY    - no ISR assigned for the current group
   R_ADC_BUFFER_UNINIT       - uninitialised result buffer 
   R_ADC_ERR_LAST            - Delimiter 
*/

typedef enum 
{
    R_ADC_ERR_OK                    = 0x00,
    R_ADC_ERR_NG                    = 0x01,
    R_ADC_ERR_RANGE                 = 0x02,
    R_ADC_UNINIT                    = 0x0A,
    R_ADC_BUSY                      = 0x0B,
    R_ADC_IDLE                      = 0x0C,
    R_ADC_ALREADY_INITIALIZED       = 0x0D,
    R_ADC_PARAM_CONFIG              = 0x0E,
    R_ADC_PARAM_POINTER             = 0x14,
    R_ADC_PARAM_GROUP               = 0x15,
    R_ADC_WRONG_TRIGG_SRC           = 0x17,
    R_ADC_NOTIF_CAPABILITY          = 0x18,
    R_ADC_BUFFER_UNINIT             = 0x19,
    R_ADC_INVALID_DATABASE          = 0xEF,
    R_ADC_ERR_LAST
} r_adc_Error_t;


/*******************************************************************************
  Type: r_adc_InitFlag_t

  Device Initialization Flag

  Members:
  R_ADC_NO_INIT      - No initialization for ADC
  R_ADC_INIT         - ADC is initialized

*/

typedef enum
{
    R_ADC_NO_INIT = 0,
    R_ADC_INIT
} r_adc_InitFlag_t;


/*******************************************************************************
  Type: r_adc_StrFlag_t

  Device Start Flag

  Members:
  R_ADC_NO_STR      - ADC not started
  R_ADC_STR         - ADC is started

*/

typedef enum
{
    R_ADC_NO_STR = 0,
    R_ADC_STR
} r_adc_StrFlag_t;


/*******************************************************************************
  Enum: r_adc_Interrupt_t

  ADCA macro interrupt type. List of all available interrupts for this macro

  Values: 
    R_ADC_INT_TSN    - Temperature sensor Interrupt
    R_ADC_INT_CG0    - Channel group 0 interrupt
    R_ADC_INT_CG1    - Channel group 1 interrupt
    R_ADC_INT_CG2    - Channel group 2 interrupt
    R_ADC_INT_ERR    - Error Interrupt
    R_ADC_INT_NUM    - Delimiter

*/

typedef enum
{
    R_ADC_INT_TSN,
    R_ADC_INT_CG0,
    R_ADC_INT_CG1,
    R_ADC_INT_CG2,
    R_ADC_INT_ERR,
    R_ADC_INT_NUM
} r_adc_Interrupt_t;


/*******************************************************************************
  Enum: r_adc_Mode_t

  ADCA macro conversion mode.

  Values:
  R_ADC_ONE_SHOT_MODE          - Multi-Cycle mode
  R_ADC_CONTINUOUS_MODE        - Continuous Operation 
*/

typedef enum
{    
    R_ADC_MULTICYCLE_MODE = 0,
    R_ADC_CONTINUOUS_MODE
} r_adc_Mode_t;

/*******************************************************************************
  Enum: r_adc_SamplingTime_t

  ADCA macro sampling time setting . (Should be discussed, because of the UM and SPEC)
  only the values defined here are allowed 

  Values:
  R_ADC_SLOW_SAMPLING        - slow conversion mode
  R_ADC_FAST_SAMPLING        - fast conversion mode
*/

typedef enum
{    
    R_ADC_SLOW_SAMPLING = 0x18,
    R_ADC_FAST_SAMPLING = 0x12,
} r_adc_SamplingTime_t;


/*******************************************************************************
  Enum: r_adc_ErrInfo_t

  ADCA Error generation setting.

  Values:
  R_ADC_DONT_GENERATE_ERR     - Macro setup option A
  R_ADC_GENERATE_ERR          - Macro setup option B 
*/

typedef enum
{    
    R_ADC_DONT_GENERATE_ERR     = 0,
    R_ADC_GENERATE_ERR
} r_adc_ErrInfo_t;


/*******************************************************************************
  Enum: r_adc_Align_t

  ADCA result alignment flag.

  Values:
    R_ADC_RIGHT_ALIGNED   - Result values are right aligned, bit 0  - 9(11)
    R_ADC_LEFT_ALIGNED    - Result values are left aligned, bit 15 - 4 (6)
*/

typedef enum
{    
    R_ADC_RIGHT_ALIGNED = 0,
    R_ADC_LEFT_ALIGNED
} r_adc_Align_t;

/*******************************************************************************
  Enum: r_adc_Trigger_t

  ADCA Trigger flag.

  Values:
    R_ADC_SW_TRIGGER   - Software trigger
    R_ADC_HW_TRIGGER   - Hardware trigger
*/

typedef enum
{    
    R_ADC_SW_TRIGGER    = 0,
    R_ADC_HW_TRIGGER
} r_adc_Trigger_t;

/*******************************************************************************
  Enum: r_adc_ResultTreat_t

  Result treatment.

  Values:
    R_ADC_RESULT_RETAIN   - Result retain after reading
    R_ADC_RESULT_CLEARED  - Result cleares after reading
*/

typedef enum
{    
    R_ADC_RESULT_RETAIN  = 0,
    R_ADC_RESULT_CLEARED
} r_adc_ResultTreat_t;

/*******************************************************************************
  Enum: r_adc_Resolution_t

  ADCA bit resolution modes.

  Values:
      R_ADC_12BIT_RES     - ADC 12 Bit resolution
      R_ADC_10BIT_RES     - ADC 10 Bit resolution
*/

typedef enum
{  
      R_ADC_12BIT_RES = 0,
      R_ADC_10BIT_RES        
} r_adc_Resolution_t;

/*******************************************************************************
  Enum: r_adc_ReadStatus_t

  ADCA Read status flag.

  Values:
    R_ADC_READ_BEFORE     - Status if value was read in LCR register before
    R_ADC_NEW_VALUE       - Status if value is new
*/

typedef enum
{    
    R_ADC_READ_BEFORE = 0,
    R_ADC_NEW_VALUE
} r_adc_ReadStatus_t;

/*******************************************************************************
  Enum: r_adc_OverwriteStatus_t

  ADCA overwrite status.

  Values:
    R_ADC_NO_OVERWR       - No value overwritten
    R_ADC_OVERWRITTEN     - Overwritten value status
*/

typedef enum
{    
    R_ADC_NO_OVERWR = 0,
    R_ADC_OVERWRITTEN
} r_adc_OverwriteStatus_t;


/*******************************************************************************
  Enum: r_adc_Multiplexer_t

  ADCA multiplexer settings.

  Values:
    R_ADC_NO_MPX       - No value overwritten
    R_ADC_MPX          - Overwritten value status
*/

typedef enum
{    
    R_ADC_NO_MPX = 0,
    R_ADC_MPX
} r_adc_Multiplexer_t;

/*******************************************************************************
  Enum: r_adc_Repeat_t

  ADCA channel conversion repeat settings

  Values:
    R_ADC_CONVERT_ONCE     - conversion performed one time
    R_ADC_CONVERT_TWICE    - conversion performed two times
    R_ADC_CONVERT_4TIMES   - conversion performed four times
*/

typedef enum
{    
    R_ADC_CONVERT_ONCE      = 0x0,
    R_ADC_CONVERT_TWICE     = 0x1, 
    R_ADC_CONVERT_4TIMES    = 0x2
} r_adc_Repeat_t;

/*******************************************************************************
  Enum: r_adc_GroupRepeat_t

  ADCA group conversion repeat settings

  Values:
    R_ADC_SGCONV_ONCE      - conversion performed one time
    R_ADC_SGCONV_TWICE     - conversion performed two times
    R_ADC_SGCONV_4TIMES    - conversion performed four times
*/

typedef enum
{    
    R_ADC_SGCONV_ONCE      = 0x0,
    R_ADC_SGCONV_TWICE     = 0x1, 
    R_ADC_SGCONV_4TIMES    = 0x3
} r_adc_GroupRepeat_t;

/*******************************************************************************
  Enum: r_adc_Limit_t

  ADCA Upper Lower Limit.

  Values:
    R_ADC_UPPER_LIMIT     - upper limit 
    R_ADC_LOWER_LIMIT     - lower limit
*/

typedef struct
{    
    uint32_t   Upper_Limit;
    uint32_t   Lower_Limit;
} r_adc_Limit_t;

/*******************************************************************************
  Enum: r_adc_Emu_t

  Disable the SVSTOP signal 

  Values:
    R_ADC_SVSTOP_EFECTIVE - Conversion halts with debugger break
    R_ADC_SVSTOP_IGNORED  - conversion continues 
*/   
    
typedef enum
{  
    R_ADC_SVSTOP_EFECTIVE = 0,
    R_ADC_SVSTOP_IGNORED
} r_adc_Emu_t;

/*******************************************************************************
  Enum: r_adc_ConversionEnd_t

  En/Disable scan end interrupt of a virtual channel or a scan group

  Values:
    R_ADC_COVEND_EFECTIVE - Enable scan end interrupt of a virtual channel or a scan group
    R_ADC_COVEND_IGNORED  - Disable scan end interrupt of a virtual channel or a scan group
*/   
    
typedef enum
{  
    R_ADC_COVEND_IGNORED  = 0,
    R_ADC_COVEND_EFECTIVE
} r_adc_ConversionEnd_t;

/*******************************************************************************
  Type: r_adc_GroupCfg_t
  
  ADCA scan group configuration.
  
  Members:  
    Mode        - Multi-Cycle or continuous mode, see: <r_adc_Mode_t>
    SGNum       - Scan group repeat number, see <r_adc_GroupRepeat_t>
    Trigger     - SW or HW/SW trigger, see <r_adc_Trigger_t>
    Int_SGEnd   - Scan group end interrupt, see <r_adc_ConversionEnd_t>
    VcStart     - first virtual channel of the group 
    VcEnd;      - last virtual channel of the group
    ConvNum     - Nuber of conversions per channel, see <r_adc_Repeat_t>
*/  
typedef struct 
{
    r_adc_Mode_t           Mode;
    r_adc_GroupRepeat_t    SGNum;
    r_adc_Trigger_t        Trigger;
    r_adc_ConversionEnd_t  Int_SGEnd;
    uint8_t                VcStart; 
    uint8_t                VcEnd;
    r_adc_Repeat_t         ConvNum;
} r_adc_GroupCfg_t;

/*******************************************************************************
  enum: r_adc_SuspendMode_t
  
   Select the suspend method used when a higher-priority SG
   interrupts a lower-priority SG.
*/  
typedef enum 
{
    R_ADC_SYNC_SUSPEND = 0,
    R_ADC_ASYNC_SUSPEND = 2,
    
} r_adc_SuspendMode_t;


/*******************************************************************************
  Type: r_adc_ULLimitCfg_t
  
  ADCA upper lower limit.
  
  Values:
    R_ADC_NO_ULL      - upper lower limit not enabled
    R_ADC_ULL_0       - upper lower limit 0
    R_ADC_ULL_1       - upper lower limit 1
    R_ADC_ULL_2       - upper lower limit 2
    R_ADC_ULL_LAST    - the number of ULL selections
*/  
typedef enum 
{
    R_ADC_NO_ULL = 0, 
    R_ADC_ULL_0,
    R_ADC_ULL_1, 
    R_ADC_ULL_2,
    R_ADC_ULL_LAST
} r_adc_ULLimitCfg_t;

/*******************************************************************************
  Type: r_adc_ULLTableCfg_t
  
  ADCA upper lower limit.
  
  Values:
    R_ADC_SEL_ULLMTBR_NONE- 00: Upper limit and lower limit are not checked.              
    R_ADC_SEL_ULLMTBR0    - 01: Upper limit and lower limit are checked for ADCEnULLMTBR0.
    R_ADC_SEL_ULLMTBR1    - 10: Upper limit and lower limit are checked for ADCEnULLMTBR1.
    R_ADC_SEL_ULLMTBR2    - 11: Upper limit and lower limit are checked for ADCEnULLMTBR2.
    R_ADC_SEL_ULL_LAST    - the number of ULL selections
*/  
typedef enum 
{
    R_ADC_SEL_ULLMTBR_NONE = 0, 
    R_ADC_SEL_ULLMTBR0,
    R_ADC_SEL_ULLMTBR1, 
    R_ADC_SEL_ULLMTBR2,
    R_ADC_SEL_ULLMTBR_LAST
} r_adc_ULLTableCfg_t;


/*******************************************************************************
  Enum: r_adc_UllErrStatus_t

  ADCA Upper Lower Error status.

  Values:
    R_ADC_NO_OVERWR       - No Ull Error
    R_ADC_OVERWRITTEN     - Ull Error occurs
*/

typedef enum
{    
    R_ADC_NO_ULLERR = 0,
    R_ADC_ULLERR
} r_adc_UllErrStatus_t;


/*******************************************************************************
  Type: r_adc_Group_t
  
  ADCA conversion group.
  
  Values:
    R_ADC_CG_0     - Scan Group 0 (Conversion Group) Temperature sensor  
    R_ADC_CG_1     - Scan Group 1 (Conversion Group)   
    R_ADC_CG_2     - Scan Group 2 (Conversion Group)
    R_ADC_CG_3     - Scan Group 3 (Conversion Group)
    R_ADC_CG_PWM   - Scan Group for PWM-Diagnose
    R_ADC_CG_LAST  - Maximum number of Scan Groups
*/  
typedef enum 
{
    R_ADC_CG_0, 
    R_ADC_CG_1, 
    R_ADC_CG_2,
    R_ADC_CG_3, 
    R_ADC_CG_PWM, 
    R_ADC_CG_LAST
    
} r_adc_Group_t;

/*******************************************************************************
  Type: r_adc_Pulldown_t

  ADCA pull down enable for physical channels 
   
  Values:
    R_ADC_PULLDN_OFF   - Disable pull down circuit
    R_ADC_PULLDN_ON    - Enable pull down circuit
*/   
    
typedef enum
{  
    R_ADC_PULLDN_OFF   = 0,
    R_ADC_PULLDN_ON
} r_adc_Pulldown_t;

/*******************************************************************************
  Type: r_adc_SDcovMode_t

  ADCA self diagnose conversion modes,only for virtual channel 33-35 available
   
  Values:
    R_ADC_SD_HOLD   - Hold value is perfomed
    R_ADC_SD_COV    - Normal conversion is perfomed
*/   
    
typedef enum
{  
    R_ADC_SD_HOLD   = 0,
    R_ADC_SD_COV
} r_adc_SDcovMode_t;

/*******************************************************************************
  Type: r_adc_SDInput_t

  ADCA self diagnose Input selection
   
  Values:
    R_ADC_SD_ANIN   - Analog inputs, physical channels
    R_ADC_SD_DIAG   - DIAG inputs, see UM
*/   
    
typedef enum
{  
    R_ADC_SD_ANIN   = 0,
    R_ADC_SD_DIAG
} r_adc_SDInput_t;

/*******************************************************************************
  Type: r_adc_SDVolTrg_t

  ADCA self diagnose voltage mode selection
   
  Values:
    R_ADC_SD_OFF    - Disable Self Diagnose voltage circuit
    R_ADC_SD_ON     - Enable Self Diagnose voltage circuit
                      The voltage is selected by the ADCAnDGCTL0 Register
*/   
    
typedef enum
{  
    R_ADC_SD_OFF   = 0,
    R_ADC_SD_ON
} r_adc_SDVolTrg_t;

/*******************************************************************************
  Type: r_adc_SDparams_t

  ADCA self diagnose configuration
   
  Members:
    SDmode      - Self diagnose conversion modes, see <r_adc_SDcovMode_t>
    SDinput     - Self diagnose input selection, see <r_adc_SDInput_t> 
    volmode     - Self diagnose voltage mode, see UM
*/   
    
typedef struct
{  
    r_adc_SDcovMode_t  SDmode;
    r_adc_SDInput_t    SDinput;
    uint8_t            volmode;
} r_adc_SDparams_t;

/*******************************************************************************
  Type: r_adc_ChannelParams_t

  Inerpreted ADCA conversion result parameters 
  
  Members:
    MPXEnable      - check if a multiplexer is used, see <r_adc_Multiplexer_t>
    MPXAddr        - the Address of the Multiplexer 
                     (the external multiplexer is only for ADC0 available)     
    INT_VCEnd      - en/disable the conversion end Interrupt of virtual channels, see <r_adc_ConversionEnd_t>
    ULLCheck       - select the Upper-Lower Limit Channels, see <r_adc_ULLimitCfg_t>   
    phyChannel     - select the Physical Channel
    phyChPD        - pull down cuircuit enable, see <r_adc_Pulldown_t>
    SDparams       - self diagnose function parameters, see <r_adc_SDparams_t>
*/     

typedef struct
{
    r_adc_Multiplexer_t   MPXEnable;
    uint16_t              MPXAddr;   
    r_adc_ConversionEnd_t INT_VCEnd;   
    r_adc_ULLimitCfg_t    ULLCheck;  
    uint16_t              phyChannel;
    r_adc_Pulldown_t      phyChPD;
    r_adc_SDparams_t      SDparams;
} r_adc_ChannelParams_t; 

/*******************************************************************************
  Type: r_adc_Config_t

  ADCA macro configuration data
    
  Members:
    Res         - Resolution 10 / 12 Bit, see <r_adc_Resolution_t>
    ResTreat    - Result retain or cleared after reading, see <r_adc_ResultTreat_t>
    Align       - Result alignment in register left or right, see <r_adc_Align_t>
    SuspendMode - Suspend Mode, see <r_adc_SuspendMode_t>
    SDVol_En    - en/disable voltage mode for self diagnose function, see <r_adc_SDVolTrg_t>
    SmpTime     - sampling time
    ULL[]       - upper lower limit setting, see <r_adc_Limit_t>
    ULErrInfo   - Upper-lower error generation flag, see <r_adc_ErrInfo_t>
    ULErrInfo   - Overwrite error generation flag, see <r_adc_ErrInfo_t>
    Group[]     - Channel group specific configuration see: <r_adc_GroupCfg_t>
    VCh[]       - virtual channel parameters, see <r_adc_ChannelParams_t>
*/     
       
typedef struct
{
    r_adc_Resolution_t     Res;
    r_adc_ResultTreat_t    ResTreat;
    r_adc_Align_t          Align;
    r_adc_SuspendMode_t    SuspendMode;
    r_adc_SDVolTrg_t       SDVol_En;
    uint16_t               SmpTime;
    r_adc_Limit_t          ULL[R_ADC_ULL_LAST];
    r_adc_ErrInfo_t        ULErrInfo;
    r_adc_ErrInfo_t        OWErrInfo;
    r_adc_GroupCfg_t       Group[R_ADC_CG_LAST];
    r_adc_ChannelParams_t  VCh[R_ADC_CHANNEL_MAX_NUM];
} r_adc_Config_t;

/*******************************************************************************
  Type: r_adc_ResParams_t

  Inerpreted ADCA conversion result parameters 
  
  Members: 
    ScanGroup       - current scan group, see <r_adc_Group_t>
    Channel[]       - related virtual channel
    PhyChannel      - Physical channel number     
    Align           - Alignment, see <r_adc_Align_t>    
    BitNo           - Resolution, see <r_adc_Resolution_t>
    OwStatus        - Overwrite status of channel result
    UlStatus        - Upper lower error status of channel result
    ErrChannel      - The Physical channel in which the upper lower error occurs
    OwChannel       - The Physical channel in which the overwrite error occurs
*/     
   
typedef struct
{
    r_adc_Group_t            ScanGroup;
    uint8_t                  Channel[R_ADC_CHANNEL_MAX_NUM];
    uint8_t                  PhyChannel;
    r_adc_Align_t            Align;
    r_adc_Resolution_t       BitNo;
    uint8_t                  OwStatus;
    uint8_t                  UlStatus;
    uint8_t                  ErrChannel;
    uint8_t                  OwChannel;
} r_adc_ResParams_t;

/*******************************************************************************
  Type: r_adc_PwmTrigger_t

  ADCA pwm diagnose channel init
   
  Values:
    R_ADC_PWM_OFF    - Disable Pwm Diagnose function
    R_ADC_PWM_ON     - Enable Pwm Diagnose function
*/   
    
typedef enum
{  
    R_ADC_PWM_OFF = 0x0,
    R_ADC_PWM_ON  = 0x1
} r_adc_PwmTrigger_t;

/*******************************************************************************
  Type: r_adc_THGroup_t

  ADCA T&H Groups, only for ADCA0 available 
  
  Values: 
    R_ADC_THG_A    - T&H Group A    
    R_ADC_THG_B    - T&H Group B 
*/

typedef enum
{
    R_ADC_THG_A = 0,
    R_ADC_THG_B,
    R_ADC_THG_LAST
} r_adc_THGroup_t;

/*******************************************************************************
  Type: r_adc_HldEndTrg_t

  En/Disable the Hold Completion Trigger of the ADCA T&H Circuits,
  only for ADCA0 available 
  
  Values: 
    R_ADC_HENDTRG_HLDCTRG   - the hold end trigger is selected for the (next) scan group start
    R_ADC_HENDTRG_SGTRG     - the scan group trigger is selected for the (next) scan group start
*/

typedef enum
{  
    R_ADC_HENDTRG_HLDCTRG  = 0,
    R_ADC_HENDTRG_SGTRG
} r_adc_HldEndTrg_t;

/*******************************************************************************
  Type: r_adc_HldTrg_t

  En/Disable the Hold Trigger of the ADCA T&H Circuits,
  only for ADCA0 available 
  
  Values: 
    R_ADC_HTRG_SGTRG        - the scan group trigger is selected for the hold operation
    R_ADC_HTRG_HLDST        - the hold trigger HLDST is selected
*/

typedef enum
{  
    R_ADC_HTRG_SGTRG  = 0,
    R_ADC_HTRG_HLDST
} r_adc_HldTrg_t;

/*******************************************************************************
  Type: r_adc_THEnable_t

  En/Disable the ADCA T&H Circuits,
  or the transition of sampling after the hold is performed on the ADCA T&H Circuits,    
  only for ADCA0 available 
   
  Values:
    R_ADC_TH_DISABLE        - T&H Circuit is disabled (see: r_adc_THCh_t) or
                              Automatic Sampling is not performed (see: r_adc_THCfg_t)
    R_ADC_TH_ENABLE         - T&H Circuit is enabled (see: r_adc_THCh_t) or
                              Automatic Sampling is performed (see: r_adc_THCfg_t)
*/

typedef enum
{  
    R_ADC_TH_DISABLE  = 0,
    R_ADC_TH_ENABLE
} r_adc_THEnable_t;

/*******************************************************************************
  Type: r_adc_TH_t

  ADCA T&H Circuits, only for ADCA0 available 
  
  Values: 
    R_ADC_TH_0     - T&H Circuit 0
    R_ADC_TH_1     - T&H Circuit 1
    R_ADC_TH_2     - T&H Circuit 2
    R_ADC_TH_3     - T&H Circuit 3
    R_ADC_TH_4     - T&H Circuit 4
    R_ADC_TH_5     - T&H Circuit 5
*/     

typedef enum
{
    R_ADC_TH_0 = 0,
    R_ADC_TH_1,
    R_ADC_TH_2,
    R_ADC_TH_3,
    R_ADC_TH_4,
    R_ADC_TH_5
} r_adc_TH_t;

/*******************************************************************************
  Type: r_adc_THChannel_t

  ADCA T&H channel parameters, only for ADCA0 available 
  
  Members: 
    TH_Ch       - T&H Channls, see <r_adc_TH_t>
    TH_E        - T&H Circuit en/disable, see <r_adc_THEnable_t>
*/

typedef struct
{
    r_adc_TH_t        TH_Ch;
    r_adc_THEnable_t  TH_E;
} r_adc_THCh_t;

/*******************************************************************************
  Type: r_adc_THCfg_t

  ADCA T&H configuration 
  
  Members: 
    TH[]           - T&H channel parameters, see <r_adc_THCh_t>
    TH_SmpMsk      - T&H Sampling mode, if automatic sampling is performed, see <r_adc_THEnable_t>
    TH_EndTrg      - Hold Completion Trigger, see <r_adc_HldEndTrg_t> 
    TH_HldTrg      - Hold Trigger, see <r_adc_HldTrg_t>     
    TH_SgGroup     - T&H scan group, see <r_adc_Group_t>
*/     

typedef struct
{
    r_adc_THCh_t       TH[R_ADC_THG_LAST];   
    r_adc_THEnable_t   TH_SmpMsk;   
    r_adc_HldEndTrg_t  TH_EndTrg;  
    r_adc_HldTrg_t     TH_HldTrg;
    r_adc_Group_t      TH_SgGroup;
} r_adc_THCfg_t;

/*******************************************************************************
  Section: Global Variables
*/
/*******************************************************************************
  Variable: r_adc_InitFlag

  ADCA initialization Flag.
*/
R_EXTERN r_adc_InitFlag_t r_adc_InitFlag[R_ADC_MACRO_NUM];

/*******************************************************************************
  Variable: r_adc_StrFlag

  ADCA Start Trigger Flag.
*/
R_EXTERN r_adc_StrFlag_t r_adc_StrFlag[R_ADC_MACRO_NUM];

/*******************************************************************************
  Variable: r_adc_OWInitFlag

  ADCA Overwrite initialization Flag.
*/
R_EXTERN r_adc_ErrInfo_t r_adc_OWInitFlag[R_ADC_MACRO_NUM];

/*******************************************************************************
  Variable: r_adc_ChEndFlag

  ADCA Channel Conversion End initialization Flag, used for different load method for conversion results
*/
R_EXTERN r_adc_ConversionEnd_t r_adc_ChEndFlag[R_ADC_MACRO_NUM];

/*******************************************************************************
  Variable: r_adc_CovResult
  
  Conversion Result Array of ADCA
*/
R_EXTERN uint16_t    r_adc_CovResult[R_ADC_MACRO_NUM][R_ADC_CG_PWM][R_ADC_CHANNEL_MAX_NUM]; 

/*******************************************************************************
  Variable: r_adc_MpxNum

  Multiplexer Data Array of ADCA, only for ADCA0
*/
R_EXTERN uint8_t     r_adc_MpxNum[R_ADC_CG_PWM][R_ADC_CHANNEL_MAX_NUM];

/*******************************************************************************
  Variable: r_adc_VChannel

  Virtual Channel Array of ADCA
*/
R_EXTERN uint8_t     r_adc_VChannel[R_ADC_MACRO_NUM][R_ADC_CG_PWM][R_ADC_CHANNEL_MAX_NUM];

/*******************************************************************************
  Variable: r_adc_UllErr

  Upper lower error Array
*/
R_EXTERN uint8_t     r_adc_UllErr[R_ADC_MACRO_NUM][R_ADC_CHANNEL_MAX_NUM];

/*******************************************************************************
  Variable: r_adc_OwErr

  Overwrite error Array
*/
R_EXTERN uint8_t     r_adc_OwErr[R_ADC_MACRO_NUM][R_ADC_CHANNEL_MAX_NUM];


/*******************************************************************************
  Section: Global API Functions
*/

/*******************************************************************************
  Function: R_ADC_Init

  Driver init function.

  Parameters:
    Unit       - Instance number
    Config     - Pointer to the configuraton, see <r_adc_Config_t>
 
  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_Init(uint32_t Unit, r_adc_Config_t * Config);

/*******************************************************************************
  Function: R_ADC_ResetCfgParams
  
  Initialize the configure parameters
  
  Parameters:
    Config     - Configuration parameters
 
  Returns:
  void
*/
void R_ADC_ResetCfgParams (r_adc_Config_t * Config);

/*******************************************************************************
  Function: R_ADC_ResetSDCfgParams
  
  Initialize the self diagnose configure parameters
  
  Parameters:
    Config     - Configuration parameters
 
  Returns:
  void
*/
void R_ADC_ResetSDCfgParams (r_adc_Config_t * Config);

/*******************************************************************************
  Function: R_ADC_DeInit

  Driver deinit function.

  Parameters:
    Unit       - Instance number

  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_DeInit(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_SetErrorCallback

  Set a user function as error handler.
  
  When the driver detects any error, it will call this function.

  Parameters:
  ErrorCallback - Pointer to a user function
  
  Returns:
  void
*/

void R_ADC_SetErrorCallback(void (*ErrorCallback )(uint32_t Unit, r_adc_Error_t Error));

/*******************************************************************************
  Function: R_ADC_SDiagDeInit

  ADCA self diagnose function deinit.

  Parameters:
    Unit       - Instance number

  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_SDiagDeInit(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_Config

  Config the ADCA macro.

  Parameters:
    Unit       - Instance number
    Config     - Pointer to the configuraton, see <r_adc_Config_t>
 
  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_Config(uint32_t Unit, r_adc_Config_t *Config);

/*******************************************************************************
  Function: R_ACD_ConfigVC

  Configure a virtual channel 
    
  Parameters:
    Unit       - ADC Instance number
    Channel    - Channel number 
    ChCfg      - Pointer to the channel configuraton, see <r_adc_ChannelParams_t>
 
  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ACD_ConfigVC (uint32_t Unit, 
                              uint8_t  Channel, 
                              r_adc_Config_t * ChCfg); 


/*******************************************************************************
  Function: R_ADC_AssignULL

  Assign Upper-Lower Limit for ADC
    
  Parameters:
    Unit       - ADC Instance number
    Ull_Num    - Upper lower limit configuration 
    Config     - Pointer to the configuraton, see <r_adc_Config_t>
 
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_AssignULL(uint32_t Unit, r_adc_ULLimitCfg_t Ull_Num, r_adc_Config_t *Config);

/*******************************************************************************
  Function: R_ADC_ConfigSG

  Set operations of Scan Groups
  
  Parameters:
    Unit       - ADC Instance number
    ScanGroup  - Selected scan group 
    Config     - Pointer to the configuraton, see <r_adc_Config_t>
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_ConfigSG(uint32_t Unit, r_adc_Group_t ScanGroup, r_adc_Config_t *Config); 

/*******************************************************************************
  Function: R_ADC_StartGroupConversion

  Start ADCA process .

  Parameters:
    Unit        - Instance number
    ChGr        - Channel Group, see: <r_adc_Group_t>      

  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_StartGroupConversion(uint32_t Unit, r_adc_Group_t ChGr);


/*******************************************************************************
  Function: R_ADC_Halt

  Halt all ADCA conversions

  Parameters:
    Unit        - Instance number

  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_Halt(uint32_t Unit);


/*******************************************************************************
  Function: R_ADC_EnableHwTrigger

  Enable (activate) the selected HW trigger for the given CG

  Parameters:
  Unit      - Instance number
  ChGr      - Channel Group, see: <r_adc_Group_t>

  Returns:
  void
*/

void R_ADC_EnableHwTrigger(uint32_t Unit, r_adc_Group_t ChGr, r_adc_HWTrigger_t HwTrg);

/*******************************************************************************
  Function: R_ADC_DisableHwTrigger

  Disable (de-activate) the selected HW trigger for the given CG

  Parameters:
  Unit      - Instance number
  ChGr      - Channel Group, see: <r_adc_Group_t>

  Returns:
  void
*/

void R_ADC_DisableHwTrigger(uint32_t Unit, r_adc_Group_t Group);

/*******************************************************************************
  Function: R_ADC_SDiagInit

  Self Diagnose Init function
  
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    SDCfg      - Pointer to config data, see <r_adc_Config_t>
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ACD_SDiagInit (uint32_t Unit, r_adc_Config_t * SDCfg);

/*******************************************************************************
  Function: R_ADC_EnablePwmTrg

   PWM Diagnose Start Trigger
  
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    PWM_En     - see <r_adc_PwmTrigger_t>
    
   Returns:
   void
*/

void R_ADC_EnablePwmTrg(uint32_t Unit, r_adc_PwmTrigger_t PWM_En);

/*******************************************************************************
  Function: R_ADC_ConfigTH

  Set operations of T&H Circuits
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    THGroup    - Selected T & H group 
    THCfg      - Pointer to config data, see <r_adc_THCfg_t>
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_ConfigTH(uint32_t Unit, r_adc_THGroup_t THGroup, r_adc_THCfg_t *THCfg);

/*******************************************************************************
  Function: R_ADC_THDisable

  Disable all T&H Circuits
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_THDisable(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_StartTrgTH

  Start all T&H Circuits
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_StartTrgTH(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_THDisable

  Start hold operation of all T&H Circuits
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    THGroup    - T&H group, see <r_adc_THGroup_t>
    
   Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_HoldTrgTH(uint32_t Unit, r_adc_THGroup_t THGroup);

/*******************************************************************************
  Function: R_ADC_ReadRes

  Read ADC Conversion Result
  
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    ConvRes    - Pointer to load conversion result, see <r_adc_ResParams_t>
    
   Returns:
   void
*/

void R_ADC_ReadRes (uint32_t Unit, r_adc_ResParams_t* ConvRes);

/*******************************************************************************
  Function: R_ADC_ReadPWM

  Read Conversion Result of PWM Channel
  
  Parameters:
    Unit       - ADC Instance number
    
   Returns:
   conversion result
*/
uint16_t R_ADC_ReadPWM (uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_ReadData

  Read ADC Conversion Error Information
  Parameters:
    Unit       - ADC Instance number, only 0 is availible for T & H Circuit
    ConvRes    - Pointer to load conversion result, see <r_adc_ResParams_t>
    
   Returns:
   void
*/

void R_ADC_ReadError (uint32_t Unit, r_adc_ResParams_t* ConvRes);

/*******************************************************************************
  Function: R_ADC_DbgSVSTOP
  
  Enable the SVSTOP
  
  Parameters:
    Unit          - Instance number
    ADC_Emu       - En/Disable SVSTOP
  
   Returns:
   void
*/
void R_ADC_DbgSVSTOP (uint32_t Unit, r_adc_Emu_t ADC_Emu);

/*******************************************************************************
  Function: R_ADC_MaxChannelNum
  
  Get the Maximum ADC Channel Number for each Unit.
  
  Parameters:
    Unit          - Instance number
  
   Returns:
   uint8_t
*/

uint8_t R_ADC_MaxChannelNum(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_SetIsrCallback

  Assign a user callback for the given interrupt of a unit. 
  
  The function will be called after the driver ISR has
  processed the interrupt.

  Parameters:
  Unit          - Instance number
  IntType       - What kind of interrupt; see: <r_adc_Interrupt_t>
  Isr           - Isr function pointer
  
  Returns:
  see: <r_adc_Error_t>

*/

r_adc_Error_t R_ADC_SetIsrCallback(uint32_t Unit, r_adc_Interrupt_t IntType, void (*Isr)(void)); 

/*******************************************************************************
  Function: R_ADC_IsrTsn

  Temperature sensor interrupt service routine. 
  
  This function shall be called by the device interrupt handler 
  (xxxxxx_isr.c) and can be found in adc0710\src\r_adc_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_ADC_IsrTsn(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_Isr0

  Conversion group 0 interrupt service routine. 
  
  This function shall be called by the device interrupt handler 
  (xxxxxx_isr.c) and can be found in adc0710\src\r_adc_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_ADC_Isr0(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_Isr1

  Conversion group 1 interrupt service routine. 
  
  This function shall be called by the device interrupt handler 
  (xxxxxx_isr.c) and can be found in adc0710\src\r_adc_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_ADC_Isr1(uint32_t Unit);

/*******************************************************************************
  Function: R_ADC_Isr2

  Conversion group 2 interrupt service routine. 
  
  This function shall be called by the device interrupt handler 
  (xxxxxx_isr.c) and can be found in adc0710\src\r_adc_isr.c. 

  Parameters:
  Unit       - Instance number
  
  Returns:
  void
*/

void R_ADC_Isr2(uint32_t Unit);



/*******************************************************************************
  Function: R_ADC_ScanGroupProirtyOrder
  
  Determines the Scan group priorty order. 
  
  Parameters:
  Unit          - Instance number
  Order         - Order can be 0x43210 or 0x32401    

  Returns:
  see: <r_adc_Error_t>
*/

r_adc_Error_t R_ADC_ScanGroupProirtyOrder(uint32_t Unit, uint32_t Order);


/*******************************************************************************
  Function: R_ADC_StartGroupConversionTsn

  Start the Conversion group for the TSN. 
  
  Parameters:
  Unit          - Instance number
  
  Returns:
  see: <r_adc_Error_t>
  
*/

r_adc_Error_t R_ADC_StartGroupConversionTsn(uint32_t Unit);


/*******************************************************************************
  Function: R_ADC_GetTemperatureTsn

  Gets the temperature conversion result. 
  
  Parameters:
  Unit          - Instance number
  
  Returns:
  Conversion result for the Temperature sensor
  
*/

uint16_t R_ADC_GetTemperatureTsn(uint32_t Unit);


/*******************************************************************************
  Function: R_ADC_ConfigLimitsTsn

  Configures the TSN 

  Parameters:
  Unit          - Instance number
  Config        - Config parameters see: <R_ADC_ConfigLimitsTsn>
  
  Returns:
  see: <r_adc_Error_t>
  
*/

r_adc_Error_t R_ADC_ConfigLimitsTsn(uint32_t Unit, r_adc_ULLTableCfg_t Config);


#ifdef __cplusplus
}
#endif

#endif /* _ADC_API_H */

