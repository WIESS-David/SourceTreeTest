/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "w_system.h"
#include "nios.h"
#include "r_bsp_gfx_sys.h"
#include "mem_init.h"
#include "davehd_types.h"
#include "davehd_debug.h"
#include "r_cdi_api.h"
#include "r_util_dhd.h"
#include "config.h"
#include "r_ddb_api.h"
#include "r_wm_api.h"
#include "wm.h"
#include "r_config_d1x.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_config_drw2d.h"
#include "r_drw2d_ctx_dhd.h"
#include "r_bsp_stdio_api.h"
#include "w_mag.h"
#include "w_cluster_task.h"
#include "main.h"

extern mars_info_def        gtmars_info;
extern queue_task_def       gtqueue_manager;
extern task_recorder_def    gttask_recorder[_TASK_NUM];
extern uint8_t u8cluster_demo_iconflag;
extern uint32_t u32_Sys_Timer_Count;
extern uint32_t u32_FPS_Count;

//============================================================================================================
// Table defines
//============================================================================================================
void (*const Cluster_State_Machine[_MAX_CLUSTER_ST_NO])(void) = 
{W_Cluster_Init, W_Cluster_Display, W_Cluster_Error};

//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Init(void)
{
    W_MAG_Init();
    gtmars_info.tcluster_info.u16timer1 = _TIME_5000ms;
    W_Task_ChangeState(_TYPE_CLUSTER, _LEVEL5, _STATE_CLUSTER_DISPLAY, Cluster_State_Machine[_STATE_CLUSTER_DISPLAY]);
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Display(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            W_MAG_Logo_Disp();
            W_MAG_WindowSwap();
            return;
        case _EVENT_LOOP:
            if(gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status == _G_TRUE)
                break;
            W_SYS_Main_Entry(); //TODO: one third 1/3
            W_MAG_Draw2D_Process();
            W_MAG_WindowSwap();
            Task_Done(gtqueue_manager.u8current_point);
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL4, _EVENT_LOOP);
            return;
        case _EVENT_DATA:
            Task_Done(gtqueue_manager.u8current_point);
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL4, _EVENT_LOOP);
            return;
        case _EVENT_TIMER1:
//            W_Cluster_IconClose();
//            u8cluter_icon_init = 1;
            u8cluster_demo_iconflag = 0;
            Task_Done(gtqueue_manager.u8current_point);
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL4, _EVENT_LOOP);
            return;
        default:
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Error(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tcluster_info.u16timer1 = _TIME_5000ms;
            break;
        case _EVENT_TIMER1:
            W_Task_ChangeState(_TYPE_CLUSTER, _LEVEL5, _STATE_CLUSTER_INIT, Cluster_State_Machine[_STATE_CLUSTER_INIT]);
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}

void W_FPS_Count() 
{
    //FPS Count
    if (u32_Sys_Timer_Count > 1000) {
        R_BSP_STDIO_Printf("FPS_Count:%d\n", u32_FPS_Count);
        u32_FPS_Count = 0;
        u32_Sys_Timer_Count = 0;
    }
    u32_Sys_Timer_Count++;
}

// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Timer1(void)
{
    if(gtmars_info.tcluster_info.u16timer1 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tcluster_info.u16timer1 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL3, _EVENT_TIMER1);
        gtmars_info.tcluster_info.u16timer1--;
    }

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Timer2(void)
{
    if(gtmars_info.tcluster_info.u16timer2 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tcluster_info.u16timer2 == _TIME_10ms)
            gtmars_info.tcluster_info.u8acc_on_check_over = _G_TRUE;
        gtmars_info.tcluster_info.u16timer2--;

    }

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_AllTimer(void)
{
    W_Cluster_Timer1();
    W_Cluster_Timer2();
    W_FPS_Count();
}
//----------------------------------------------------------------------------------------------------------------------------


