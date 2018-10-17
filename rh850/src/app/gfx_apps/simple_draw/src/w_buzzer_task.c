
/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "w_buzzer_task.h"
#include "w_system.h"
#include "w_sys_pwm.h"
#include "nios.h"

extern mars_info_def gtmars_info;

volatile const uint16_t WARNING_TABLE[][3] = {
    /*TONE*/    /*DELAY*/ /*NEXT STATUS*/
    {BUZZER_TONE1, 6000,  _STATE_END}   //'4Khz' 600ms
};
    
volatile const uint16_t TICK_TABLE[][3] = {
    /*TONE*/    /*DELAY*/ /*NEXT STATUS*/
    {BUZZER_TONE2, 12,  _STATE_DECODE},
    {BUZZER_OFF,   10,  _STATE_DECODE},
    {BUZZER_TONE2, 8,  _STATE_DECODE},
    {BUZZER_OFF,   10,  _STATE_DECODE},
    {BUZZER_TONE2, 4,  _STATE_DECODE},
    {BUZZER_OFF,   1000,  _STATE_END},
};
    
volatile const uint16_t TOCK_TABLE[][3] = {
    /*TONE*/    /*DELAY*/ /*NEXT STATUS*/
    {BUZZER_TONE2, 12,  _STATE_DECODE}, //3 '2.5KHz' plus
    {BUZZER_OFF,   1000,  _STATE_END},    //1ms off
};

//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_Init(void)
{
    W_PWMBUZZER_Init();
}
//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_Process(void)
{
    if(gtmars_info.tbuzzer_task_info.tsys_flag.start_bit != _TRUE)
    {
        return;
    }
    if(gtmars_info.tbuzzer_task_info.tsys_flag.delay_bit == _TRUE)
    {
        if(gtmars_info.tbuzzer_task_info.u16timer1 > 0)
        {
            return;
        }
        else
        {
            gtmars_info.tbuzzer_task_info.tsys_flag.delay_bit = _FALSE;
        }
    }
    switch(gtmars_info.tbuzzer_task_info.u16status)
    {
        case _STATE_DECODE:
            gtmars_info.tbuzzer_task_info.u16buzzer_id = *gtmars_info.tbuzzer_task_info.u16_sts_ptr++;
            if(gtmars_info.tbuzzer_task_info.u16buzzer_id == BUZZER_TONE1)
            {
                W_BUZZER_SetTone(BUZZER_TONE1);
            }
            else if (gtmars_info.tbuzzer_task_info.u16buzzer_id == BUZZER_TONE2)
            {
                W_BUZZER_SetTone(BUZZER_TONE2);
            }
            else
            {
                W_BUZZER_SetTone(BUZZER_OFF);
            }
            break;
            gtmars_info.tbuzzer_task_info.u16timer1 = *gtmars_info.tbuzzer_task_info.u16_sts_ptr++;
            if(gtmars_info.tbutton_task_info.u16timer1 != 0)
            {
                gtmars_info.tbuzzer_task_info.tsys_flag.delay_bit = _TRUE;
            }
            gtmars_info.tbuzzer_task_info.u16status = *gtmars_info.tbuzzer_task_info.u16_sts_ptr++;
        case _STATE_END:
            gtmars_info.tbuzzer_task_info.tsys_flag.start_bit = _FALSE;
            W_BUZZER_SetTone(BUZZER_OFF);
        default:
            break;
    }
}

// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_Timer1(void)
{
    if(gtmars_info.tbuzzer_task_info.u16timer1 > 0)
    {
        gtmars_info.tbuzzer_task_info.u16timer1--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_AllTimer(void)
{
    W_BUZZER_Timer1();
}
//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_GET_TABLE_ADDR(uint8_t table_index)
{
    gtmars_info.tbuzzer_task_info.u8table_index = table_index;
    switch (table_index)
    {
    case _BUZZER_WARNING:
        gtmars_info.tbuzzer_task_info.u16_sts_ptr = (uint16_t *)WARNING_TABLE;
        break;
    case _BUZZER_TICK:
        gtmars_info.tbuzzer_task_info.u16_sts_ptr = (uint16_t *)TOCK_TABLE;
        break;
    case _BUZZER_TOCK:
        gtmars_info.tbuzzer_task_info.u16_sts_ptr = (uint16_t *)TICK_TABLE;
        break;
    default:
        break;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_BUZZER_START(uint8_t table_index)
{
    gtmars_info.tbuzzer_task_info.u16timer1 = 0;
    gtmars_info.tbuzzer_task_info.tsys_flag.start_bit = _TRUE;
    gtmars_info.tbuzzer_task_info.tsys_flag.delay_bit = _FALSE;
    gtmars_info.tbuzzer_task_info.u16status = _STATE_DECODE;
    W_BUZZER_GET_TABLE_ADDR(table_index);
}
