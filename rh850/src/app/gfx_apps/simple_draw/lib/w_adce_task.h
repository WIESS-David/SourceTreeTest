//============================================================================================================
//
// Header file for the CAN table rule
//
//============================================================================================================
#ifndef _WIESS_ADCE_TASK_H_
#define _WIESS_ADCE_TASK_H_

// Nios ADCE State Machine Define
#define _STATE_ADCE_INIT        0x00
#define _STATE_ADCE_PROCESS     0x01
#define _STATE_ADCE_ERROR       0x02

#define _ADCE_KEY_CHANNEL       12
#define _ADCE_LCD_TEMP_CHANNEL  13
#define _ADCE_SYS_TEMP_CHANNEL  14


//************************************************************************************************************
//  Function define
//************************************************************************************************************
void W_ADCE_Init(void);
void W_ADCE_Process(void);
void W_ADCE_Error(void);
void W_ADCE_Timer1(void);
void W_ADCE_Timer2(void);
void W_ADCE_Timer3(void);
void W_ADCE_Timer4(void);
void W_ADCE_AllTimer(void);


#endif

