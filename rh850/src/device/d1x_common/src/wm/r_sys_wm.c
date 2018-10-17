/*
****************************************************************************
PROJECT : WM
FILE    : $Id: r_sys_wm.c 13421 2017-05-08 13:49:09Z matthias.nippert $
============================================================================ 
DESCRIPTION
WM system functions for D1x
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2016
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
****************************************************************************
*/

/***********************************************************
  Title: D1Mx and D1Lx WM SYS API implementation  
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "r_cdi_api.h"

#include "r_ddb_api.h"      /* display database with display timings */
#include "r_vdce_api.h"
#include "r_config_spea.h"
#include "r_spea_api.h"

#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"

#ifdef USE_ROS
#include "r_os_api.h"
#endif

#include "r_dev_pin_types.h"
#include "r_dev_api.h"

#if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
#include "r_profile.h"
#endif

#ifdef R_WM_SYS_ERROR_TRACKING
    #include "r_bsp_stdio_api.h"
#endif




/***********************************************************
  Section: Local Defines
*/


/***********************************************************
  Section: Local Function Declarations
*/
static void loc_VblankIsr(uint32_t Unit);
static void loc_ScanlineIsr(uint32_t Unit);
static void loc_VI_VblankIsr(uint32_t Unit);
static void locIsr_VO_0_Vblank(void);
static void locIsr_VO_0_Scanline(void);
static void locIsr_VI_0_Vblank(void);
static void locIsr_VI_0_Overflow(void);
static void locIsr_VO_0_Layer0underflow(void);
static void locIsr_VO_0_Layer1underflow(void);
static void locIsr_VO_0_Layer2underflow(void);
static void locIsr_VO_0_Layer3underflow(void);
static void locIsr_VO_1_Vblank(void);
static void locIsr_VO_1_Scanline(void);
static void locIsr_VI_1_Vblank(void);
static void locIsr_VI_1_Overflow(void);
static void locIsr_VO_1_Layer0underflow(void);
static void locIsr_VO_1_Layer1underflow(void);
static void locIsr_VO_1_Layer2underflow(void);
static void locIsr_VO_1_Layer3underflow(void);

static void loc_VO_LayerunderflowIsr(uint32_t Unit, uint32_t Layer);
static void loc_VI_OverflowIsr(uint32_t Unit);


static void loc_VDCE_ErrorCallback(const uint32_t Unit, const r_vdce_Error_t Error);
static void loc_SPEA_ErrorCallback(const uint32_t Unit, uint32_t Error);


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable : r_wm_DevSys
  
  Sys layer device object.
*/
r_wm_DevSys_t r_wm_DevSys[R_WM_DEV_NUM];

r_wm_WinCapbs_t r_wm_DevCabps[ R_WM_MAX_LAYERS] = {
    /* Default features of VDCE HW layers */
    R_WM_WINCAPBS_RLE, R_WM_WINCAPBS_SPRITES, R_WM_WINCAPBS_SPRITES, R_WM_WINCAPBS_SPRITES };
    


/***********************************************************
  Variable : device
  
  Current device.
*/
r_dev_Device_t r_wm_Device = R_DEV_LAST_DX_DEV; /* set invalid */


/***********************************************************
  Variable : loc_WM_Isr
  
  List of all WM internal interrupt callbacks.
*/

void (*const loc_WM_Isr[R_WM_DEV_NUM][R_WM_EVENT_LAST])(void) =
{
    { &locIsr_VO_0_Vblank, &locIsr_VO_0_Scanline, &locIsr_VI_0_Vblank, &locIsr_VI_0_Overflow, &locIsr_VO_0_Layer0underflow, &locIsr_VO_0_Layer1underflow, &locIsr_VO_0_Layer2underflow, &locIsr_VO_0_Layer3underflow}
#if (R_WM_DEV_NUM > 1)
    ,
    { &locIsr_VO_1_Vblank, &locIsr_VO_1_Scanline, &locIsr_VI_1_Vblank, &locIsr_VI_1_Overflow, &locIsr_VO_1_Layer0underflow, &locIsr_VO_1_Layer1underflow, &locIsr_VO_1_Layer2underflow, &locIsr_VO_1_Layer3underflow}
#endif
};




/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable : ROS Synchronisation Objects
  
  Variables used by ROS for threadsafe operation.
*/
#ifdef LOC_USE_OS
    static R_OS_ThreadMutex_t     locMutexMsgQueue[R_WM_DEV_NUM];
    static R_OS_ThreadMutex_t     locMutexBufferMgmnt[R_WM_DEV_NUM];
    static R_OS_ThreadMutex_t     locMutexWindows[R_WM_DEV_NUM];
    static R_OS_ThreadMutex_t     locMutexDevice[R_WM_DEV_NUM];
    static uint32_t               locMutexInitCreated[R_WM_DEV_NUM] = {0};
    static R_OS_Sem_t             locSemaISRTrigger[R_WM_DEV_NUM][R_WM_EVENT_LAST];
#endif

