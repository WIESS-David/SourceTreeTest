//============================================================================================================
//
// Header file for the system flag definitions
//
//============================================================================================================
#ifndef _W_SYSTEM_H_
#define _W_SYSTEM_H_

#include "r_typedefs.h"



// Nios Task State Numver Define
#define _MAX_RSCAN_ST_NO            (uint8_t)0x03
#define _MAX_CLUSTER_ST_NO          (uint8_t)0x04
#define _MAX_ADCE_ST_NO             (uint8_t)0x03
#define _MAX_BUTTON_ST_NO           (uint8_t)0x03
#define _MAX_BUZZER_ST_NO           (uint8_t)0x03

// Nios Task ID Define
#define _TYPE_RSCAN                 (uint8_t)0xA0
#define _TYPE_TOUCH                 (uint8_t)0xA1
#define _TYPE_CLUSTER               (uint8_t)0xB0
#define _TYPE_ADCE                  (uint8_t)0xB1
#define _TYPE_BUTTON                (uint8_t)0xB2
#define _TYPE_BUZZER                (uint8_t)0xB3

// Nios Timer Paremeters Define
#define _TIME_DISABLE               0
#define _TIME_10ms                  1
#define _TIME_20ms                  2
#define _TIME_50ms                  5
#define _TIME_100ms                 10
#define _TIME_110ms                 11
#define _TIME_120ms                 12
#define _TIME_130ms                 13
#define _TIME_140ms                 14
#define _TIME_150ms                 15
#define _TIME_160ms                 16
#define _TIME_170ms                 17
#define _TIME_200ms                 20
#define _TIME_250ms                 25
#define _TIME_300ms                 30
#define _TIME_500ms                 50
#define _TIME_510ms                 51
#define _TIME_610ms                 61
#define _TIME_1000ms                100
#define _TIME_3000ms                300
#define _TIME_5000ms                500

// Filter Parameters Define
#define _FILTER_SIZE                4

// System Mode Define
#define _DEMO_MODE                   0
#define _NORMAL_MODE                 1

typedef struct
{
    uint16_t u16timer1;
    uint16_t u16timer2;
    uint8_t u8acc_on_check_over;
}cluster_info_def;


typedef enum
{
    eTouch_Lift,
    eTouch_Down,
    eTouch_None
}finger_action;

typedef struct
{
    uint16_t u16finger_id;
    finger_action efinger_action;
    uint16_t u16x_position;
    uint16_t u16y_position;
}touch_position_info_def;

typedef union
{
    uint8_t u8vehicle_info_byte;
    struct
    {
        uint8_t u8key_acc :1;		
        uint8_t u8key_on :1;
        uint8_t u8key_start :1;
        uint8_t u8ac_plugin :1;
        uint8_t u8dc_plugin :1;
        uint8_t u8remote_access :1;
        uint8_t :1;
        uint8_t :1;
    };
}tvehicle_info_def;

typedef union
{
    uint8_t u8shifter_mode_byte;
    struct
    {
        uint8_t u8Shifter_Status :3;		
        uint8_t :1;
        uint8_t u8driving_porfile :3;
        uint8_t :1;
    };
}tshifter_mode_def;

typedef union
{
    uint8_t u8fault_code2_byte;
    struct
    {
        uint8_t battery_12v_under_voltage_fault :1;
        uint8_t parkin_brake_fault :1;
        uint8_t  :6;
    };
} fault_code2_def;

typedef union
{
    uint8_t u8fault_code1_byte;
    struct
    {
        uint8_t drive_sys_fault:1;
        uint8_t cruising_sys_fault:1;
        uint8_t hv_sys_fault:1;
        uint8_t converter_sys_fault:1;
        uint8_t brake_sys_fault:1;
        uint8_t transmission_sys_fault:1;
        uint8_t over_temperature_fault:1;
        uint8_t charging_sys_fault:1;
    };
}fault_code1_def;

typedef union
{
    uint8_t u8sys_consistency_check1_byte;
    struct
    {
        uint8_t ready_to_drive_light_on :1;
        uint8_t parking_brake_light_on :1;
        uint8_t :5;
        uint8_t life_signal :1;
    };
}sys_consistency_check1_def;

typedef union
{
    uint8_t u8sys_consistency_check3_byte;
    struct
    {
        uint8_t low_soc_light_on :1;
        uint8_t :6;
        uint8_t life_signal :1;
    };
}sys_consistency_check3_def;

