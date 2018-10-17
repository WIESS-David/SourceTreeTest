#include "w_sys_pwm.h"
#include "r_dev_api.h"
#include "r_gpio_api.h"



static const r_dev_PinConfig_t tpwm_pinconfig2[] = 
{
    /* PWGA16 - PWM pins */
    {16, 2, 2, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  PWGA18 Output */ 
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};
//*************************************************************************************************************
static const r_dev_PinConfig_t tpwm_pinconfig[] = 
{
    /* PWGA16 - PWM pins */
    {16, 0, 2, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  PWGA16 Output */ 
    {16, 1, 2, R_DEV_PIN_OUT   , 0u, R_DEV_PIN_PULLUP, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* O:  PWGA17 Output */ 
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};
//*************************************************************************************************************
void W_PWM_SetClock(__type5073 * PWBAn, uint8_t u8unit, uint16_t u16multiplier)
{
    switch(u8unit)
    {
        default:
        case 0:
            PWBAn->BRS0.UINT16 = u16multiplier;
            break;
        case 1:
            PWBAn->BRS1.UINT16 = u16multiplier;
            break;
        case 2:
            PWBAn->BRS2.UINT16 = u16multiplier;
            break;
        case 3:
            PWBAn->BRS3.UINT16 = u16multiplier;
            break;
    }
}
//*************************************************************************************************************
uint8_t W_PWM_GetClockOperatingStatus(__type5073 * PWBAn, uint8_t u8unit)
{
    switch(u8unit)
    {
        default:
        case 0:
            return PWBAn->TE.BIT.TE0;
        case 1:
            return PWBAn->TE.BIT.TE1;
        case 2:
            return PWBAn->TE.BIT.TE2;
        case 3:
            return PWBAn->TE.BIT.TE3;
    }
}
//*************************************************************************************************************
void W_PWM_SetClockEnable(__type5073 *PWBAn, uint8_t u8unit)
{
    switch(u8unit)
    {
        default:
        case 0:
            PWBAn->TS.BIT.TS0 = 1;
            break;
        case 1:
            PWBAn->TS.BIT.TS1 = 1;
            break;
        case 2:
            PWBAn->TS.BIT.TS2 = 1;
            break;
        case 3:
            PWBAn->TS.BIT.TS3 = 1;
            break;
    }
}
//*************************************************************************************************************
void W_PWM_SetClockDisable(__type5073 *PWBAn, uint8_t u8unit)
{
    switch(u8unit)
    {
        default:
        case 0:
            PWBAn->TT.BIT.TT0 = 0;
            break;
        case 1:
            PWBAn->TT.BIT.TT1 = 0;
            break;
        case 2:
            PWBAn->TT.BIT.TT2 = 0;
            break;
        case 3:
            PWBAn->TT.BIT.TT3 = 0;
            break;
    }
}
//*************************************************************************************************************
void W_PWM_SetClockOutputInDebug(__type5073 *PWBAn, uint8_t u8enable)
{
    if(u8enable == R_TRUE)
        PWBAn->EMU.BIT.SVSDIS = 1;
    else
        PWBAn->EMU.BIT.SVSDIS = 0;
}
//*************************************************************************************************************

void W_PWM_SetClockSourceFormPWBA(__type5072 *PWGAn, uint8_t u8source)
{
    PWGAn->CTL.UINT8 = u8source & 0x03;
}
//*************************************************************************************************************
uint16_t W_PWM_GetCycleCount(__type5072 *PWGAn)
{
    return PWGAn->CNT.UINT16;
}
//*************************************************************************************************************
void W_PWM_SetOutputSetCondition(__type5072 *PWGAn, uint16 u16value)
{
    PWGAn->CSDR.UINT16 = u16value & 0x0FFF;
}
//*************************************************************************************************************
void W_PWM_SetOutputResetCondition(__type5072 *PWGAn, uint16 u16value)
{
    PWGAn->CRDR.UINT16 = u16value & 0x0FFF;
}
//*************************************************************************************************************
void W_PWM_SetGenerationCondition(__type5072 *PWGAn, uint16 u16value)
{
    PWGAn->CTDR.UINT16= u16value & 0x0FFF;
}
//*************************************************************************************************************
uint16_t W_PWM_Get_CSDR_Buffer(__type5072 *PWGAn)
{
    return PWGAn->CSBR.UINT16;
}
//*************************************************************************************************************
uint16_t W_PWM_Get_CRDR_Buffer(__type5072 *PWGAn)
{
    return PWGAn->CRBR.UINT16;
}
//*************************************************************************************************************
uint16_t W_PWM_Get_CTDR_Buffer(__type5072 *PWGAn)
{
    return PWGAn->CTBR.UINT16;
}
//*************************************************************************************************************
uint8_t W_PWM_GetBufferReloadStatus(__type5072 *PWGAn)
{
    return PWGAn->RSF.UINT8;
}
//*************************************************************************************************************
void W_PWM_SetBufferReloadTrigger(__type5072 *PWGAn)
{
    PWGAn->RDT.UINT8 = 1;
}
//*************************************************************************************************************
void W_PWM_Set_PWGA_Trigger(__type5039* SELBn, uint32_t u32ch)
{
    SELBn->SLPWGA0.UINT32 |= (uint32_t)(1 << u32ch);
}
//*************************************************************************************************************
void W_PWM_Set_PWGA_Trigger_Clear(__type5039* SELBn, uint32_t u32ch)
{
    uint32_t u32temp = ~((uint32_t)(1 << u32ch));
    SELBn->SLPWGA0.UINT32 &= u32temp;
}
//*************************************************************************************************************
void W_PWM_Set_PWSA_Control(__type5074 *PWSAn, uint8_t u8enable)
{
    if(u8enable == R_TRUE)
        PWSAn->CTL.UINT8 = 1;
    else
        PWSAn->CTL.UINT8 = 0;
}
//*************************************************************************************************************
uint8_t W_PWM_Get_PWSA_Status(__type5074 *PWSAn)
{
    return PWSAn->STR.UINT8;
}
//*************************************************************************************************************
void W_PWM_Set_PWSA_Status_Clear(__type5074 *PWSAn, uint8_t u8clear)
{
    PWSAn->STC.UINT8 = u8clear & 0x03;
}
//*************************************************************************************************************
uint8_t W_PWM_Get_PWGA_TriggerChannelNumber(__type5074 *PWSAn, uint8_t u8queue)
{
    switch(u8queue)
    {
        default:
        case 0:
            return PWSAn->QUE0.UINT8;
        case 1:
            return PWSAn->QUE1.UINT8;
        case 2:
            return PWSAn->QUE2.UINT8;
        case 3:
            return PWSAn->QUE3.UINT8;
        case 4:
            return PWSAn->QUE4.UINT8;
        case 5:
            return PWSAn->QUE5.UINT8;
        case 6:
            return PWSAn->QUE6.UINT8;
        case 7:
            return PWSAn->QUE7.UINT8;
    }
}
//*************************************************************************************************************
void W_PWM_SetDuty(uint8_t u8duty,uint8_t channel)
{
    uint16_t u16high_duty;
    
    if(u8duty > 100)
        u8duty = 100;
     
    u16high_duty = (u8duty * 4095) / 100;
   
    W_PWM_Set_PWGA_Trigger_Clear(SELB, channel);          // Clear trigger channel .
    W_PWM_SetClockDisable(PWBA0, PWBA_UNIT0);
    W_PWM_Set_PWSA_Control(PWSA0, R_FALSE);                 // Enable PWSA operation.
    if( channel == 16)
    {
        while(W_PWM_GetBufferReloadStatus(PWGA16));
        W_PWM_SetOutputSetCondition(PWGA16, 0);                 // Set counter vaule of output high level 
        W_PWM_SetOutputResetCondition(PWGA16, u16high_duty);    // Set counter vaule of output low level.
        W_PWM_SetBufferReloadTrigger(PWGA16);                   // Trigger data reload
    }
    else if( channel == 17)
    {
        while(W_PWM_GetBufferReloadStatus(PWGA17));
        W_PWM_SetOutputSetCondition(PWGA17, 0);                 // Set counter vaule of output high level 
        W_PWM_SetOutputResetCondition(PWGA17, u16high_duty);    // Set counter vaule of output low level.
        W_PWM_SetBufferReloadTrigger(PWGA17);                   // Trigger data reload
    }
    W_PWM_Set_PWSA_Control(PWSA0, R_TRUE);                  // Enable PWSA operation.
    W_PWM_SetClockEnable(PWBA0, PWBA_UNIT0);                // Enable PWBA UNIT0 clock.
    W_PWM_Set_PWGA_Trigger(SELB, channel);                // Set trigger channel i
}

//*************************************************************************************************************
void W_PWMBKL_Init(void)
{
    W_PWM_SetClock(PWBA0, PWBA_UNIT0, 1);                   // Set PWBA clock freq = PCLK(freq) /(2 * 1) = 20/2 = 10KHz. for PWMCLOCK0
    R_DEV_PinInit(tpwm_pinconfig);
    W_PWM_SetClockSourceFormPWBA(PWGA16, PWBA_UNIT0);       // Select PWGA16 clock source from UNIT0.
    W_PWM_SetClockSourceFormPWBA(PWGA17, PWBA_UNIT0);       // Select PWGA17 clock source from UNIT0.
}

//*************************************************************************************************************
void W_BUZZER_SetTone(uint8_t u8tone)
{
    W_PWM_Set_PWGA_Trigger_Clear(SELB, 18);                     // Clear trigger channel .
    W_PWM_SetClockDisable(PWBA0, PWBA_UNIT1);
    W_PWM_SetClockDisable(PWBA0, PWBA_UNIT2);
    if(u8tone == BUZZER_TONE1)
    {
        W_PWM_SetClockSourceFormPWBA(PWGA18, PWBA_UNIT1);
        W_PWM_SetOutputSetCondition(PWGA18, 0);                 // Set counter vaule of output high level 
        W_PWM_SetOutputResetCondition(PWGA18, 2048);            // Set counter vaule of output low level.
        W_PWM_SetBufferReloadTrigger(PWGA18);                   // Trigger data reload
        W_PWM_SetClockEnable(PWBA0, PWBA_UNIT1);                // Enable PWBA UNIT1 clock.
        W_PWM_Set_PWGA_Trigger(SELB, 18);                       // Set trigger channel i
    }
    else if(u8tone == BUZZER_TONE2)
    {
        W_PWM_SetClockSourceFormPWBA(PWGA18, PWBA_UNIT2);
        W_PWM_SetOutputSetCondition(PWGA18, 0);                 // Set counter vaule of output high level 
        W_PWM_SetOutputResetCondition(PWGA18, 2048);            // Set counter vaule of output low level.
        W_PWM_SetBufferReloadTrigger(PWGA18);                   // Trigger data reload
        W_PWM_SetClockEnable(PWBA0, PWBA_UNIT2);                // Enable PWBA UNIT1 clock.
        W_PWM_Set_PWGA_Trigger(SELB, 18);                       // Set trigger channel i
    }
}

//*************************************************************************************************************
void W_PWMBUZZER_Init(void)
{
    W_PWM_SetClock(PWBA0, PWBA_UNIT1, 5);                   // Set PWBA clock freq = PCLK(freq) /(2 * 5) = 20/10 = 2KHz. for PWMCLOCK1
    W_PWM_SetClock(PWBA0, PWBA_UNIT2, 4);                  // Set PWBA clock freq = PCLK(freq) /(2 * 4) = 20/8 = 2.5KHz for PWMCLOCK2
    R_DEV_PinInit(tpwm_pinconfig2);
}