/***********************************************************
  Variable : loc_Wait*
  
  Synchronisation flags mainly used by non-threading configuration of WM to synchronise with the interrupts.
*/
static volatile uint32_t loc_WaitVblankInt[R_WM_DEV_NUM];
static volatile uint32_t loc_WaitScanlineInt[R_WM_DEV_NUM];

/***********************************************************
  Variable : loc_WM_LastError_*
  
  If active, defines variables to save the last occurred error.
*/
#ifdef R_WM_SYS_ERROR_TRACKING
    volatile r_wm_Error_t loc_WM_LastError_Code = R_WM_ERR_OK;
    volatile uint32_t     loc_WM_LastError_Info = 0;
    volatile uint32_t     loc_WM_LastError_Unit = 0;
    volatile char *       loc_WM_LastError_File = 0;
    volatile uint32_t     loc_WM_LastError_Line = 0;
    volatile uint32_t     loc_WM_LastError_Overflow = 0;
    volatile uint32_t     loc_WM_LastError_NewError = 0;
#endif /* R_WM_SYS_ERROR_TRACKING */

/***********************************************************
  Variable : loc_*Heap_WM
  
  References to the memory manager used for data structure and buffer allocation.
*/
static r_cdi_Heap_t *loc_CpuHeap_WM = 0;
static r_cdi_Heap_t *loc_VidHeap_WM = 0;




/***********************************************************
  Section: Local Functions
*/

/***********************************************************
  Function: loc_IsrVblank
  
  Generic handler for the VBLANK interrupt.
*/
static void loc_VblankIsr(uint32_t Unit)
{
    r_wm_Event_t event = { R_WM_EVENT_VBLANK };

#if defined R_WM_SKIP_FRAME_ON_SPEA_BUSY
    uint32_t spea_update_pending;
    
    /* get update flag and filter for unit - if flag is set, do not process this interrupt */
    spea_update_pending =  R_SPEA_UnitUpdateActive(0, R_SPEA_SU0);
    spea_update_pending |= R_SPEA_UnitUpdateActive(0, R_SPEA_SU1);
    spea_update_pending |= R_SPEA_UnitUpdateActive(0, R_SPEA_SU2);
    if ( 4 == R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO) )
    {
        spea_update_pending |= R_SPEA_UnitUpdateActive(0, R_SPEA_SU3);
    }
    spea_update_pending &= (1uL<<Unit);
    if (0 != spea_update_pending)
    {
  #if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
        R_WM_Profile_Log_VBLANK_Skipped();
  #endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */
        return;
    }
#endif /* R_WM_SKIP_FRAME_ON_SPEA_BUSY */


#ifdef LOC_USE_OS
    while (loc_WaitVblankInt[Unit] != 0)
    {
        R_OS_SemPost(&locSemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
        loc_WaitVblankInt[Unit]--;
    }
#else /* LOC_USE_OS */
    if (loc_WaitVblankInt[Unit] == 1)
    {
        loc_WaitVblankInt[Unit] = 0;
    }
#endif /* LOC_USE_OS */


    if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE) )
    {
        /* Can we do triple buffering? */
        if (r_wm_DevSys[Unit].RootCapt->Window->Surface.Fb.BufNum > 2)
        {
            r_wm_WinBuffer_t *buf;
            
            /* Get a free buffer */
            buf = R_WM_Cap_CapBufGet (Unit, r_wm_DevSys[Unit].RootCapt->Window);

            if (buf != 0)
            {
                /* And capture into it starting with the next frame */
                R_VDCE_CapBufSet(r_wm_DevSys[Unit].RootCapt->CapUnit,
                                 (uint32_t)buf->Data,
                                 (uint32_t)buf->Data);
            }
        }
    }

    if ((r_wm_DevSys[Unit].RegEvents & (uint32_t)(1uL << R_WM_EVENT_VBLANK)) != 0)
    {
        if (r_wm_DevSys[Unit].EventCb != 0)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
        }
    }
}

/***********************************************************
  Function: loc_IsrScanline
  
  Generic handler for the SCANLINE interrupt.
*/
static void loc_ScanlineIsr(uint32_t Unit)
{
    r_wm_Event_t event = { R_WM_EVENT_SCANLINE };
    if ((r_wm_DevSys[Unit].RegEvents & (uint32_t)(1uL << R_WM_EVENT_SCANLINE)) != 0)
    {
        if (r_wm_DevSys[Unit].EventCb != 0)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
        }
    }

#ifdef LOC_USE_OS
    while (loc_WaitScanlineInt[Unit] != 0)
    {
        R_OS_SemPost(&locSemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
        loc_WaitScanlineInt[Unit]--;
    }
#else /* LOC_USE_OS */
    if (loc_WaitScanlineInt[Unit] == 1)
    {
        loc_WaitScanlineInt[Unit] = 0;
    }
#endif /* LOC_USE_OS */
}

