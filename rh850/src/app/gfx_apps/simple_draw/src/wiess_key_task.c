// firmware version v1.00.01

#include "wiess_key_task.h"
#include "w_cluster_task.h"
#include "r_dev_api.h"
#include "r_gpio_api.h"
#include "r_typedefs.h"
#include "w_system.h"
#include "nios.h"



extern mars_info_def gtmars_info;
w_dev_button_t gtbutton_ACC;
//w_dev_button_t gtbutton_ILL;
//w_dev_button_t gtbutton_SPD;
//w_dev_button_t gtbutton_DR;
w_dev_button_t gtbutton_HEADLAMP_H;
w_dev_button_t gtbutton_HEADLAMP_L;
w_dev_button_t gtbutton_TAIL_LAMP;
w_dev_button_t gtbutton_REVERSE_LAMP;
w_dev_button_t gtbutton_LEFT_TURN_LAMP;
w_dev_button_t gtbutton_RIGHT_TURN_LAMP;
w_dev_button_t gtbutton_FRONT_FOG_LAMP;
w_dev_button_t gtbutton_REAR_FOG_LAMP;
w_dev_button_t gtbutton_CHARGE;
w_dev_button_t gtbutton_IGN;
w_dev_button_t gtbutton_DEMO;

//*************************************************************************************************************
static const r_dev_PinConfig_t loc_ACCPinConfig[] = 
{
    /* P0_3 - GPIO input         pins */
    {ACC_Port, ACC_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  ACC Input */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};

static const r_dev_PinConfig_t loc_CHARGEPinConfig[] =
{
    /* P0_0 - GPIO input         pins */
    {CHARGE_Port, CHARGE_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  CHARGE Input */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};

static const r_dev_PinConfig_t loc_IGNPinConfig[] =
{
    /* P0_0 - GPIO input         pins */
    {IGN_Port, IGN_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  IGN Input */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};
//static const r_dev_PinConfig_t loc_ILLPinConfig[] = 
//{
//    /* P1_5 - GPIO input         pins */
//    {ILL_Port, ILL_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Car ILL input */
//    /* delimiter - do not remove */
//    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
//};
//static const r_dev_PinConfig_t loc_SpeedPinConfig[] = 
//{
//    /* P16_5 - GPIO input         pins */
//    {SPD_Port, SPD_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Speed button input */
//    /* delimiter - do not remove */
//    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
//};
//static const r_dev_PinConfig_t loc_DRPinConfig[] = 
//{
//    /* P1_0 - GPIO input         pins */
//    {DR_Port, DR_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  DR Input */
//    /* delimiter - do not remove */
//    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
//};

static const r_dev_PinConfig_t loc_LampPinConfig[] =
{
    /* P1_4 - GPIO input         pins */
    {HEAD_LAMP_H_Port, HEAD_LAMP_H_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Head lamp hight input */
    /* P1_5 - GPIO input         pins */
    {HEAD_LAMP_L_Port, HEAD_LAMP_L_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Head lamp low input */
    /* P1_0 - GPIO input         pins */
    {TAIL_LAMP_Port, TAIL_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Tail lamp input */
    /* P3_0 - GPIO input         pins */
    {REVERSE_LAMP_Port, REVERSE_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Reverse lamp input */
    /* P1_3 - GPIO input         pins */
    {LEFT_TURN_LAMP_Port, LEFT_TURN_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Left Turn lamp input */
    /* P3_2 - GPIO input         pins */
    {RIGHT_TURN_LAMP_Port, RIGHT_TURN_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Right Turn lamp input */
    /* P1_1 - GPIO input         pins */
    {FRONT_FOG_LAMP_Port, FRONT_FOG_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Front Fog lamp input */
    /* P3_1 - GPIO input         pins */
    {REAR_FOG_LAMP_Port, REAR_FOG_LAMP_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Rear Fog lamp input */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};

static const r_dev_PinConfig_t loc_DemoPinConfig[] =
{
    /* P1_4 - GPIO input         pins */
    {DEMO_Port, DEMO_Pin, 0, R_DEV_PIN_IN   , 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* I:  Rear Fog lamp input */
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};



//*************************************************************************************************************
void W_Button_Parameter_Init(w_dev_button_t *ptbutton ,uint32_t u32port, uint8_t u8pin, uint8_t u8level)
{
    ptbutton->u32port = u32port;
    ptbutton->u8pin = u8pin;
    ptbutton->u16high_level_counter = 0;
    ptbutton->u16low_level_counter = 0;
    ptbutton->u8level_status = u8level;
}
//*************************************************************************************************************
void W_Button_Init(void)
{
    R_DEV_PinInit(loc_ACCPinConfig);
    R_DEV_PinInit(loc_CHARGEPinConfig);
    R_DEV_PinInit(loc_IGNPinConfig);
//    R_DEV_PinInit(loc_ILLPinConfig);
//    R_DEV_PinInit(loc_SpeedPinConfig);
//    R_DEV_PinInit(loc_DRPinConfig);
    R_DEV_PinInit(loc_LampPinConfig);
    R_DEV_PinInit(loc_DemoPinConfig);

    W_Button_Parameter_Init(&gtbutton_ACC,ACC_Port,ACC_Pin,_HIGHT_LEVEL);
    W_Button_Parameter_Init(&gtbutton_IGN,IGN_Port,IGN_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_CHARGE,CHARGE_Port,CHARGE_Pin,_HIGHT_LEVEL);
//    W_Button_Parameter_Init(&gtbutton_ILL,ILL_Port,ILL_Pin,_HIGHT_LEVEL);
//    W_Button_Parameter_Init(&gtbutton_SPD,SPD_Port,SPD_Pin,_HIGHT_LEVEL);
//    W_Button_Parameter_Init(&gtbutton_DR,DR_Port,DR_Pin,_HIGHT_LEVEL);
    W_Button_Parameter_Init(&gtbutton_HEADLAMP_H,HEAD_LAMP_H_Port,HEAD_LAMP_H_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_HEADLAMP_L,HEAD_LAMP_L_Port,HEAD_LAMP_L_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_TAIL_LAMP,TAIL_LAMP_Port,TAIL_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_REVERSE_LAMP,REVERSE_LAMP_Port,REVERSE_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_LEFT_TURN_LAMP,LEFT_TURN_LAMP_Port,LEFT_TURN_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_RIGHT_TURN_LAMP,RIGHT_TURN_LAMP_Port,RIGHT_TURN_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_FRONT_FOG_LAMP,FRONT_FOG_LAMP_Port,FRONT_FOG_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_REAR_FOG_LAMP,REAR_FOG_LAMP_Port,REAR_FOG_LAMP_Pin,_LOW_LEVEL);
    W_Button_Parameter_Init(&gtbutton_DEMO,DEMO_Port,DEMO_Pin,_HIGHT_LEVEL);
}
//*************************************************************************************************************
uint8_t W_Button_Detect(w_dev_button_t* ptbutton, uint16_t u16debounce_max)
{
    if(R_GPIO_PinRead(ptbutton->u32port,ptbutton->u8pin) == _HIGHT_LEVEL)
    {
        if(ptbutton->u8level_status != _HIGHT_LEVEL)
        {
            if(ptbutton->u16high_level_counter >= u16debounce_max)
            {
                ptbutton->u8level_status = _HIGHT_LEVEL;
                return  ptbutton->u8level_status;
            }
            ptbutton->u16high_level_counter++;
        }
        else
        {
            ptbutton->u16high_level_counter = 0;
            ptbutton->u16low_level_counter = 0;
        }
    }
    else if(R_GPIO_PinRead(ptbutton->u32port,ptbutton->u8pin) == _LOW_LEVEL)
    {
        if(ptbutton->u8level_status != _LOW_LEVEL)
        {
            if(ptbutton->u16low_level_counter >= u16debounce_max)
            {
                ptbutton->u8level_status = _LOW_LEVEL;
                return ptbutton->u8level_status;
            }
            ptbutton->u16low_level_counter++;
        }
        else
        {
            ptbutton->u16high_level_counter = 0;
            ptbutton->u16low_level_counter = 0;
        }
    }
    return ptbutton->u8level_status;
}
//*************************************************************************************************************
void W_Button_Timer1(void)
{
    if(gtmars_info.tbutton_task_info.u16timer1 > 0)
    {
        gtmars_info.tbutton_task_info.u16timer1--;
    }
}
//*************************************************************************************************************
void W_Button_AllTimer(void)
{
    W_Button_Timer1();
}
//*************************************************************************************************************
void W_Get_Start_Procedure(void)
{
    uint8_t i = 0;
    gtmars_info.tdemo_info.u8system_mode = 1;
#if 0
    while ((gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status == _G_FAIL) &&
            gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.charge_status == _G_FAIL)
    {
        if(gtmars_info.tbutton_task_info.u16timer1 == 0)
        {
            gtmars_info.tbutton_task_info.u16timer1 = 1;
            gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status = W_Button_Detect(&gtbutton_ACC,10); // 100ms debounce
            gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.charge_status = W_Button_Detect(&gtbutton_CHARGE,10); // 100ms debounce
            gtmars_info.tdemo_info.u8system_mode = W_Button_Detect(&gtbutton_DEMO,10); // 100ms debounce
        }
    }
    i = 1;
#else if
    while(gtbutton_ACC.u8level_status == _HIGHT_LEVEL) //&& (gtbutton_CHARGE.u8level_status == _HIGHT_LEVEL))
    {
        if(gtmars_info.tbutton_task_info.u16timer1 == 0)
        {
            gtmars_info.tbutton_task_info.u16timer1 = 1;
            gtbutton_ACC.u8level_status = gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status = W_Button_Detect(&gtbutton_ACC,10); // 100ms debounce
//            gtbutton_CHARGE.u8level_status = gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.charge_status = W_Button_Detect(&gtbutton_CHARGE,10); // 100ms debounce
            gtmars_info.tdemo_info.u8system_mode = W_Button_Detect(&gtbutton_DEMO,10); // 100ms debounce
        }
    }
#endif
}

