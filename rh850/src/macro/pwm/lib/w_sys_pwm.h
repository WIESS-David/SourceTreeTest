
#include "dr7f701461.dvf.h"
#include "r_typedefs.h"

#define PWBA_UNIT0      0
#define PWBA_UNIT1      1
#define PWBA_UNIT2      2
#define PWBA_UINT3      3

#define PWGA_CH0        0
#define PWGA_CH1        1
#define PWGA_CH2        2
#define PWGA_CH3        3
#define PWGA_CH4        4
#define PWGA_CH5        5
#define PWGA_CH6        6
#define PWGA_CH7        7
#define PWGA_CH8        8
#define PWGA_CH9        9
#define PWGA_CH10       10
#define PWGA_CH11       11
#define PWGA_CH12       12
#define PWGA_CH13       13
#define PWGA_CH14       14
#define PWGA_CH15       15
#define PWGA_CH16       16
#define PWGA_CH17       17
#define PWGA_CH18       18
#define PWGA_CH19       19
#define PWGA_CH20       20
#define PWGA_CH21       21
#define PWGA_CH22       22
#define PWGA_CH23       23

#define BUZZER_TONE1    1
#define BUZZER_TONE2    2
#define BUZZER_OFF      0

uint8_t W_PWM_GetClockOperatingStatus(__type5073 * PWBAn,uint8_t u8unit);
uint16_t W_PWM_GetCycleCount(__type5072 *PWGAn);
uint16_t W_PWM_Get_CSDR_Buffer(__type5072 *PWGAn);
uint16_t W_PWM_Get_CRDR_Buffer(__type5072 *PWGAn);
uint16_t W_PWM_Get_CTDR_Buffer(__type5072 *PWGAn);
uint8_t W_PWM_GetBufferReloadStatus(__type5072 *PWGAn);
uint8_t W_PWM_Get_PWSA_Status(__type5074 *PWSAn);
uint8_t W_PWM_Get_PWGA_TriggerChannelNumber(__type5074 *PWSAn, uint8_t u8queue);

void W_PWM_SetClock( __type5073 * PWBAn, uint8_t u8unit, uint16_t u16multiplier);
void W_PWM_SetClockEnable(__type5073 *PWBAn, uint8_t u8unit);
void W_PWM_SetClockDisable(__type5073 *PWBAn, uint8_t u8unit);
void W_PWM_SetClockOutputInDebug(__type5073 *PWBAn, uint8_t u8enable);
void W_PWM_SetCountClockSource(__type5072 *PWGAn, uint8_t u8source);
void W_PWM_SetOutputSetCondition(__type5072 *PWGAn, uint16 u16value);
void W_PWM_SetOutputResetCondition(__type5072 *PWGAn, uint16 u16value);
void W_PWM_SetGenerationCondition(__type5072 *PWGAn, uint16 u16value);
void W_PWM_Set_PWGA_Trigger(__type5039* SELBn, uint32_t u32ch);
void W_PWM_Set_PWGA_Trigger_Clear(__type5039* SELBn, uint32_t u32ch);
void W_PWM_SetBufferReloadTrigger(__type5072 *PWGAn);
void W_PWM_Set_PWSA_Control(__type5074 *PWSAn, uint8_t u8enable);
void W_PWM_SetClockSourceFormPWBA(__type5072 *PWGAn, uint8_t u8source);
void W_PWMBKL_Init(void);
void W_PWM_Set_PWSA_Status_Clear(__type5074 *PWSAn, uint8_t u8clear);
void W_PWM_SetDuty(uint8_t u8duty,uint8_t chanel);

void W_PWMBUZZER_Init(void);
void W_BUZZER_SetTone(uint8_t u8tone);
