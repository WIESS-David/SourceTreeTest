// firmware versioin v1.00.01

#include "r_typedefs.h"

#define ACC_Port                0
#define ACC_Pin                 3

#define CHARGE_Port             0
#define CHARGE_Pin              0

#define IGN_Port                0
#define IGN_Pin                 1

#define ILL_Port                1
#define ILL_Pin                 5

#define SPD_Port                16
#define SPD_Pin                 5

#define DR_Port                 1
#define DR_Pin                  0

#define HEAD_LAMP_H_Port       1
#define HEAD_LAMP_H_Pin        4

#define HEAD_LAMP_L_Port       1
#define HEAD_LAMP_L_Pin        5

#define TAIL_LAMP_Port         1
#define TAIL_LAMP_Pin          0

#define REVERSE_LAMP_Port      3
#define REVERSE_LAMP_Pin       0

#define LEFT_TURN_LAMP_Port    1
#define LEFT_TURN_LAMP_Pin     3

#define RIGHT_TURN_LAMP_Port   3
#define RIGHT_TURN_LAMP_Pin    2

#define FRONT_FOG_LAMP_Port    1
#define FRONT_FOG_LAMP_Pin     1

#define REAR_FOG_LAMP_Port     3
#define REAR_FOG_LAMP_Pin      1

#define DEMO_Port              3
#define DEMO_Pin               3

#define _HIGHT_LEVEL             1
#define _LOW_LEVEL               0

#define ACC_ON_DEBOUNCE_TIME    15
#define ACC_OFF_DEBOUNCE_TIME   30
#define ILL_DEBOUNCE_TIME       ACC_ON_DEBOUNCE_TIME
#define SPD_DEBOUNCE_TIME       15
#define DR_DEBOUNCE_TIME        15
#define LAMP_DEBOUNCE_TIME      2
#define CHARGE_DEBOUNCE_TIME    10
#define IGN_DEBOUNCE_TIME       10

typedef struct
{
    uint32_t u32port;
    uint8_t u8pin;
    uint16_t u16low_level_counter;
    uint16_t u16high_level_counter;
    uint8_t u8level_status;
    uint8_t u8button_activity;
}w_dev_button_t;


void W_Button_Parameter_Init(w_dev_button_t *ptbutton ,uint32_t u32port, uint8_t u8pin, uint8_t u8level);
void W_Button_Init(void);
uint8_t W_Button_Detect(w_dev_button_t* ptbutton, uint16_t u16debounce_max);
void W_Button_AllTimer(void);
void W_Button_Timer1(void);
void W_Get_Start_Procedure(void);

