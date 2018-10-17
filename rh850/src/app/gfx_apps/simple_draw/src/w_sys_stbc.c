#include "w_sys_stbc.h"

//*************************************************************************************************************
void W_STBC_TiggerDeepStop(void)
{
    uint32_t u32temp = 0x00000002;
    SYSPROTCMD0 = 0xA5;
    SYSSTBC0PSC = u32temp;
    SYSSTBC0PSC = ~u32temp;
    SYSSTBC0PSC = u32temp;
}
//*************************************************************************************************************
uint32_t W_STBC_GetWakeUpEventVaule(void)
{
    return SYSWUF0;
}
//*************************************************************************************************************
void W_STBC_SetWakeUp(uint32_t u32event_channel,uint8_t u8enable)
{
    if(u8enable == STBC_ENABLE)
        SYSWUFMSK0 &= ~(0x000000001 << u32event_channel);
    else
        SYSWUFMSK0 |= (0x000000001 << u32event_channel);
}
//*************************************************************************************************************
void W_STBC_SetWakeUpMaskVaule(uint32_t u32value)
{
    SYSWUFMSK0 = u32value;
}
//*************************************************************************************************************
uint32_t W_STBC_GetWakeUpMaskValue(void)
{
    return SYSWUFMSK0;
}
//*************************************************************************************************************
void W_STBC_ClearWakeUpEvent(uint32_t u32event_channel)
{
    SYSWUFC0 |= (0x00000001 << u32event_channel);
}
//*************************************************************************************************************
void W_STBC_ClearWakeUpEventValue(uint32_t u32value)
{
    SYSWUFC0 = u32value;
}
//*************************************************************************************************************
void W_STBC_SetIOHOLD(uint32_t      u32io_hold_ch, uint8_t u8activate)
{
    uint32_t u32temp;
        
    if(u8activate == IOHOLD_ACTIVATE)
        u32temp = SYSIOHOLD | (0x000000001 << u32io_hold_ch);
    else
        u32temp = SYSIOHOLD & ~(0x000000001 << u32io_hold_ch);

    SYSPROTCMD0 = 0xA5;
    SYSIOHOLD = u32temp;
    SYSIOHOLD = ~u32temp;
    SYSIOHOLD = u32temp;
}
//*************************************************************************************************************
void W_STBC_SetIOHOLDValue(uint32_t u32value)
{
    SYSPROTCMD0 = 0xA5;
    SYSIOHOLD = u32value;
    SYSIOHOLD = ~u32value;
    SYSIOHOLD = u32value;
}
//*************************************************************************************************************
uint32_t  W_STBC_GetIOHOLDValue(void)
{
    return SYSIOHOLD;
}
//*************************************************************************************************************
void W_STBC_GoToDeepStop(void)
{
    W_STBC_ClearWakeUpEventValue(0xFFFFFFFF);
    W_STBC_SetIOHOLDValue(0x0000007F);
    W_STBC_SetWakeUp(WAKE_UP_CH1,STBC_ENABLE);
    W_STBC_TiggerDeepStop();
}

