//============================================================================================================
//
// Header file for the CAN table rule
//
//============================================================================================================
#ifndef _WIESS_TOUCH_TASK_H_
#define _WIESS_TOUCH_TASK_H_

#include "r_riic_main.h"

#define _RIIC_UNIT                                  0x01
#define _RIIC_CLOCK_FREQ                            400000

#define _TOUCH_ADDRESS                              0x2A

#define _RIIC_WRITE                                 0
#define _RIIC_READ                                  1

#define _TOUCH_DATA_NUM                             66

#define _MAX_LOW_DEBOUNCE                           0
#define _MAX_HIGH_DEBOUNCE                          1


// Nios Touch State Machine Define
#define _STATE_TOUCH_INIT                           0x00
#define _STATE_TOUCH_WAITING                        0x01
#define _STATE_TOUCH_ERROR                          0x02

#define _FINGER_X_RANGE                             20
#define _FINGER_Y_RANGE                             20


#define _MODE_ICON_X_RANGE_MAX                      360
#define _MODE_ICON_X_RANGE_MIN                      100

#define _MODE_ICON_Y_RANGE_MAX                      3800
#define _MODE_ICON_Y_RANGE_MIN                      3300
//************************************************************************************************************
//  Function define
//************************************************************************************************************
void W_Touch_RIIC_GPIO_Init(void);
void W_Touch_EINT_GPIO_Init(void);
r_riic_Error_t W_Touch_RIIC_Init(void);
void W_Touch_Init(void);
void W_Touch_Waiting(void);
void W_Touch_Error(void);
void W_Touch_Timer1(void);
void W_Touch_Timer2(void);
void W_Touch_AllTimer(void);

#endif

