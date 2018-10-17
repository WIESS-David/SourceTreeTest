//============================================================================================================
//
// Header file for the CAN table rule
//
//============================================================================================================
#ifndef _WIESS_RSCAN_TASK_H_
#define _WIESS_RSCAN_TASK_H_

#include "w_sys_rscan.h"

#define protected_write(preg,pstatus,reg,value)     do{\
                                                    (preg)=0xa5u;\
                                                    (reg)=(value);\
                                                    (reg)=~(value);\
                                                    (reg)=(value);\
                                                    }while((pstatus)==1u)

// Nios OBDII State Machine Define
#define _STATE_RSCAN_INIT                           0x00
#define _STATE_RSCAN_TIMING                         0x01
#define _STATE_RSCAN_ERROR                          0x02

//************************************************************************************************************
//  Function define
//************************************************************************************************************
void W_RSCAN_Task_Init(void);
void W_RSCAN_Timimg(void);
void W_RSCAN_Error(void);
void W_RSCAN_Timer1(void);
void W_RSCAN_Timer2(void);
void W_RSCAN_Timer3(void);
void W_RSCAN_Timer4(void);
void W_RSCAN_AllTimer(void);
void W_RSCAN_ChangeState(uint8_t u8canstate);

void W_RSCAN_Read_FIFO(void);
void W_RSCAN_Data_Process(void);
#endif
