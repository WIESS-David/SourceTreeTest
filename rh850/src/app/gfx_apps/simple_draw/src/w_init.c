//Wiess Mars Init by Luke
#include "r_typedefs.h"
#include "main.h"
#include "r_dev_pin_types.h"
#include "r_dev_api.h "

#define _Port_I2C 44
#define _Pin_I2CDAT 10
#define _Pin_I2CCLK 11


static const r_dev_PinConfig_t tmarspin_config[] = 
{
#ifdef _MARS
//    { _Port_I2C, _Pin_I2CDAT, 4u, R_DEV_PIN_OUT, 1u, R_DEV_PIN_PULLNO, 1u, 0u, 0u, R_DEV_PIN_SCHMITT4}, 
//    { _Port_I2C, _Pin_I2CCLK, 4u, R_DEV_PIN_OUT, 1u, R_DEV_PIN_PULLNO, 1u, 0u, 0u, R_DEV_PIN_SCHMITT4}, 
    { _Port_TW8836, _Pin_TW8836RST , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    { _Port_TW8836, _Pin_TW8836DIS , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    { _Port_MCUReady , _Pin_ToTW8836 , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},    
//    { _Port_DetectState, _Pin_Reverse ,  0u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    { _Port_DetectState, _Pin_HDMIHPD ,  0u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    { _Port_DetectState, _Pin_CVBSReady ,  0u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    { _Port_Backlight , _Pin_ClusterEnable , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    { _Port_Backlight , _Pin_HMIEnable , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
//    { _Port_Backlight , 0, 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
//    { _Port_Backlight , 1, 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
#else if _MAG
    { _PORT_LCD, _PIN_STBY, 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    { _PORT_LCD, _PIN_BACKLINGT_EN, 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    { _PORT_LCD , _PIN_LCD_RST , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},    
    { _PORT_6V , _PIN_6V , 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},    
    { _PORT_RSV1, _PIN_RSV1, 0u, R_DEV_PIN_OUT, 0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},    
#endif
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 

};

void W_Pin_Init(void)
{
    R_DEV_PinInit(tmarspin_config);
}