/***********************************************************
  Function: loc_VI_VblankIsr
  
  Generic handler for the Video Input VBLANK interrupt.
*/
static void loc_VI_VblankIsr(uint32_t Unit)
{
    r_wm_Event_t event = { R_WM_EVENT_VI_VBLANK };
    
    if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE) )
    {
        /* Can we do triple buffering? */
        if (r_wm_DevSys[Unit].RootCapt->Window->Surface.Fb.BufNum > 2)
        {
            r_wm_WinBuffer_t *buf;
            
            /* Get freshly captured buffer */
            buf = R_WM_Cap_DispBufGet(Unit,r_wm_DevSys[Unit].RootCapt->Window);
            
            if (buf != 0)
            {
                /* And display it with the next frame */
                if (R_FALSE == R_WM_Sys_WindowSetFb(Unit, r_wm_DevSys[Unit].RootCapt->Window, buf->Data))
                {
                    /*err = R_WM_ERR_SYS_WIN_SWAP_FAILED;*/
                }
            }
        }
    }
    
    if ((r_wm_DevSys[Unit].RegEvents & (uint32_t)(1uL << R_WM_EVENT_VI_VBLANK)) != 0)
    {
        if (r_wm_DevSys[Unit].EventCb != 0)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
        }
    }
}


/***********************************************************
  Function: loc_VI_OverflowIsr
  
  Generic handler for the overflow interrupt.
*/
static void loc_VI_OverflowIsr(uint32_t Unit)
{
    r_wm_Event_t event = { R_WM_EVENT_VI_OVERFLOW };
    if ((r_wm_DevSys[Unit].RegEvents & (uint32_t)(1uL << R_WM_EVENT_VI_OVERFLOW)) != 0)
    {
        if (r_wm_DevSys[Unit].EventCb != 0)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
        }
    }
}


/***********************************************************
  Function: loc_VO_LayerunderflowIsr
  
  Generic handler for the undeflow interrupt.
*/
static void loc_VO_LayerunderflowIsr(uint32_t Unit, uint32_t Layer)
{
    r_wm_Event_t event = { R_WM_EVENT_LAYER0_UNDERFLOW, 0 };

    /* see what layer is received */
    if(Layer == 0)
    {
        event.Id = R_WM_EVENT_LAYER0_UNDERFLOW;
    }
    else if(Layer == 1)
    {
        event.Id = R_WM_EVENT_LAYER1_UNDERFLOW;
    }
    else if(Layer == 2)
    {
        event.Id = R_WM_EVENT_LAYER2_UNDERFLOW ;
    }
    else /* layer 3 underflow */
    {
        event.Id = R_WM_EVENT_LAYER3_UNDERFLOW;
    }


    if ((r_wm_DevSys[Unit].RegEvents & (uint32_t)(1uL << event.Id)) != 0)
    {
        if (r_wm_DevSys[Unit].EventCb != 0)
        {
            r_wm_DevSys[Unit].EventCb(Unit, &event);
        }
    }
}


/***********************************************************
  Function: locIsr_VO_0_Vblank
  
  VDCE VBLANK ISR for VOUT unit 0.
*/
static void locIsr_VO_0_Vblank(void)
{
    loc_VblankIsr(0);
}

/***********************************************************
  Function: locIsr_VO_0_Scanline
  
  VDCE SCANLINE ISR for VOUT unit 0.
*/

static void locIsr_VO_0_Scanline(void)
{
    loc_ScanlineIsr(0);
}

/***********************************************************
  Function: locIsr_VI_0_Vblank
  
  VDCE VBLANK ISR for VIN unit 0.
*/
static void locIsr_VI_0_Vblank(void)
{
    loc_VI_VblankIsr(0);
}

/***********************************************************
  Function: locIsr_VI_0_Overflow
  
  VDCE Overflow ISR for VIN unit 0.
*/
static void locIsr_VI_0_Overflow(void)
{
    loc_VI_OverflowIsr(0);
}

/***********************************************************
  Function: locIsr_VO_0_Layer0underflow
  
  VDCE underflow layer 0 ISR for VO unit 0.
*/
static void locIsr_VO_0_Layer0underflow(void)
{
    loc_VO_LayerunderflowIsr(0, 0);
}

/***********************************************************
  Function: locIsr_VO_0_Layer1underflow
  
  VDCE underflow layer 1 ISR for VO unit 0.
*/
static void locIsr_VO_0_Layer1underflow(void)
{
    loc_VO_LayerunderflowIsr(0, 1);
}

/***********************************************************
  Function: locIsr_VO_0_Layer2underflow
  
  VDCE underflow layer 2 ISR for VO unit 0.
*/
static void locIsr_VO_0_Layer2underflow(void)
{
    loc_VO_LayerunderflowIsr(0, 2);
}

/***********************************************************
  Function: locIsr_VO_0_Layer3underflow
  
  VDCE underflow layer 3 ISR for VO unit 0.
*/
static void locIsr_VO_0_Layer3underflow(void)
{
    loc_VO_LayerunderflowIsr(0, 3);
}



#if (R_WM_DEV_NUM > 1)

/***********************************************************
  Function: locIsr_VO_1_Vblank
  
  VDCE VBLANK ISR for VOUT unit 1.
*/

static void locIsr_VO_1_Vblank(void)
{
    loc_VblankIsr(1);
}

/***********************************************************
  Function: locIsr_VO_0_Scanline
  
  VDCE SCANLINE ISR for VOUT unit 1.
*/

