#include "dr7f701461.dvf.h"
#include "r_typedefs.h"

#define STBC_DISABLE        0
#define STBC_ENABLE         1

#define WAKE_UP_CH0         0
#define WAKE_UP_CH1         1
#define WAKE_UP_CH2         2
#define WAKE_UP_CH3         3
#define WAKE_UP_CH4         4
#define WAKE_UP_CH5         5
#define WAKE_UP_CH6         6
#define WAKE_UP_CH7         7
#define WAKE_UP_CH8         8
#define WAKE_UP_CH9         9
#define WAKE_UP_CH10        10
#define WAKE_UP_CH11        11
#define WAKE_UP_CH12        12
#define WAKE_UP_CH13        13
#define WAKE_UP_CH14        14
#define WAKE_UP_CH15        15
#define WAKE_UP_CH16        16
#define WAKE_UP_CH17        17
#define WAKE_UP_CH18        18


#define IOHOLD_RELEASE      0
#define IOHOLD_ACTIVATE     1

#define IOHOLD6_P16_P17     6
#define IOHOLD5_P42         5
#define IOHOLD4             4       // P43_7 ~ P43_12
#define IOHOLD3             3       // P43_0, P43_1, P44_0 ~ P44_11, P45_0 ~ P45_13
#define IOHOLD2_P3          2
#define IOHOLD1_P1_P2       1
#define IOHOLD0_P0          0

void W_STBC_TiggerDeepStop(void);
void W_STBC_SetWakeUp(uint32_t u32event_channel,uint8_t u8enable);
void W_STBC_SetWakeUpMaskVaule( uint32_t u32value);
void W_STBC_ClearWakeUpEvent(uint32_t u32event_channel);
void W_STBC_ClearWakeUpEventValue(uint32_t u32value);
void W_STBC_SetIOHOLD(uint32_t u32io_hold_ch, uint8_t u8activate);
void W_STBC_SetIOHOLDValue(uint32_t u32value);
uint32_t  W_STBC_GetIOHOLDValue(void);
uint32_t W_STBC_GetWakeUpMaskValue(void);
uint32_t W_STBC_GetWakeUpEventVaule(void);
void W_STBC_GoToDeepStop(void);

