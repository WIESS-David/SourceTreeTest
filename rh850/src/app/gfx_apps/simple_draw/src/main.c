/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: main.c 13335 2017-04-24 07:17:12Z shinya.tomari $
============================================================================ 
DESCRIPTION
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

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"        /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"         /* Board support package */
#include "r_bsp_hmi_api.h"     /* Board support package - HMI (Buttons + Rotary) */
#include "r_ddb_api.h"
#include "r_cdi_api.h"
#include "r_wm_api.h"
#include "davehd_driver.h"

#include "main.h"
#include "wm.h"
#include "mem_init.h"
#include "r_util_dhd.h"

#include "config.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"

#include "r_tick_api.h"

#include "r_jcua_api.h"
#include "r_bsp_stdio_api.h"
#include "r_dev_api.h"
#include "w_sys_pwm.h"
#include "w_buzzer_task.h"
#include "w_sys_reset.h"
#include "w_init.h"
#include "w_sys_stbc.h"
#include "w_system.h"
#include "nios.h"
#include "w_rscan_task.h"
#include "w_cluster_task.h"
#include "wiess_key_task.h"
#include "w_adce_task.h"

// Luke
const uint8_t cmversion[10] = "V1.00.03";

extern uint8_t gu8undo_num;
extern queue_task_def gtqueue_manager;
extern task_recorder_def gttask_recorder[_TASK_NUM];

extern void (*const RSCAN_State_Machine[_MAX_RSCAN_ST_NO])(void);
extern void (*const Cluster_State_Machine[_MAX_CLUSTER_ST_NO])(void);
extern void (*const ADCE_State_Machine[_MAX_ADCE_ST_NO])(void);

mars_info_def gtmars_info;
// Luke add
r_wm_Window_t ClusterSpeed_WM;
r_wm_Window_t ClusterRLE_WM;
r_wm_Window_t  ClusterRPM_WM;


extern void R_GPIO_WritePin(int32_t Port, uint8_t Pin, uint8_t Value);
/***********************************************************
  Section: Local Defines
*/
#if (defined(BSP_INIT_OCTA_FLASH_R) || defined(BSP_INIT_OCTA_RAM_RW) || defined(BSP_INIT_OCTA_PIN))
/* OCTA RAM/Flash */
#define LOC_EX_MEM_TYPE   (1)
#elif (defined(BSP_INIT_HYPB_FLASH_R) || defined(BSP_INIT_HYPB_RAM_RW) || defined(BSP_INIT_HYPB_PIN))
/* Hyper RAM/Flash */
#define LOC_EX_MEM_TYPE   (2)
#else
/* Serial Flash */
#define LOC_EX_MEM_TYPE   (0)
#endif

extern void R_GPIO_WritePin(int32_t Port, uint8_t Pin, uint8_t Value);
extern void R_SYS_GPIO_PinDirection(int32_t Port, uint8_t Pin, uint8_t Dir);

void W_APPMCU_Status(uint8_t u8temp)
{
    R_GPIO_WritePin( _Port_MCUReady, _Pin_ToTW8836, u8temp);
}

void W_APPBKL_Adjust(uint8_t u8duty)
{
    W_PWM_SetDuty( u8duty, _Cluster_PWM);
    W_PWM_SetDuty( u8duty, _HMI_PWM);
}

void W_APPBKL_Set(uint8_t u8panel,uint8_t u8temp)
{
    R_GPIO_WritePin( _Port_Backlight, u8panel, u8temp);
}

void W_APPBKL_Init(void)
{
    W_APPBKL_Set( _Pin_ClusterEnable, _G_HighEN);
    R_TICK_WaitMS(0,100);
    W_APPBKL_Set( _Pin_HMIEnable , _G_HighEN);
    W_PWMBKL_Init();
    W_APPBKL_Adjust(100);
}

void DetectACCStatus()
{
/*    if(W_Button_Detect(&gtbutton_ACC,ACC_OFF_DEBOUNCE_TIME) == 0)
    {
        W_APPBKL_Set( _Pin_ClusterEnable, _G_LowDIS);
        W_APPBKL_Set( _Pin_HMIEnable, _G_LowDIS);
        R_TICK_WaitMS(0,100);
        W_RESET_Software();
    }
*/
}

/*******************************************************************************
  Function: loc_Error

  This function is called when an error occurs.
  The application will hang.
*/