static void locIsr_VO_1_Scanline(void)
{
    loc_ScanlineIsr(1);
}

/***********************************************************
  Function: locIsr_VI_1_Vblank
  
  VDCE VBLANK ISR for VIN unit 1.
*/
static void locIsr_VI_1_Vblank(void)
{
    loc_VI_VblankIsr(1);
}

/***********************************************************
  Function: locIsr_VI_1_Overflow
  
  VDCE Overflow ISR for VIN unit 1.
*/
static void locIsr_VI_1_Overflow(void)
{
    loc_VI_OverflowIsr(1);
}

/***********************************************************
  Function: locIsr_VO_1_Layer0underflow
  
  VDCE underflow layer 0 ISR for VO unit 1.
*/
static void locIsr_VO_1_Layer0underflow(void)
{
    loc_VO_LayerunderflowIsr(1, 0);
}

/***********************************************************
  Function: locIsr_VO_1_Layer1underflow
  
  VDCE underflow layer 1 ISR for VO unit 1.
*/
static void locIsr_VO_1_Layer1underflow(void)
{
    loc_VO_LayerunderflowIsr(1, 1);
}

/***********************************************************
  Function: locIsr_VO_1_Layer2underflow
  
  VDCE underflow layer 2 ISR for VO unit 1.
*/
static void locIsr_VO_1_Layer2underflow(void)
{
    loc_VO_LayerunderflowIsr(1, 2);
}

/***********************************************************
  Function: locIsr_VO_1_Layer3underflow
  
  VDCE underflow layer 3 ISR for VO unit 1.
*/
static void locIsr_VO_1_Layer3underflow(void)
{
    loc_VO_LayerunderflowIsr(1, 3);
}



#endif /* (R_WM_DEV_NUM > 1) */
 


#ifdef R_WM_SYS_ERROR_TRACKING
    
    /***********************************************************
      Function: R_WM_Sys_GetLastError
    */
    r_wm_Error_t R_WM_Sys_GetLastError(uint32_t *Unit, uint32_t *AdditionalInfo, uint32_t *Overflow, uint32_t *New)
    {
        if (Unit != 0)
        {
            *Unit = loc_WM_LastError_Unit;
        }
        if (AdditionalInfo != 0)
        {
            *AdditionalInfo = loc_WM_LastError_Info;
        }
        if (Overflow != 0)
        {
            *Overflow = loc_WM_LastError_Overflow;
        }
        if (New != 0)
        {
            *New = loc_WM_LastError_NewError;
        }
        
        loc_WM_LastError_NewError = 0;
        loc_WM_LastError_Overflow = 0;
        
        return loc_WM_LastError_Code;
    }
    
    /***********************************************************
      Function: loc_SetLastError
    */
    void loc_WM_Sys_SetLastError(uint32_t Unit, r_wm_Error_t Error, uint32_t AdditionalInfo, char *File, uint32_t Line)
    {
        if (AdditionalInfo != 0)
        {
            R_BSP_STDIO_Printf("ERROR in %s, Line %u, Unit %u, Code %u/%u\n", File, Line, Unit, Error, AdditionalInfo);
        }
        else
        {
            R_BSP_STDIO_Printf("ERROR in %s, Line %u, Unit %u, Code %u\n", File, Line, Unit, Error);
        }
        
        if (0 == loc_WM_LastError_NewError)
        {
            loc_WM_LastError_Code = Error;
            loc_WM_LastError_Info = AdditionalInfo;
            loc_WM_LastError_Unit = Unit;
            loc_WM_LastError_File = File;
            loc_WM_LastError_Line = Line;
            loc_WM_LastError_NewError = 1;
        }
        else
        {
            loc_WM_LastError_Overflow = 1;
        }
    }

#endif /* R_WM_SYS_ERROR_TRACKING */


/***********************************************************
  Function: loc_VDCE_ErrorCallback
  
  Function registered as error callback for the VDCE driver.
*/
static void loc_VDCE_ErrorCallback(const uint32_t Unit, const r_vdce_Error_t Error)
{
    if (R_VDCE_ERR_OK != Error)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, Error);
        R_WM_ErrorHandler(Unit, R_WM_ERR_VOUT_INTERNAL);
    }
}

/***********************************************************
  Function: loc_SPEA_ErrorCallback
  
  Function registered as error callback for the SPEA driver.
*/
static void loc_SPEA_ErrorCallback(const uint32_t Unit, uint32_t Error)
{
    if (R_SPEA_ERR_OK != Error)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SPEA_INTERNAL, Error);
        R_WM_ErrorHandler(Unit, R_WM_ERR_SPEA_INTERNAL);
    }
}

/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/