typedef union
{
    uint8_t u8door_close_status_byte;
    struct
    {
        uint8_t front_right :1;
        uint8_t front_left :1;
        uint8_t rear_right :1;
        uint8_t rear_left :1;
        uint8_t rear_Tailgate :1;
        uint8_t hood_switch :1;
        uint8_t charger_lid :1;
        uint8_t outer_mirror_fold :1;
    };
}tdoor_close_status_def;

typedef union
{
    uint8_t u8lamp_status_byte;
    struct
    {
        uint8_t u8drl :1;
        uint8_t u8stop_lamp :1;
        uint8_t u8welcome_lamp :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t u8head_lamp_auto:1;
    };
}tlamp_status_def;

typedef union
{
    uint8_t u8cabin_status_byte;
    struct
    {
        uint8_t front_right_buckle_switch_pluged :1;
        uint8_t front_left_buckle_switch_pluged :1;
        uint8_t second_row_buckle_switch_pluged :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t rear_defogger_on :1;
    };
}tcabin_status_def;

typedef union
{
    uint8_t u8wiper_status_byte;
    struct
    {
        uint8_t wiper_low_speed_on :1;
        uint8_t wiper_high_speed_on :1;
        uint8_t wiper_intermittent :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t screen_washing_on :1;
        uint8_t wiper_auto_on:1;
    };
}twiper_status_def;

typedef union
{
    uint8_t u8security_status_byte;
    struct
    {
        uint8_t central_door_lock_unlock :1;
        uint8_t passenger_power_window_lock_unlock :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t avas_on :1;
        uint8_t valet_mode:1;
        uint8_t arm_dis_Arm_mode:1;
    };
}tsecurity_status_def;

typedef union
{
    uint8_t u8operation1_mode_flag_byte;
    struct
    {
        uint8_t head_lamp_malfunction_flag :1;
        uint8_t wiper_malfunction_flag :1;
        uint8_t security_sys_malfunction_flag :1;
        uint8_t shock_alarmed :1;
        uint8_t theift_alarmed :1;
        uint8_t rfid_keyd_detect :1;
        uint8_t remote_access_control_relay_on :1;
        uint8_t life_signal :1;
    };
}operation1_mode_flag_def;

typedef union
{
    uint8_t u8havc_status_byte;
    struct
    {
        uint8_t power_on_off :1;
        uint8_t external_internal_circulation :1;
        uint8_t auto_mode_on_off :1;
        uint8_t hv_compressor_on_off :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
    };
}hvac_status_def;

typedef union
{
    uint8_t u8powertrain_status_byte;
    struct
    {
        uint8_t cruise_driving:2;
        uint8_t :6;
    };
}powertrain_status_def;

typedef union
{
    uint8_t u8operation2_mode_flag_byte;
    struct 
    {
        uint8_t tpms_flag :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t life_signal :1;
    };
}operation2_mode_flag_def;

typedef union
{
    uint8_t u8opertion3_mode_flag_byte;
    struct
    {
        uint8_t languages_setup :3;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t life_signal :1;
    };
}opertion3_mode_flag_def;

typedef struct
{
    uint8_t u8vehicle_speed;
    uint8_t u8cruise_speed;
    uint8_t u8accel_Pedal_map;
    uint8_t u8regeneration_map;
    uint16_t u16power_usage_power;
    fault_code1_def tfault_code1_info;
    uint8_t u8main_checksum1;
} tcan_id133_def;

typedef struct
{
    tvehicle_info_def tvehicle_info;
    tshifter_mode_def tshifter_and_mode_info;
    powertrain_status_def tpowertrain_status_info;
    uint8_t u8coolant_temp_1;
    uint8_t u8coolant_temp_2;
    uint8_t u8coolant_temp_3;
    fault_code2_def tfault_code2_info;
    uint8_t u8main_checksum2;
} tcan_id134_def;

typedef struct
{
    uint16_t u16remained_distance;
    uint16_t u16total_distance;
    uint32_t u32mileague;
    sys_consistency_check1_def tsys_consisterncy_check1;
} tcan_id53e_def;

typedef struct
{
    uint16_t u16battery_remain_capacity;
    uint16_t u16battery_design_capacity;
    uint8_t u8hv_battery_soc;
    uint8_t u8hv_battery_soh;
    sys_consistency_check3_def tsys_consistency_check3_info;
} tcan_id53f_def;

typedef struct
{
    tdoor_close_status_def tdoor_status_info;
    tlamp_status_def tlamp_status_info;
    tcabin_status_def tcabin_status_info;
    twiper_status_def twiper_status_info;
    tsecurity_status_def tsecurity_status_info;
    operation1_mode_flag_def toperation1_mode_flag_info;
} tcan_id434_def;