void loc_Error(int32_t Err)
{
    while (Err)
    {
    }
}
void W_Reset8836(uint8_t u8temp)
{
    R_GPIO_WritePin( _Port_TW8836, _Pin_TW8836RST , u8temp);
}
void W_ResetFlash( uint8_t u8temp)
{
    R_GPIO_WritePin( _Port_Reset, _Pin_Flash, u8temp);
}
void Clear_Watchdog(void)
{
    R_DEV_WRITE_REG(32, 0xffed0000, 0x00AC);
}
void Mars_Init(void)
{
    W_APPBKL_Init();
    W_Reset8836(_G_HighDIS);
    W_APPBKL_Adjust(80);
    W_BUZZER_Init();
}
void W_Panel_Test(void)
{
    R_GPIO_WritePin(_PORT_LCD,_PIN_LCD_RST,1);
    R_TICK_WaitMS(0,30);
    R_GPIO_WritePin(_PORT_LCD,_PIN_STBY,1);
    R_TICK_WaitMS(0,100);
    R_GPIO_WritePin(_PORT_LCD,_PIN_BACKLINGT_EN,1);
    R_TICK_WaitMS(0,40);
}
void W_System_Init(void)
{
    W_Pin_Init();
#ifdef _MARS
    W_Reset8836( _G_LowEN);
    W_ResetFlash( _G_HighDIS);
#else if _MAG
    W_Panel_Test();
    R_GPIO_WritePin(_PORT_6V,_PIN_6V,0);
#endif
    W_Button_Init();
}
void W_APPDetect_Process(void)
{
    DetectACCStatus();
}
/*============================================================================
 Sub Functions
============================================================================*/
void init_TAUB0(void) 
{                                           /* initialize timer TAUB0 channel 0 to interval timer mode */
    TAUB0TPS = 0x0022;                      /* specify clock frequency of CK0...CK3  (CK0 -> 10 MHz / (2 ^ 2) = 2.5 MHz);(CK1 -> 10 Mhz / (2 ^ 2) = 2.5 MHz) */
    TAUB0CMOR0 = 0x0000;                    /* interval timer mode 10ms timebase timer, clock source from CK0 */
    TAUB0CMOR1 = 0x0000;                    /* interval timer mode 100us  timebase timer, clock source from CK1 */
    TAUB0CDR0 = 250;                       /* compare register in interval timer mode (312.5KHz / 3125 = 100 Hz) */
    TAUB0CDR1 = 250;                        /* compare register in interval timer mode (2.5MHz / 250 = 10KHz) */
    TAUB0TS = 0x0003;                       /* start channel 0 and channel 1*/
    INTC1EIC17 = 0x0047;                    /* demask timer TAUB0 chanel 0 interrupt and enable reference table jump method */
    INTC1EIC18 = 0x0047;                    /* demask timer TAUB0 chanel 1 interrupt and enable reference table jump method */
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Main_TimerCore(void)
{
    static uint8_t u8_10ms_timer = 0;
    if(u8_10ms_timer == 0)
    {
        u8_10ms_timer = 100;
        W_RSCAN_AllTimer();
        W_Cluster_AllTimer();
        W_ADCE_AllTimer();
        W_Button_AllTimer();
        W_SYS_TIMER_ALL();
    }
    else
    {
        u8_10ms_timer--;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void W_Main_TaskInit(void)
{
    uint8_t u8counter = 0;
    W_Task_RecorderInit(gttask_recorder);
    W_Task_QueueManagerInit(&gtqueue_manager);
    
    // If you want to increase or decrease the tasks,please change _TASK_NUM at same time.
    // The recorder will remember the task that it need to execute.
    W_Task_TaskInRecorder(Cluster_State_Machine[_STATE_INIT],_TYPE_CLUSTER,u8counter++);
    W_Task_TaskInRecorder(RSCAN_State_Machine[_STATE_INIT],_TYPE_RSCAN,u8counter++);
    W_Task_TaskInRecorder(ADCE_State_Machine[_STATE_INIT],_TYPE_ADCE,u8counter++);
    
    // The assigner will arbitrate the priority for each tasks.
    W_Task_AssignedTask(gttask_recorder);
    
    // Excuter will push task into the queue.
    W_Task_PushTaskInQueue(&gtqueue_manager,gttask_recorder,gu8undo_num);
}
/*******************************************************************************
  Function: main

  entry after startup
*/
int32_t main(void)
{
    W_BSP_Init();
    W_System_Init();
    
#if 1
//    while(W_Button_Detect(&gtbutton_ACC,ACC_ON_DEBOUNCE_TIME) == 0){}
#else
    W_STBC_GoToDeepStop();
#endif
    W_Main_TaskInit();
    init_TAUB0();
    W_Get_Start_Procedure();
    Mars_Init();
    /*
     * get start procedure 
     */
    while(1)
    {
        W_APPDetect_Process();
        W_Task_ProcessTask();
    }
    //R_UTIL_DHD_DeInit(LOC_DHD_UNIT);

    //return(0);
}

/*============================================================================
 Interrupt Service Routines - ISR
============================================================================*/
/* TAUB0 interrupt service routine */
__interrupt void INTTAUB0I0(void)
{
    W_Main_TimerCore();
}

__interrupt void INTTAUB0I1(void)
{
    W_BUZZER_AllTimer();
    W_BUZZER_Process();
}

/* 
__interrupt void INTRCAN0REC(void)
{
    W_RSCAN_ChangeState(_STATE_RSCAN_TIMING);
}

__interrupt void INTRCAN0ERR(void)
{
    W_RSCAN_ChangeState(_STATE_RSCAN_ERROR);
}

__interrupt void INTADCE0I1(void)
{
    R_DEV_Nop();
}
*/