/***********************************************************
  Function: R_WM_Sys_DeviceFeature
*/
uint32_t R_WM_Sys_DeviceFeature( r_wm_sys_DevFeature_t Feature )
{
    uint32_t retval = 0;
    switch (Feature) {
        case R_WM_SYS_FEATURE_RLE_LAYER_NO:
            /* D1M1A has 4, all other devices have 1 layer */
            if (r_wm_Device == R_DEV_R7F701441) {
                retval = R_SPEA_D1M1A_MAX_SPRITE_UNIT;
            }
            else {
                retval = 4-R_SPEA_MAX_SPRITE_UNIT;
            }
            break;
            
        case R_WM_SYS_FEATURE_SPRITE_LAYER_NO:
            /* D1M1A has 4, all other devices have 3 layers */
            if (r_wm_Device == R_DEV_R7F701441) {
                retval = R_SPEA_D1M1A_MAX_SPRITE_UNIT;
            }
            else {
                retval = R_SPEA_MAX_SPRITE_UNIT;
            }
            break;
            
        case R_WM_SYS_FEATURE_SWITCH_CAPABILITIES:
            /* Not supported by D1L2(H). Lower devices haven't a compatible VO at all */
            if ( r_wm_Device <= R_DEV_R7F701403 ){
                retval = 0;
            }
            else {
                retval = 1;
            }
            break;
            
        case R_WM_SYS_FEATURE_GAMMA_CORRECTION:
            /* Not supported by D1L2(H). Lower devices haven't a compatible VO at all */
            if ( r_wm_Device <= R_DEV_R7F701403 ){
                retval = 0;
            }
            else {
                retval = 1;
            }
            break;
            
        default:
            break;
    }
    return retval;
}


/***********************************************************
  Group: WM Basic interface functions
*/




/***********************************************************
  Function: R_WM_Sys_DevCountGet
*/
uint32_t R_WM_Sys_DevCountGet(void)
{
    return R_WM_DEV_NUM;
}

/***********************************************************
  Function: R_WM_Sys_DevInfoGet
*/
void  R_WM_Sys_DevInfoGet   (   uint32_t  Unit,
                                uint32_t* MaxLayers,
                                uint32_t* MaxPitch, 
                                uint32_t* MaxWidth, 
                                uint32_t* MaxHeight,
                                uint32_t* SpritesOrderAscending
                            )
{
    if (0 != MaxLayers)
    {
        *MaxLayers =  R_WM_MAX_LAYERS;
    }
    if (0 != MaxPitch)
    {
       *MaxPitch = R_WM_MAX_LAYER_PITCH;
    }
    if (0 != MaxWidth)
    {
       *MaxWidth = R_WM_MAX_LAYER_WIDTH;
    }
    if (0 != MaxHeight)
    {
       *MaxHeight = R_WM_MAX_LAYER_HEIGHT;
    }
    if (0 != SpritesOrderAscending)
    {
        *SpritesOrderAscending = 0;
    }
}