typedef struct
{
    hvac_status_def thvac_status_info;
    uint8_t u8temp_inside;
    uint8_t u8temp_outside;
    operation2_mode_flag_def toperation2_mode_flag_info;
} tcan_id340_def;

typedef struct
{
    uint32_t u32time_in_seconds;
    uint8_t u8date;
    uint8_t u8Month;
    uint16_t u16Year;
    opertion3_mode_flag_def topertion3_mode_flag_info;
} tcan_id56b_def;

typedef union
{
    uint8_t u8car_power_status_byte;
    struct
    {
        uint8_t charge_status :1;
        uint8_t ign_status :1;
        uint8_t acc_status :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
    };
}car_power_def;

typedef union
{
    uint8_t u8car_light_byte;
    struct
    {
        uint8_t reverse_lamp :1;
        uint8_t tail_lamp :1;
        uint8_t head_lamp_h :1;
        uint8_t head_lamp_l :1;
        uint8_t l_turn_lamp :1;
        uint8_t r_turn_lamp :1;
        uint8_t f_fog_lamp :1;
        uint8_t r_fog_lamp :1;
        
    };
} car_light_def;

typedef struct
{
    car_power_def tcar_power_info;
    car_light_def tcar_light_info; 
}car_power_light_def;

typedef union
{
    uint8_t u8lamp_from_can_byte;
    struct
    {
        uint8_t head_lamp_high :1;
        uint8_t head_lamp_low :1;
        uint8_t tail_lamp :1;
        uint8_t reverse_lamp :1;
        uint8_t front_fog :1;
        uint8_t rear_fog :1;
        uint8_t left_turn :1;
        uint8_t right_turn :1;
    };
} lamp_form_can_def;

typedef struct
{
    lamp_form_can_def tlamp_form_can_info;
    uint8_t u8menu_key_status;
}tcan_id287_def;

typedef struct
{    
    tcan_id133_def tcan_id133_info;
    tcan_id134_def tcan_id134_info;
    tcan_id53e_def tcan_id53e_info;
    tcan_id53f_def tcan_id53f_info;
    tcan_id434_def tcan_id434_info;
    tcan_id340_def tcan_id340_info;
    tcan_id56b_def tcan_id56b_info;
    tcan_id287_def tcan_id287_info;
    car_power_light_def tcar_power_light_info;
    uint16_t u16timer1;
    uint16_t u16timer2;
    uint16_t u16timer3;
    uint16_t u16timer4;
} car_info_def;

typedef struct
{
    uint16_t u16timer1;
    uint16_t u16timer2;
    uint16_t u16timer3;
    uint16_t u16timer4;
} adc_info_def;

typedef struct
{
    uint8_t u8checkdata[10];
    uint8_t u8enter_checkcnt;
    uint8_t u8down_checkcnt;
    uint8_t u8up_checkcnt;
    uint8_t u8menu_checkcnt;
    uint8_t u8menu_display_flag;
    uint8_t u8button_status;
    uint16_t u16menutimer;
    uint16_t u16timer1;
} tbutton_data_def;

typedef union
{
    uint8_t u8warnning_flag_byte;
    struct
    {
        uint8_t charge_10percent:1;
        uint8_t charge_20percent:1;
        uint8_t criuse_control_warnning:1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
    };
} warnning_info_def;

typedef union
{
    uint8_t u8status_flag_byte;
    struct
    {
        uint8_t start_bit:1;
        uint8_t delay_bit:1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
        uint8_t :1;
    };
} sys_flag_def;

typedef struct
{
    uint16_t u16timer1;
    uint16_t u16buzzer_id;
    uint16_t u16status;
    uint16_t *u16_sts_ptr;
    uint8_t u8table_index;
    sys_flag_def  tsys_flag;
    warnning_info_def twarnning_info;
} tbuzzer_info_def;

typedef struct
{
    uint16_t u16timer1;
    uint16_t u16timer2;
    uint16_t u16timer3;
    uint16_t u16timer4;
    uint8_t u8turn_status;
    uint8_t u8system_mode;
} tdemo_info_def;

typedef struct
{
    cluster_info_def tcluster_info;
    car_info_def tcar_info;
    adc_info_def tadc_task_info;
    tbutton_data_def tbutton_task_info;
    tbuzzer_info_def tbuzzer_task_info;
    tdemo_info_def tdemo_info;
} mars_info_def;

void W_SYS_Check_CAN_Message(void);
void W_SYS_DEMO_Message(void);
void W_SYS_Check_GPIO(void);
void W_SYS_Main_Entry(void);
void W_SYS_TIMER_ALL(void);
void W_SYS_Reset(void);


#endif
