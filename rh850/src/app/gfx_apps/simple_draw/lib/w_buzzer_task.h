//============================================================================================================
//
// Header file for the BUZZER table rule
//
//============================================================================================================
#ifndef _WIESS_BUZZER_TASK_H_
#define _WIESS_BUZZER_TASK_H_

#define _STATE_DECODE             0x0000
#define _STATE_END                0xFFFF

#define _BUZZER_WARNING            0
#define _BUZZER_TICK               1
#define _BUZZER_TOCK               2

//************************************************************************************************************
//  Function define
//************************************************************************************************************
void W_BUZZER_Init(void);
void W_BUZZER_Process(void);
void W_BUZZER_Timer1(void);
void W_BUZZER_AllTimer(void);
void W_BUZZER_GET_TABLE_ADDR(uint8_t table_index);
void W_BUZZER_START(uint8_t table_index);

#endif