/***********************************************************
  Function: R_WM_Sys_DevInit
*/
uint32_t R_WM_Sys_DevInit   (   uint32_t Unit,
                                void (*EventCb)(uint32_t Unit, const r_wm_Event_t *Event)
                            )
{
    uint32_t i;
    
    r_wm_Device = R_DEV_GetDev();
    
#ifdef LOC_USE_OS

    /* create a mutex for access to the event queue*/
    R_OS_ThreadMutexInit(&locMutexMsgQueue[Unit], 0);

    /* create a mutex for access to the buffers of a window */
    R_OS_ThreadMutexInit(&locMutexBufferMgmnt[Unit], 0);

    /* create a mutex for global WM locking */
    R_OS_ThreadMutexInit(&locMutexWindows[Unit], 0);

    for (i = 0; i < R_WM_EVENT_LAST; ++i)
    {		 
        R_OS_SemInit(&locSemaISRTrigger[Unit][i], 0, 0);
    }
#endif /* LOC_USE_OS */

    loc_WaitVblankInt[Unit] = 0;
    loc_WaitScanlineInt[Unit] = 0;

    for (i = 0; i <  R_WM_MAX_LAYERS; ++i)
    {
        r_wm_DevSys[Unit].WinToLayerMap[i].Win = R_NULL;
        r_wm_DevSys[Unit].WinToLayerMap[i].Layer = LOC_INVALID_LAYER;
        r_wm_DevSys[Unit].HwLayerEnabled[i] = 0;
    }
    r_wm_DevSys[Unit].EventCb = EventCb;
    r_wm_DevSys[Unit].GlobalStatusFlags = 0;
    r_wm_DevSys[Unit].HorizontalMoveReserve = 0;
    
    if (R_VDCE_ERR_OK != R_VDCE_Init(Unit))
    {
        return 0;
    }
    if (R_VDCE_ERR_OK != R_VDCE_ErrorCallbackSet(&loc_VDCE_ErrorCallback))
    {
        return 0;
    }

    if (R_SPEA_ERR_OK != R_SPEA_Init(0))
    {
        return 0;
    }
    if (R_SPEA_ERR_OK != R_SPEA_UnitEnable(0, R_SPEA_SU0, 1))
    {
        return 0;
    }
    if (R_SPEA_ERR_OK != R_SPEA_UnitEnable(0, R_SPEA_SU1, 1))
    {
        return 0;
    }
    if (R_SPEA_ERR_OK != R_SPEA_UnitEnable(0, R_SPEA_SU2, 1))
    {
        return 0;
    }
    if ( 4 == R_WM_Sys_DeviceFeature(R_WM_SYS_FEATURE_SPRITE_LAYER_NO) )
    {
        if (R_SPEA_ERR_OK != R_SPEA_UnitEnable(0, R_SPEA_SU3, 1))
        {
            return 0;
        }
    }
    R_SPEA_SetErrorCallback(&loc_SPEA_ErrorCallback);
    
    if (R_VDCE_ERR_OK != R_VDCE_DisplayDisable(Unit))
    {
        return 0;
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_DeInit
*/
uint32_t R_WM_Sys_DevDeinit(uint32_t  Unit)
{
#ifdef LOC_USE_OS
    uint32_t i;
#endif /* LOC_USE_OS */

	/* Preparing shutdown by trying to escape from any wait states */
    r_wm_DevSys[Unit].GlobalStatusFlags |= R_WM_SYS_GLOBAL_STATUS_FLAG_SHUTDOWN;

#ifdef LOC_USE_OS
    /* Make sure, wm isn't waiting for some events */
    while (loc_WaitVblankInt[Unit] != 0)
    {
        R_OS_SemPost(&locSemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
        loc_WaitVblankInt[Unit]--;
    }
    while (loc_WaitScanlineInt[Unit] != 0)
    {
        R_OS_SemPost(&locSemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
        loc_WaitScanlineInt[Unit]--;
    }
    
    /* Wait for MSG Queue to exit */
    R_OS_ThreadSleep(10);   

    /* delete mutex */
    R_OS_ThreadMutexDestroy(&locMutexWindows[Unit]);
    R_OS_ThreadMutexDestroy(&locMutexMsgQueue[Unit]);
    R_OS_ThreadMutexDestroy(&locMutexBufferMgmnt[Unit]);

    for (i = 0; i < R_WM_EVENT_LAST; ++i)
    {		 
        R_OS_SemDestroy(&locSemaISRTrigger[Unit][i]);
    }
#else /* LOC_USE_OS */
	loc_WaitVblankInt[Unit] = 0;
    loc_WaitScanlineInt[Unit] = 0;
#endif /* LOC_USE_OS */

    if (R_VDCE_ERR_OK != R_VDCE_DeInit(Unit))
    {
        return 0;
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_DevEventRegister
*/
uint32_t R_WM_Sys_DevEventRegister  (   uint32_t        Unit,
                                        r_wm_EventId_t  EventId,
                                        uint32_t        Arg
                                    )
{
    r_wm_DevSys[Unit].RegEvents |= (uint32_t)(1uL << EventId);
    if (EventId == R_WM_EVENT_SCANLINE)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_SCANLINE, loc_WM_Isr[Unit][R_WM_EVENT_SCANLINE]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcScanlineSet(Unit, Arg))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_SCANLINE))
        {
            return 0;
        }
    }
    else if(EventId == R_WM_EVENT_VI_OVERFLOW)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW, loc_WM_Isr[Unit][R_WM_EVENT_VI_OVERFLOW]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_ERR_CAP_WRITE_OVERFLOW))
        {
            return 0;
        }
    }
    else if(EventId == R_WM_EVENT_LAYER0_UNDERFLOW)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_ERR_LAYER0_UNDERFLOW, loc_WM_Isr[Unit][R_WM_EVENT_LAYER0_UNDERFLOW]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_ERR_LAYER0_UNDERFLOW))
        {
            return 0;
        }
    }
    else if(EventId == R_WM_EVENT_LAYER1_UNDERFLOW)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_ERR_LAYER1_UNDERFLOW, loc_WM_Isr[Unit][R_WM_EVENT_LAYER1_UNDERFLOW]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_ERR_LAYER1_UNDERFLOW))
        {
            return 0;
        }
    }
    else if(EventId == R_WM_EVENT_LAYER2_UNDERFLOW)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_ERR_LAYER2_UNDERFLOW, loc_WM_Isr[Unit][R_WM_EVENT_LAYER2_UNDERFLOW]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_ERR_LAYER2_UNDERFLOW))
        {
            return 0;
        }
    }
    else if(EventId == R_WM_EVENT_LAYER3_UNDERFLOW)
    {
        if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Unit, R_VDCE_INTC_ERR_LAYER3_UNDERFLOW, loc_WM_Isr[Unit][R_WM_EVENT_LAYER3_UNDERFLOW]))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Unit, R_VDCE_INTC_ERR_LAYER3_UNDERFLOW))
        {
            return 0;
        }
    }
    else
    {
        /* nothing */
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_IsShutdownActive
*/
uint32_t R_WM_Sys_IsShutdownActive(uint32_t Unit)
{
    if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_SHUTDOWN) )
    {
        return 1;
    }
    return 0;
}




/***********************************************************
  Group: WM Basic internal frame synchronous control functions
*/


/***********************************************************
  Function: R_WM_Sys_DevFrameStarted
*/
void R_WM_Sys_DevFrameStarted(uint32_t Unit)
{
    /* reset the sprite unit update flags */
    r_wm_DevSys[Unit].FrameStatusFlags = 0;
}


