#include "w_system.h"
#include "w_buzzer_task.h"
#include "wiess_key_task.h"
#include "w_mag.h"
#include "w_cluster_task.h"
#include "w_rscan_task.h"
#include "w_sys_pwm.h"
#include "w_sys_reset.h"
#include "nios.h"
//#include "w_canio_gen.h"

extern mars_info_def gtmars_info;
extern w_dev_button_t gtbutton_ACC;
extern w_dev_button_t gtbutton_HEADLAMP_H;
extern w_dev_button_t gtbutton_HEADLAMP_L;
extern w_dev_button_t gtbutton_TAIL_LAMP;
extern w_dev_button_t gtbutton_REVERSE_LAMP;
extern w_dev_button_t gtbutton_LEFT_TURN_LAMP;
extern w_dev_button_t gtbutton_RIGHT_TURN_LAMP;
extern w_dev_button_t gtbutton_FRONT_FOG_LAMP;
extern w_dev_button_t gtbutton_REAR_FOG_LAMP;
extern w_dev_button_t gtbutton_CHARGE;
extern w_dev_button_t gtbutton_IGN;
extern void R_GPIO_WritePin(int32_t Port, uint8_t Pin, uint8_t Value);

//*************************************************************************************************************
void W_SYS_Reset(void)
{
    R_GPIO_WritePin(16,9,0);    // close backlight
    W_RESET_Software();
}
//*************************************************************************************************************
void W_SYS_Check_CAN_Message(void)
{
    double dChargePercent = 0;
    W_RSCAN_Read_FIFO();
    W_RSCAN_Data_Process();
    /**********************************
     * Calculating Charge Percent
     **********************************/
    if (gtmars_info.tcar_info.tcan_id53f_info.u16battery_remain_capacity == 0)
    {
        dChargePercent = 0;
    }
    else
    {
        dChargePercent = (gtmars_info.tcar_info.tcan_id53f_info.u16battery_remain_capacity * 100)/gtmars_info.tcar_info.tcan_id53f_info.u16battery_design_capacity;
    }
    /****************************************
     * Charge 10%
     ****************************************/
    if((gtmars_info.tcar_info.tcan_id53f_info.tsys_consistency_check3_info.low_soc_light_on == 1) && (dChargePercent == 10))
    {
        if(gtmars_info.tbuzzer_task_info.twarnning_info.charge_10percent == 0)
        {
            W_BUZZER_START(_BUZZER_WARNING);
            gtmars_info.tbuzzer_task_info.twarnning_info.charge_10percent = 1;
        }
    }
    else
    {
        gtmars_info.tbuzzer_task_info.twarnning_info.charge_10percent = 0;
    }
    /****************************************
     * Charge 20%
     ****************************************/
    if((gtmars_info.tcar_info.tcan_id53f_info.tsys_consistency_check3_info.low_soc_light_on == 1) && (dChargePercent == 20))
    {
        if(gtmars_info.tbuzzer_task_info.twarnning_info.charge_20percent == 0)
        {
            W_BUZZER_START(_BUZZER_WARNING);
            gtmars_info.tbuzzer_task_info.twarnning_info.charge_20percent = 1;
        }
    }
    else
    {
        gtmars_info.tbuzzer_task_info.twarnning_info.charge_20percent = 0;
    }
    /****************************************
     * Smart Key Lost Warnning
     ****************************************/
    if(gtmars_info.tcar_info.tcan_id434_info.toperation1_mode_flag_info.rfid_keyd_detect == 0)
    {
        //W_BUZZER_START(_BUZZER_WARNING);
    }
    /****************************************
     * Seat   Belt Warnning
     ****************************************/
    if((gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_D) ||
       (gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_R))
    {
        if((gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.front_left_buckle_switch_pluged == 0) ||
           (gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.front_right_buckle_switch_pluged == 0) ||
           (gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.second_row_buckle_switch_pluged == 0))
        {
            W_BUZZER_START(_BUZZER_WARNING);
        }
    }
   /****************************************
    * Low Voltage Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id134_info.tfault_code2_info.battery_12v_under_voltage_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * Coolant Temperature Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.over_temperature_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * High Voltage Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.hv_sys_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * Drive System Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.drive_sys_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * Transmission Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.transmission_sys_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * Charge System Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.charging_sys_fault == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * DC - DC Warnning
    ****************************************/
    //TODO : Wait Customer Define the byte 7 
   /****************************************
    * Tire Pressure Warnning
    ****************************************/
    if(gtmars_info.tcar_info.tcan_id340_info.toperation2_mode_flag_info.tpms_flag == 1)
    {
        W_BUZZER_START(_BUZZER_WARNING);
    }
   /****************************************
    * Cruise Control Warnning
    ****************************************/
    if(gtmars_info.tbuzzer_task_info.twarnning_info.criuse_control_warnning != gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.cruising_sys_fault)
    {
        /* Exit Cruise Control Error */
        if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.cruising_sys_fault == 1)
        {
            W_BUZZER_START(_BUZZER_WARNING);
        }
        gtmars_info.tbuzzer_task_info.twarnning_info.criuse_control_warnning = gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.cruising_sys_fault;
    }
   /****************************************
    * Front Truck Open
    ****************************************/
   /****************************************
    * Tailgate Open
    ****************************************/
   /****************************************
    * Doors Open
    ****************************************/
    if((gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_D) ||
       (gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_R))
    {
        if((gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.u8door_close_status_byte & 0x3F) != 0)            
        {
            W_BUZZER_START(_BUZZER_WARNING);
        }
    }
   /****************************************
    * Brake System Warnning
    ****************************************/
   /****************************************
    * Parking Brake Applied
    ****************************************/
    if((gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_D) ||
       (gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_R))
    {
        if((gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.brake_sys_fault == 1) ||
           (gtmars_info.tcar_info.tcan_id134_info.tfault_code2_info.parkin_brake_fault == 1))
        {
            W_BUZZER_START(_BUZZER_WARNING);
        }
    }
}
//*************************************************************************************************************
void W_SYS_ACC_Check(void) 
{
    uint8_t u8LampIO;

    /****************************************
      * ACC I/O Detect
      ****************************************/
    u8LampIO = W_Button_Detect(&gtbutton_ACC,ACC_ON_DEBOUNCE_TIME);
    if(gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status != u8LampIO)
    {
        gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status = u8LampIO;
        W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL4, _EVENT_FIRST);
    }

}
//*************************************************************************************************************
void W_SYS_Check_GPIO(void) 
{
    uint8_t u8LampIO;
    uint8_t u8Test;
    if(gtmars_info.tbutton_task_info.u16timer1 == 0)
    {
        gtmars_info.tbutton_task_info.u16timer1 = 1; // timebase 10ms
        /****************************************
         * CHARGE I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_CHARGE,CHARGE_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.charge_status != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.charge_status = u8LampIO;
        }
        /****************************************
         * IGN I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_IGN,IGN_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.ign_status != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.ign_status = u8LampIO;
        }
        /****************************************
         * Head Lamp Hight I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_HEADLAMP_H,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_h != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_h = u8LampIO;
        }
        /****************************************
         * Head Lamp Low I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_HEADLAMP_L,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_l != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_l = u8LampIO;
        }
        /****************************************
         * Tail Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_TAIL_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.tail_lamp != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.tail_lamp = u8LampIO;
        }
        /****************************************
         * Reverse Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_REVERSE_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.reverse_lamp != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.reverse_lamp = u8LampIO;
        }
        /****************************************
         * Left Turn Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_LEFT_TURN_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp != u8LampIO)
        {
            /* Hight to Low Edge */
            if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp == _HIGHT_LEVEL)
            {
                W_BUZZER_START(_BUZZER_TOCK);
            }
            /* Low to Hight Edge */
            else
            {
                W_BUZZER_START(_BUZZER_TICK);
            }
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = u8LampIO;
        }
        /****************************************
         * Right Turn Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_RIGHT_TURN_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp != u8LampIO)
        {
            /* Hight to Low Edge */
            if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp == _HIGHT_LEVEL)
            {
                W_BUZZER_START(_BUZZER_TOCK);
            }
            /* Low to Hight Edge */
            else
            {
                W_BUZZER_START(_BUZZER_TICK);
            }
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = u8LampIO;
        }
        /****************************************
         * Front Fog Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_FRONT_FOG_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.f_fog_lamp != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.f_fog_lamp = u8LampIO;
        }
        /****************************************
         * Rear Fog Lamp I/O Detect
         ****************************************/
        u8LampIO = W_Button_Detect(&gtbutton_REAR_FOG_LAMP,LAMP_DEBOUNCE_TIME);
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_fog_lamp != u8LampIO)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_fog_lamp = u8LampIO;
        }
    }
}
//*************************************************************************************************************
void W_SYS_DEMO_Turnligth(void)
{
    if(gtmars_info.tdemo_info.u8turn_status == 0)
    {
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp == 1)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = 0;
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = 0;

            W_BUZZER_START(_BUZZER_TICK);
        }
        else
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = 1;
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = 1;
            W_BUZZER_START(_BUZZER_TOCK);
        }
    }
    else if(gtmars_info.tdemo_info.u8turn_status == 1)
    {
        gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = 0;
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp == 1)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = 0;
            W_BUZZER_START(_BUZZER_TICK);
        }
        else
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = 1;
            W_BUZZER_START(_BUZZER_TOCK);
        }
    }
    else if(gtmars_info.tdemo_info.u8turn_status == 2)
    {
        gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp = 0;
        if(gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp == 1)
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = 0;
            W_BUZZER_START(_BUZZER_TICK);
        }
        else
        {
            gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp = 1;
            W_BUZZER_START(_BUZZER_TOCK);
        }
    }

}
//*************************************************************************************************************
void W_SYS_DEMO_Message(void)
{
    static uint8_t u8ticktok_flg;
    static uint8_t u8demo_state;

    gtmars_info.tcar_info.tcan_id53e_info.tsys_consisterncy_check1.ready_to_drive_light_on = 1;
    if(gtmars_info.tdemo_info.u16timer1 == 0)
    {
        gtmars_info.tdemo_info.u16timer1 = 100; // 10ms * 100 = 1s
        W_SYS_DEMO_Turnligth();

        /* Seat Belt Check */
        if(gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.u8cabin_status_byte != 0x07)
        {
            gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.u8cabin_status_byte = 0x07;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.u8cabin_status_byte = 0x00;
        }
        /* High Voltage Check*/
        if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.hv_sys_fault == 0)
        {
            gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.hv_sys_fault = 1;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.hv_sys_fault = 0;
        }
        /* Icon 19 */
        if(gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.front_fog == 0)
        {
            gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.front_fog = 1;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.front_fog = 0;
        }
        /* Icon 13 */
        if(gtmars_info.tcar_info.tcan_id434_info.tlamp_status_info.u8head_lamp_auto == 1)
        {
            gtmars_info.tcar_info.tcan_id434_info.tlamp_status_info.u8head_lamp_auto = 0;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id434_info.tlamp_status_info.u8head_lamp_auto = 1;
        }
    }
    if(gtmars_info.tdemo_info.u16timer2 == 0)
    {
        gtmars_info.tdemo_info.u16timer2 = 10; //10ms * 10 = 100ms
        /* padels */
        if((gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map < 100 )&&
           (gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map == 0))
        {
            gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map += 10;
        }
        else if(gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map < 100)
        {
            gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map = 0;
            gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map += 10;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map = 0;
        }
    }
    if(gtmars_info.tdemo_info.u16timer3 == 0)
    {
        gtmars_info.tdemo_info.u16timer3 = 70; //10ms * 70 = 700ms
        /* speed */
        if (gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed < 40)
        {
            gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed = 40;
        }
        else if(gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed < 250)
        {
            gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed++;
        }
        else
        {
            gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed = 40;
        }
    }
    if(gtmars_info.tdemo_info.u16timer4 <= 10)
    {
        gtmars_info.tdemo_info.u16timer4 = _TIME_5000ms; 
        if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status < 4)
            gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status ++;
        else
            gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status = 0x01;
        if(gtmars_info.tdemo_info.u8turn_status < 2)
            gtmars_info.tdemo_info.u8turn_status ++;
        else
            gtmars_info.tdemo_info.u8turn_status = 0;
            
        if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8driving_porfile == _DRIVE_SPORT)
            gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8driving_porfile = _DRIVE_ECO;
        else
            gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8driving_porfile = _DRIVE_SPORT;
    }
}

extern void W_Copy_CAN_to_MEM(void);
void W_SYS_Main_Entry(void) 
{
    W_Copy_CAN_to_MEM(); //test

    W_SYS_ACC_Check();
    if(gtmars_info.tdemo_info.u8system_mode == _NORMAL_MODE)
    {
        W_SYS_Check_CAN_Message();
        W_SYS_Check_GPIO();
    }
    else
    {
        W_SYS_DEMO_Message();
    }
}

void W_SYS_TIMER_ALL(void)
{
    if(gtmars_info.tdemo_info.u16timer1 > 0)
        gtmars_info.tdemo_info.u16timer1--;
    if(gtmars_info.tdemo_info.u16timer2 > 0)
        gtmars_info.tdemo_info.u16timer2--;
    if(gtmars_info.tdemo_info.u16timer3 > 0)
        gtmars_info.tdemo_info.u16timer3--;
    if(gtmars_info.tdemo_info.u16timer4 > 0)
       gtmars_info.tdemo_info.u16timer4 --; 
}
