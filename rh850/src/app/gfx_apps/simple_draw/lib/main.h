/***********************************************************************************************
PROJECT : simple_draw
FILE   : $Id: main.h 13335 2017-04-24 07:17:12Z shinya.tomari $
============================================================================ 
DESCRIPTION
header for simple_draw
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2014
                                  by 
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/
#ifndef MAIN_H_
#define MAIN_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifdef MARS
#define _MARS
#endif 

#ifdef MAG
#define _MAG
#endif 

#ifdef HUK
#define _HUK
#endif   

//#define _HUK

#define LOC_JCUA_UNIT (0u)
#define LOC_SFMA_UNIT (0u)
#define LOC_VDCE_UNIT (0u)
//MAG GPIO
#define _PORT_POWER_STATUS 0
#define _PIN_CHARGE 0
#define _PIN_IGN 1
#define _PIN_ACC 3
#define _PORT_LAMP1 1
#define _PIN_TIAL 1
#define _PIN_FRONT_FOG 1
#define _PIN_LEFT_TURN 3
#define _PIN_HEAD_H 4
#define _PIN_HEAD_L 5
#define _PORT_LAMP2 3
#define _PIN_REV 0
#define _PIN_REAR_FOG 1
#define _PIN_RIGHT_TURN 2
#define _PORT_DEMO 3
#define _PIN_DEMO 3
#define _PORT_LCD 16
#define _PIN_STBY 8
#define _PIN_BACKLINGT_EN 9
#define _PORT_6V 16
#define _PIN_6V 10
#define _PIN_LCD_RST 11
#define _PORT_RSV1 10
#define _PIN_RSV1 0
// MARS USE Luke add
//GPIO


#define _Cluster_PWM 16
#define _HMI_PWM 17
#define _Panel_Cluster 0
#define _HMI_Cluster 1
#define _G_Input 1
#define _G_Output 0
#define _G_LowEN  0
#define _G_HighEN 1
#define _G_LowDIS 0
#define _G_HighDIS 1
#define _Port_Backlight 16
#define _Pin_ClusterEnable 9
#define _Pin_HMIEnable 10
#define _Port_MCUReady 17
#define _Pin_ToTW8836 0
#define _Port_TW8836 45
#define _Pin_TW8836RST 10
#define _Pin_TW8836DIS 11
#define _Port_DetectState 1
#define _Pin_Reverse 0
#define _Pin_HDMIHPD 1
#define _Pin_CVBSReady 2
#define _Port_Reset 17
#define _Pin_Flash 3

#define _G_ON 1
#define _G_OFF 0
#define _ILL 5
#define _Stall_D 8
#define _Stall_R 9
#define _G_TRUE 1
#define _G_FAIL 0

/*******************************************************************************
  Section: Local Functions
*/

void loc_Error(int32_t Err);

void* loc_StartSFMA(void);
void  loc_StopSFMA(void);
void* loc_StartOCTA(void);
void  loc_StopOCTA(void);

void loc_JcuaRun(void);

void* loc_StartSFMA2(void);
void W_APPMCU_Status(uint8_t u8temp);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