/***********************************************************
  Function: R_WM_Sys_DevFrameFinished
*/
void R_WM_Sys_DevFrameFinished(uint32_t Unit)
{
    /* check the sprite unit update flags */
    uint32_t layer;
    
    /* Rewrite the Sprite configuration for those units that got changed during the last frame */
    if (0 == loc_WM_Sprite_ReassignAllSprites(Unit))
    {
        /* This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, so just report and continue */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
    }
    
    /* Set update request bit for those Sprite Unit Registers, that have been written to in the last frame */
    if (0 == loc_WM_Sprite_UpdateAllSprites(Unit))
    {
        /* This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, so just report and continue */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
    }

    /* Process update request for those RLE Unit Registers, if it has been written to in the last frame */
    if (0 == loc_WM_Sprite_UpdateAllRLEs(Unit))
    {
        /* This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, so just report and continue */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
    }
    
    
    /* If no window was visible in this frame, pending SPEA Update Requests cannot be completed immediately.
       => Manually create a SYNC signal. 
          Flickering or artifacts are no problem as nothing is visible on-screen! */
    if (0 == loc_WM_Sprite_SynchronizeSprites(Unit))
    {
        /* This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, so just report and continue */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
    }
          
    /* Check if a CLUT update is pending for any layer */
    if (0 == loc_WM_Window_UpdateAllCLUTs(Unit))
    {
        /* This API function <R_WM_Sys_DevFrameFinished> cannot return a value by design, so just report and continue */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
    }
    
    /* Check if we will have visible windows in the following frame */
    {
        uint32_t status = 0;
        
        for(layer = 0; layer <  R_WM_MAX_LAYERS; layer++)
        {
            if (r_wm_DevSys[Unit].HwLayerEnabled[layer] != 0)
            {
                status = 1;
            }
        }
        if (status != 0)
        {
            r_wm_DevSys[Unit].GlobalStatusFlags |= R_WM_SYS_GLOBAL_STATUS_FLAG_WINDOWS_VISIBLE;
        }
        else
        {
            r_wm_DevSys[Unit].GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_STATUS_FLAG_WINDOWS_VISIBLE;
        }
    }
}

/***********************************************************
  Function: R_WM_Sys_DevWaitForHwWriteReady
*/
void R_WM_Sys_DevWaitForHwWriteReady(uint32_t Unit)
{
#ifdef LOC_USE_OS
    /* wait for the SCANLINE interrupt */
    loc_WaitScanlineInt[Unit]++;

    /* Don't wait if a shutdown request flag is set */
    if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_SHUTDOWN) )
    {
        return;
    }
    R_OS_SemWait(&locSemaISRTrigger[Unit][R_WM_EVENT_SCANLINE]);
#else /* LOC_USE_OS */
    /* wait for the SCANLINE interrupt */
    loc_WaitScanlineInt[Unit] = 1;

    while (loc_WaitScanlineInt[Unit] == 1)
    {
    }
#endif /* LOC_USE_OS */
}

/***********************************************************
  Function: R_WM_Sys_DevWaitForHwUpdated
*/
void R_WM_Sys_DevWaitForHwUpdated(uint32_t Unit)
{
#ifdef LOC_USE_OS
    /* wait for the VBLANK interrupt */
    loc_WaitVblankInt[Unit]++;

    /* Don't wait if a shutdown request flag is set */
    if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_SHUTDOWN) )
    {
        return;
    }
    R_OS_SemWait(&locSemaISRTrigger[Unit][R_WM_EVENT_VBLANK]);
#else /* LOC_USE_OS */
    /* wait for the VBLANK interrupt */
    loc_WaitVblankInt[Unit] = 1;

    while (loc_WaitVblankInt[Unit] == 1)
    {
    }
#endif /* LOC_USE_OS */
}




/***********************************************************
  Group: WM Memory functions
*/


/***********************************************************
  Function: R_WM_Sys_Heap_Set
*/
void R_WM_Sys_Heap_Set(const void *Cpu, const void *Video)
{
    loc_CpuHeap_WM = (r_cdi_Heap_t*)Cpu;
    loc_VidHeap_WM = (r_cdi_Heap_t*)Video;
}

/***********************************************************
  Function: R_WM_Sys_Alloc
*/
void* R_WM_Sys_Alloc    (   uint32_t      Size,
                            r_wm_Memory_t MemType
                        )
{
    uint32_t addr;
    switch (MemType)
    {
    case R_WM_MEM_CPU:
        if (0 == loc_CpuHeap_WM)
        {
            addr = 0;
        }
        else
        {
            addr = (uint32_t)R_CDI_Alloc(loc_CpuHeap_WM, Size);
        }
    	break;
    case R_WM_MEM_VIDEO:
        if (0 == loc_VidHeap_WM)
        {
            addr = 0;
        }
        else
        {
            addr = (uint32_t)R_CDI_Alloc(loc_VidHeap_WM, Size);
        }
        break;
    default: 
        addr = 0;
        break;
    }
    return (void*)addr;
}


