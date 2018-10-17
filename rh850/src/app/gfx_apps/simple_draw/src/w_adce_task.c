
/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "w_adce_task.h"
#include "w_system.h"
#include "r_adc_api.h"
#include "nios.h"

extern mars_info_def gtmars_info;
extern queue_task_def gtqueue_manager;
extern task_recorder_def gttask_recorder[_TASK_NUM];
extern void R_TICK_WaitMS(uint32_t Unit, uint32_t TickMS);
//============================================================================================================
// Table defines
//============================================================================================================
void (*const ADCE_State_Machine[_MAX_ADCE_ST_NO])(void) = 
{W_ADCE_Init , W_ADCE_Process , W_ADCE_Error};

/*============================================================================
 Variables
============================================================================*/
r_adc_Config_t tadce_config;
r_adc_ResParams_t tadc_key_parms;
r_adc_ResParams_t tadc_pannel_parms;
r_adc_ResParams_t tadc_system_parms;
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Init_Config(void)
{
    uint8_t u8temp;
    for(u8temp=0; u8temp<3; u8temp++)
    {
        tadce_config.VCh[u8temp].phyChannel   = 0x1C + u8temp;
        tadce_config.VCh[u8temp].INT_VCEnd   = R_ADC_COVEND_IGNORED;
        tadce_config.VCh[u8temp].ULLCheck   = R_ADC_ULL_0;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Paramter(r_adc_ResParams_t *tadc_paramter,uint8_t u8channel)
{
    tadc_paramter->ScanGroup = R_ADC_CG_1;
    tadc_paramter->Channel[0] = 0;
    tadc_paramter->PhyChannel = u8channel;
    tadc_paramter->Align = R_ADC_RIGHT_ALIGNED;
    tadc_paramter->BitNo = R_ADC_12BIT_RES;
    tadc_paramter->OwStatus = 0;
    tadc_paramter->UlStatus = 0;
    tadc_paramter->ErrChannel = 0;
    tadc_paramter->OwChannel = 0;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Init_Paramter(void)
{
    W_ADCE_Paramter(&tadc_key_parms, _ADCE_KEY_CHANNEL);
    W_ADCE_Paramter(&tadc_pannel_parms, _ADCE_LCD_TEMP_CHANNEL);
    W_ADCE_Paramter(&tadc_system_parms, _ADCE_SYS_TEMP_CHANNEL);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Init(void)
{
    R_ADC_ResetCfgParams(&tadce_config);
    W_ADCE_Init_Config();
    W_ADCE_Init_Paramter();
    R_ADC_Init(0, &tadce_config);
    W_Task_ChangeState(_TYPE_ADCE, _LEVEL5, _STATE_ADCE_PROCESS, ADCE_State_Machine[_STATE_ADCE_PROCESS]);
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Process(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;
    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tadc_task_info.u16timer2 = _TIME_20ms;
            gtmars_info.tadc_task_info.u16timer3 = _TIME_150ms;
            gtmars_info.tadc_task_info.u16timer4 = _TIME_250ms;
            break;
        case _EVENT_DATA:
            break;
        case _EVENT_TIMER1:
            break;
        case _EVENT_TIMER2:
            R_ADC_StartGroupConversion(0, R_ADC_CG_1);
            R_TICK_WaitMS(0,1);
            R_ADC_ReadRes(0, &tadc_key_parms);
            gtmars_info.tadc_task_info.u16timer2 = _TIME_20ms;
            break;
        case _EVENT_TIMER3:
            R_ADC_StartGroupConversion(0, R_ADC_CG_1);
            R_TICK_WaitMS(0,1);
            R_ADC_ReadRes(0, &tadc_pannel_parms);
            gtmars_info.tadc_task_info.u16timer3 = _TIME_3000ms;
            break;
        case _EVENT_TIMER4:
            R_ADC_StartGroupConversion(0, R_ADC_CG_1);
            R_TICK_WaitMS(0,1);
            R_ADC_ReadRes(0, &tadc_system_parms);
            gtmars_info.tadc_task_info.u16timer3 = _TIME_1000ms;
            break;

        default:
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Error(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tadc_task_info.u16timer1 = _TIME_5000ms;
            break;
        case _EVENT_TIMER1:
            W_Task_ChangeState(_TYPE_ADCE, _LEVEL5, _STATE_ADCE_INIT, ADCE_State_Machine[_STATE_ADCE_INIT]);
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Timer1(void)
{
    if(gtmars_info.tadc_task_info.u16timer1 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tadc_task_info.u16timer1 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_ADCE, _LEVEL3, _EVENT_TIMER1);
        gtmars_info.tadc_task_info.u16timer1--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Timer2(void)
{
    if(gtmars_info.tadc_task_info.u16timer2 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tadc_task_info.u16timer2 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_ADCE, _LEVEL3, _EVENT_TIMER2);
        gtmars_info.tadc_task_info.u16timer2--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Timer3(void)
{
    if(gtmars_info.tadc_task_info.u16timer3 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tadc_task_info.u16timer3 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_ADCE, _LEVEL3, _EVENT_TIMER3);
        gtmars_info.tadc_task_info.u16timer3--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_Timer4(void)
{
    if(gtmars_info.tadc_task_info.u16timer4 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tadc_task_info.u16timer4 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_ADCE, _LEVEL5, _EVENT_TIMER4);
        gtmars_info.tadc_task_info.u16timer4--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_ADCE_AllTimer(void)
{
    W_ADCE_Timer1();
    W_ADCE_Timer2();
    W_ADCE_Timer3();
    W_ADCE_Timer4();
}
//----------------------------------------------------------------------------------------------------------------------------

