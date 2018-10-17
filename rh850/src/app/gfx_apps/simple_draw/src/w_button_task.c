
/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "w_button_task.h"
#include "w_system.h"
#include "nios.h"
#include "r_adc_api.h"

extern mars_info_def gtmars_info;
extern queue_task_def gtqueue_manager;
extern task_recorder_def gttask_recorder[_TASK_NUM];
extern void R_TICK_WaitMS(uint32_t Unit, uint32_t TickMS);
//============================================================================================================
// Table defines
//============================================================================================================
void (*const Button_State_Machine[_MAX_BUTTON_ST_NO])(void) = 
{W_Button_Init , W_Button_Process , W_Button_Error};

/*============================================================================
 Variables
============================================================================*/

//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Paramter_Clr(void)
{
    uint8_t u8clearcnt;

    for(u8clearcnt = 0; u8clearcnt < 5; u8clearcnt ++)
    {
        gtmars_info.tbutton_task_info.u8checkdata[u8clearcnt] = 0;
    }
    gtmars_info.tbutton_task_info.u8enter_checkcnt  = 0;
    gtmars_info.tbutton_task_info.u8down_checkcnt   = 0;
    gtmars_info.tbutton_task_info.u8up_checkcnt     = 0;
    gtmars_info.tbutton_task_info.u8menu_checkcnt   = 0;
    gtmars_info.tbutton_task_info.u8button_status   = Button_Release;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Event(uint8_t u8button_state)
{
    gtmars_info.tbutton_task_info.u8button_status   = u8button_state;
    W_Task_ChangeEvent(_TYPE_BUTTON, _LEVEL5, _EVENT_DATA);

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Detect(void)
{
    if(r_adc_CovResult[0][1][28] > _BUTTON_RELEASE_VALUE)
    {
        if(gtmars_info.tbutton_task_info.u8button_status != Button_Release)
            W_Button_Paramter_Clr();
        return;  
    }
    if(gtmars_info.tbutton_task_info.u8button_status != Button_Release)
        return;
    
    if(r_adc_CovResult[0][1][28] < _BUTTON_ENTER_AD_MAX &&  r_adc_CovResult[0][1][28] > _BUTTON_ENTER_AD_MIN)
    {
        gtmars_info.tbutton_task_info.u8checkdata[gtmars_info.tbutton_task_info.u8enter_checkcnt] = r_adc_CovResult[0][1][28];
        gtmars_info.tbutton_task_info.u8enter_checkcnt ++;
        if(gtmars_info.tbutton_task_info.u8enter_checkcnt >= 4)
            W_Button_Event(Button_Enter);
    }
    else if(r_adc_CovResult[0][1][28] < _BUTTON_DOWN_AD_MAX &&  r_adc_CovResult[0][1][28] > _BUTTON_DOWN_AD_MIN)
    {
        gtmars_info.tbutton_task_info.u8checkdata[gtmars_info.tbutton_task_info.u8enter_checkcnt] = r_adc_CovResult[0][1][28];
        gtmars_info.tbutton_task_info.u8down_checkcnt ++;
        if(gtmars_info.tbutton_task_info.u8down_checkcnt >=4)
            W_Button_Event(Button_Down);
    }
    else if(r_adc_CovResult[0][1][28] < _BUTTON_UP_AD_MAX &&  r_adc_CovResult[0][1][28] > _BUTTON_UP_AD_MIN)
    {
        gtmars_info.tbutton_task_info.u8checkdata[gtmars_info.tbutton_task_info.u8enter_checkcnt] = r_adc_CovResult[0][1][28];
        gtmars_info.tbutton_task_info.u8up_checkcnt ++;
        if(gtmars_info.tbutton_task_info.u8up_checkcnt >=4)
            W_Button_Event(Button_Up);
    }
    else if(r_adc_CovResult[0][1][28] < _BUTTON_MENU_AD_MAX &&  r_adc_CovResult[0][1][28] > _BUTTON_MENU_AD_MIN)
    {
        gtmars_info.tbutton_task_info.u8checkdata[gtmars_info.tbutton_task_info.u8enter_checkcnt] = r_adc_CovResult[0][1][28];
        gtmars_info.tbutton_task_info.u8menu_checkcnt ++;
        gtmars_info.tbutton_task_info.u16menutimer = _TIME_3000ms; //check long press
        if(gtmars_info.tbutton_task_info.u8menu_checkcnt >=4)
            W_Button_Event(Button_Menu);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Action(void)
{
    //send button status to disp variable
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Init(void)
{
    W_Button_Paramter_Clr();
    W_Task_ChangeState(_TYPE_BUTTON, _LEVEL5, _STATE_BUTTON_PROCESS, Button_State_Machine[_STATE_BUTTON_PROCESS]);
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Process(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;
    switch(u8event)
    {
        case _EVENT_FIRST:
            break;
        case _EVENT_DATA:
            W_Button_Action();
            break;
        case _EVENT_TIMER1:
            break;
        case _EVENT_TIMER2:
            break;
        default:
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Error(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tbutton_task_info.u16timer1 = _TIME_5000ms;
            break;
        case _EVENT_TIMER1:
            W_Task_ChangeState(_TYPE_BUTTON, _LEVEL5, _STATE_BUTTON_INIT, Button_State_Machine[_STATE_BUTTON_INIT]);
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Timer1(void)
{
    if(gtmars_info.tbutton_task_info.u16timer1 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tbutton_task_info.u16timer1 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_BUTTON, _LEVEL3, _EVENT_TIMER1);
        gtmars_info.tbutton_task_info.u16timer1--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Button_Timer2(void)
{
    if(gtmars_info.tbutton_task_info.u16menutimer == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tbutton_task_info.u16menutimer == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_BUTTON, _LEVEL3, _EVENT_TIMER2);
        gtmars_info.tbutton_task_info.u16menutimer--;
    }
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Button_AllTimer(void)
{
    W_Button_Timer1();
    W_Button_Timer2();
}
//----------------------------------------------------------------------------------------------------------------------------

