
/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "r_gpio_api.h"
#include "wiess_touch_task.h"
#include "wiess_system.h"
#include "nios.h"
#include "stdlib.h"

extern mars_info_def gtmars_info;
extern queue_task_def gtqueue_manager;
extern task_recorder_def gttask_recorder[_TASK_NUM];

void (*const Touch_State_Machine[_MAX_TOUCH_ST_NO])(void) = 
{W_Touch_Init , W_Touch_Waiting , W_Touch_Error};

uint8_t u8touch_data[_TOUCH_DATA_NUM] = {0};

const r_dev_PinConfig_t criic_pin_config[] = 
{
    /* I2C   Data   pins */
    {44, 10, 4, R_DEV_PIN_OUT , 1u, R_DEV_PIN_PULLNO, 1u, 0u, 0u, R_DEV_PIN_SCHMITT4},
    /* I2C Clock pins */
    {44, 11, 4, R_DEV_PIN_OUT , 1u, R_DEV_PIN_PULLNO, 1u, 0u, 0u, R_DEV_PIN_SCHMITT4},

    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};
//----------------------------------------------------------------------------------------------------------------------------
const r_dev_PinConfig_t ceint_pin_config[] = 
{
    /* Touch interrupt input           pins */
    {45, 12, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1},

    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_RIIC_GPIO_Init(void)
{
    /* Set   SDA as P44_10 and SCL as P44_11 */
    R_DEV_PinInit(criic_pin_config);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_EINT_GPIO_Init(void)
{
    /* Set   Touch interrupt pin as P45_12 */
    R_DEV_PinInit(ceint_pin_config);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Finger_Data_Flush(void)
{
    uint8_t u8counter;
    for(u8counter = 0; u8counter < 5; u8counter++)
    {
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].efinger_action = eTouch_None;
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16finger_id = 0xFFFF;
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position = 0xFFFF;
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position = 0xFFFF;

    }
    gtmars_info.ttouch_task_info.ttouch_finger.u8finger_num = 0;
    gtmars_info.ttouch_task_info.u8touch_flag = R_FALSE;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Finger_Data_Store(void)
{
    uint8_t u8counter;
    for(u8counter = 0; u8counter < 5; u8counter++)
    {
        gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].efinger_action = gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].efinger_action;
        gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].u16finger_id = gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16finger_id;
        gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].u16x_position = gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position;
        gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].u16y_position = gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position;
    }
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Check_Mode_Switch(    touch_finger_info_def *ttouch_finger)
{
    uint8_t u8counter;
    for(u8counter = 0; u8counter < 5; u8counter++)
    {
        if(ttouch_finger->ttouch_position[u8counter].u16x_position < _MODE_ICON_X_RANGE_MAX && ttouch_finger->ttouch_position[u8counter].u16x_position > _MODE_ICON_X_RANGE_MIN)
        {
            if(ttouch_finger->ttouch_position[u8counter].u16y_position < _MODE_ICON_Y_RANGE_MAX && ttouch_finger->ttouch_position[u8counter].u16y_position > _MODE_ICON_Y_RANGE_MIN)
            {
/*                if(gtmars_info.tcluster_info.tcluster_display.u8currnt_mode == _CLUSTER_MODE_COMFORT)
                    gtmars_info.tcluster_info.tcluster_display.u8currnt_mode = _CLUSTER_MODE_SPORT;
                else
                    gtmars_info.tcluster_info.tcluster_display.u8currnt_mode = _CLUSTER_MODE_COMFORT;
*/
                break;
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Check_Finger_Data(uint8_t *prdata)
{
    uint8_t u8finger_num = prdata[3];
    uint8_t u8counter;
    uint16_t u16x_diff;
    uint16_t u16y_diff;
    
    gtmars_info.ttouch_task_info.ttouch_finger.u8finger_num = u8finger_num;

    for(u8counter = 0; u8counter < u8finger_num; u8counter++)
    {
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].efinger_action = prdata[4 + u8counter * 10];
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16finger_id = prdata[5 + u8counter * 10];
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position = prdata[7 + u8counter * 10];
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position <<= 8;
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position += prdata[6 + u8counter * 10];
        
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position = prdata[9 + u8counter * 10];
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position <<= 8;
        gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position += prdata[8 + u8counter * 10];
    }

    for(u8counter = 0; u8counter < u8finger_num; u8counter++)
    {
        if(gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].efinger_action  == eTouch_Down)
        {
            W_Touch_Finger_Data_Store();
            gtmars_info.ttouch_task_info.u8touch_flag = R_TRUE;
   
        }
        else if(gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].efinger_action  == eTouch_Lift)
        {
            if(abs((gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16x_position - gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].u16x_position) < _FINGER_X_RANGE) &&
               abs((gtmars_info.ttouch_task_info.ttouch_finger.ttouch_position[u8counter].u16y_position - gtmars_info.ttouch_task_info.ttouch_finger.tpretouch_position[u8counter].u16y_position) < _FINGER_Y_RANGE))
            {
                if(gtmars_info.ttouch_task_info.u8touch_flag == R_TRUE)
                {
                    W_Touch_Check_Mode_Switch(&gtmars_info.ttouch_task_info.ttouch_finger);
                    gtmars_info.ttouch_task_info.u8touch_flag = R_FALSE;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------
r_riic_Error_t W_Touch_RIIC_Init(void)
{
    r_riic_Parameter_t triic;
    triic.Baudrate = _RIIC_CLOCK_FREQ;
    return R_RIIC_Init(_RIIC_UNIT,&triic);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Init(void)
{
    gtmars_info.ttouch_task_info.u16timer1 = _TIME_DISABLE;
    gtmars_info.ttouch_task_info.u16timer2 = _TIME_DISABLE;

    W_Touch_RIIC_GPIO_Init();
    W_Touch_EINT_GPIO_Init();

    W_Touch_RIIC_Init();
    W_Task_ChangeState(_TYPE_TOUCH, _LEVEL5, _STATE_TOUCH_WAITING, Touch_State_Machine[_STATE_TOUCH_WAITING]);
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Waiting(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;
    uint8_t u8slave_address;
    uint8_t u8counter,u8temp;
    
    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.ttouch_task_info.u8touch_flag = R_FALSE;
            gtmars_info.ttouch_task_info.u8release_flag = R_TRUE;
            gtmars_info.ttouch_task_info.u16int_high_debounce_Counter = 0;
            gtmars_info.ttouch_task_info.u16int_low_debounce_counter = 0;
            Task_Done(gtqueue_manager.u8current_point);
            W_Task_ChangeEvent(_TYPE_TOUCH, _LEVEL4, _EVENT_LOOP);
            return;
        case _EVENT_LOOP:
            /*if( R_GPIO_PinRead(45,12) == 1)
            {
                gtmars_info.ttouch_task_info.u16int_high_debounce_Counter++;
                gtmars_info.ttouch_task_info.u16int_low_debounce_counter = 0;

            }
            else
            {
                gtmars_info.ttouch_task_info.u16int_high_debounce_Counter = 0;
                gtmars_info.ttouch_task_info.u16int_low_debounce_counter++;
            }

            if( gtmars_info.ttouch_task_info.u16int_low_debounce_counter > _MAX_LOW_DEBOUNCE )
            {
                u8slave_address = _TOUCH_ADDRESS;
                u8slave_address = (u8slave_address  << 1);
                gtmars_info.ttouch_task_info.u16int_low_debounce_counter = 0;

                W_RIIC_receive_from_slave(_RIIC_UNIT,u8slave_address,&u8touch_data,_TOUCH_DATA_NUM);
                W_Touch_Check_Finger_Data(&u8touch_data);

                if( gtmars_info.ttouch_task_info.u8release_flag == R_TRUE)
                {
                    W_Touch_Check_Mode_Switch(&gtmars_info.ttouch_task_info.ttouch_finger);
                    gtmars_info.ttouch_task_info.u8release_flag = R_FALSE;
                }
            }
            
            if( gtmars_info.ttouch_task_info.u16int_high_debounce_Counter > _MAX_HIGH_DEBOUNCE )
            {
                gtmars_info.ttouch_task_info.u8release_flag = R_TRUE;
                gtmars_info.ttouch_task_info.u16int_high_debounce_Counter = 0;
                gtmars_info.ttouch_task_info.u16int_low_debounce_counter = 0;
            }*/
            while(R_GPIO_PinRead(45,12) == 0)
            {
                u8slave_address = _TOUCH_ADDRESS;
                u8slave_address = (u8slave_address  << 1);
                gtmars_info.ttouch_task_info.u16int_low_debounce_counter = 0;
                W_RIIC_receive_from_slave(_RIIC_UNIT,u8slave_address,&u8touch_data,_TOUCH_DATA_NUM);
                W_Touch_Check_Finger_Data(&u8touch_data);
            }
            
            Task_Done(gtqueue_manager.u8current_point);
            W_Task_ChangeEvent(_TYPE_TOUCH, _LEVEL4, _EVENT_LOOP);
            return;
        case _EVENT_TIMER1:
            break;
        default:
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Error(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tcar_info.u16timer1 = _TIME_5000ms;
            break;
        case _EVENT_TIMER1:
            W_Task_ChangeState(_TYPE_TOUCH, _LEVEL5, _STATE_TOUCH_INIT, Touch_State_Machine[_STATE_TOUCH_INIT]);
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Timer1(void)
{
    if(gtmars_info.ttouch_task_info.u16timer1 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.ttouch_task_info.u16timer1 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_TOUCH, _LEVEL3, _EVENT_TIMER1);
        gtmars_info.tcar_info.u16timer1--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_Timer2(void)
{
    if(gtmars_info.ttouch_task_info.u16timer2 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.ttouch_task_info.u16timer2 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_TOUCH, _LEVEL3, _EVENT_TIMER2);
        gtmars_info.ttouch_task_info.u16timer2--;

    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Touch_AllTimer(void)
{
    W_Touch_Timer1();
    W_Touch_Timer2();
}