/***********************************************************
  Function: R_WM_Sys_Free
*/
uint32_t R_WM_Sys_Free  (   void*           Memory,
                            r_wm_Memory_t   MemType
                        )
{
    if (MemType == R_WM_MEM_CPU)
    {
        if (0 == loc_CpuHeap_WM)
        {
            R_WM_SYS_REPORT_ERROR(0xff, R_WM_ERR_MALLOC_FAILED, 0);
            return 0;
        }
        R_CDI_Free((uint32_t)Memory, loc_CpuHeap_WM);
    }
    else
    if (MemType == R_WM_MEM_VIDEO)
    {
        if (0 == loc_VidHeap_WM)
        {
            R_WM_SYS_REPORT_ERROR(0xff, R_WM_ERR_MALLOC_FAILED, 0);
            return 0;
        }
        R_CDI_Free((uint32_t)Memory, loc_VidHeap_WM);
    }
    else
    {
        R_WM_SYS_REPORT_ERROR(0xff, R_WM_ERR_MALLOC_FAILED, 0);
        return 0;
    }
    return 1;
}




/***********************************************************
  Group: WM OS interface
*/


/***********************************************************
  Function: R_WM_Sys_LockWindows
*/
void R_WM_Sys_LockWindows(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }

#ifdef LOC_USE_OS
    R_OS_ThreadMutexLock(&locMutexWindows[Unit]);
#endif
}


/***********************************************************
  Function: R_WM_Sys_UnlockWindows
*/
void R_WM_Sys_UnlockWindows(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }

#ifdef LOC_USE_OS
    R_OS_ThreadMutexUnlock(&locMutexWindows[Unit]);
#endif
}


/***********************************************************
  Function: R_WM_Sys_LockMsgQueue
*/
void R_WM_Sys_LockMsgQueue(uint32_t Unit)
{
#ifdef LOC_USE_OS
    R_OS_ThreadMutexLock(&locMutexMsgQueue[Unit]);
#endif
}

/***********************************************************
  Function: R_WM_Sys_LockMsgQueue
*/
uint32_t R_WM_Sys_TryLockMsgQueue(uint32_t Unit)
{
#ifdef LOC_USE_OS
    R_OS_ThreadMutexLock(&locMutexMsgQueue[Unit]);
#endif
    return 1;
}


/***********************************************************
  Function: R_WM_Sys_UnlockMsgQueue
*/
void R_WM_Sys_UnlockMsgQueue(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }
#ifdef LOC_USE_OS
    R_OS_ThreadMutexUnlock(&locMutexMsgQueue[Unit]);
#endif
}

/***********************************************************
  Function: R_WM_Sys_LockBuffers
*/
void R_WM_Sys_LockBuffers(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }

#ifdef LOC_USE_OS
    R_OS_ThreadMutexLock(&locMutexBufferMgmnt[Unit]);
#endif
}


/***********************************************************
  Function: R_WM_Sys_UnlockBuffers
*/
void R_WM_Sys_UnlockBuffers(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }

#ifdef LOC_USE_OS
    R_OS_ThreadMutexUnlock(&locMutexBufferMgmnt[Unit]);
#endif
}

/***********************************************************
  Function: R_WM_Sys_LockDevice
*/
void R_WM_Sys_LockDevice(uint32_t Unit)
{
     if (Unit >= R_WM_DEV_NUM)
     {
        R_WM_ErrorHandler(Unit, R_WM_ERR_INVALID_WM_UNIT);
        return;
     }

#ifdef LOC_USE_OS
    if (0 == locMutexInitCreated[Unit])
    {
        R_OS_ThreadMutexInit(&locMutexDevice[Unit], 0);
        locMutexInitCreated[Unit] = 1;
    }
    R_OS_ThreadMutexLock(&locMutexDevice[Unit]);
#endif
}


/***********************************************************
  Function: R_WM_Sys_UnlockDevice
*/
void R_WM_Sys_UnlockDevice(uint32_t Unit)
{
#ifdef LOC_USE_OS
    R_OS_ThreadMutexUnlock(&locMutexDevice[Unit]);
#endif
}




/***********************************************************
  Group: WM Messaging interface
*/


/***********************************************************
  Function: R_WM_Sys_MsgQueueSetup
*/
uint32_t R_WM_Sys_MsgQueueSetup(uint32_t Unit, void* MsgQueueStorage, uint32_t Size)
{
    return (R_CDI_RbSetup(&r_wm_DevSys[Unit].EvQueueRingBuf, MsgQueueStorage, Size, sizeof(r_wm_Msg_t)) == 0);
}

/***********************************************************
  Function: R_WM_Sys_MsgQueueRead
*/
uint32_t R_WM_Sys_MsgQueueRead(uint32_t Unit, r_wm_Msg_t* Msg)
{
    if (R_CDI_RbRead(&r_wm_DevSys[Unit].EvQueueRingBuf, Msg, 1) != 1)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
        return 0;
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_MsgQueueWrite
*/
uint32_t R_WM_Sys_MsgQueueWrite(uint32_t Unit, r_wm_Msg_t* Msg)
{
    if (R_CDI_RbWrite(&r_wm_DevSys[Unit].EvQueueRingBuf, (void*)Msg, 1) != 1)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NG, 0);
        return 0;
    }
    return 1;
}

