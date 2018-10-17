//============================================================================================================
//
// Header file for the CAN table rule
//
//============================================================================================================
#ifndef _WIESS_BUTTON_TASK_H_
#define _WIESS_BUTTON_TASK_H_

// Nios Button State Machine Define
#define _STATE_BUTTON_INIT        0x00
#define _STATE_BUTTON_PROCESS     0x01
#define _STATE_BUTTON_ERROR       0x02

#define _BUTTON_ADC_RANGE      100
#define _BUTTON_ENTER_AD_MAX   3351 + _BUTTON_ADC_RANGE
#define _BUTTON_ENTER_AD_MIN   3351 - _BUTTON_ADC_RANGE
#define _BUTTON_DOWN_AD_MAX    2606 + _BUTTON_ADC_RANGE
#define _BUTTON_DOWN_AD_MIN    2606 - _BUTTON_ADC_RANGE
#define _BUTTON_UP_AD_MAX      1861 + _BUTTON_ADC_RANGE
#define _BUTTON_UP_AD_MIN      1861 - _BUTTON_ADC_RANGE
#define _BUTTON_MENU_AD_MAX    1117 + _BUTTON_ADC_RANGE
#define _BUTTON_MENU_AD_MIN    1117 - _BUTTON_ADC_RANGE
#define _BUTTON_RELEASE_VALUE  4000

enum
{
    Button_Release  = 0,
    Button_Enter    = 1,
    Button_Down     = 2,
    Button_Up       = 3,
    Button_Menu     = 4,
    Button_Menu_Long= 5,
};


//************************************************************************************************************
//  Function define
//************************************************************************************************************
//void W_Button_Init(void);
//void W_Button_Process(void);
//void W_Button_Error(void);
//void W_Button_Timer1(void);
//void W_Button_Timer2(void);
//void W_Button_AllTimer(void);
//void W_Button_Detect(void);


#endif


