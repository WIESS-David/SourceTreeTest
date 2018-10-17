/*
****************************************************************************
PROJECT : WM
FILE    : $Id: r_sys_wm.c 8106 2016-03-24 15:02:00Z florian.zimmermann $
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
#include "r_spea_api.h"

#include "r_config_wm.h"
#include "r_wm_api.h"
#include "r_wm_sys.h"
#include "r_sys_wm.h"


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
  Macro: VDCECTL
  
  ITU 656 register.
*/
#define VDCECTL         0xFFC0601Cu

/***********************************************************
  Macro: VDCECTL_VISL1
  
  ITU 656 register.
*/

#define VDCECTL_VISL0   (1ul<<0)
#define VDCECTL_VISL1   (1ul<<1)
#define VDCECTL_PXSL    (1ul<<3)
#define VDCECTL_UPDT0   (1ul<<4)
#define VDCECTL_UPDT1   (1ul<<5)


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable : r_wm_DevSys
  
  Sys layer device object.
*/
extern r_wm_DevSys_t r_wm_DevSys[R_WM_DEV_NUM];

/***********************************************************
  Section: Local Function Declarations
*/

/***********************************************************
  Section: Local Variables
*/


/***********************************************************
  Variable : loc_capture1PinConfig
  
  Pin Configuration for ITU capturing on Video Input 1.
*/
static const r_dev_PinConfig_t loc_capture1PinConfig[] =   /* based on loc_D1M2H_ViItuCaputePinConfig */
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    
    /* VDCE1 Input ITU656 (from analog SAA71xx video converter) */
    {43, 2,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In Data7 */
    {43, 3,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 4,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 5,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 6,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, 
    {43, 7,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 8,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {43, 9,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In Data0 */ 
    {43, 10, 2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In CLK */
    
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1} 
};

/***********************************************************
  Variable : loc_capture0PinConfig
  
  Pin Configuration for ITU capturing on Video Input 0.
*/
static const r_dev_PinConfig_t loc_capture0PinConfig[] =
/*  Port Pin Func      Dir      Feedback     Pull        OpenDrain   HiCurr   HiSpeed  InputType */
{
    /* VDCE0 Input ITU656 (from analog SAA71xx video converter) */
/* ITU from SAA71xx video converter */
    {42, 0,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In Data7 */
    {42, 1,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 2,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 3,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 4,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},

    {42, 5,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 6,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1},
    {42, 7,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In Data0 */
    {42, 8,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In CLK */
#if 0 /* Not needed for ITU656. Enable, if you have VI0 using Sync signals! */
    {42, 9,  2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In HS - not needed for ITU656 */
    {42, 10, 2u, R_DEV_PIN_IN,  0u, R_DEV_PIN_PULLNO, 0u, 0u, 0u, R_DEV_PIN_CMOS1}, /* VDCE1 In VS - not needed for ITU656 */
#endif
    /* delimiter - do not remove */
    {0u, R_DEV_PIN_LAST,0u,R_DEV_PIN_OUT,0u,R_DEV_PIN_PULLNO,   0u, 0u, 0u, R_DEV_PIN_CMOS1}
};


/***********************************************************
  Section: Local Functions
*/




/***********************************************************
  Function: R_WM_Sys_DevRootCaptureSet
*/
void R_WM_Sys_DevRootCaptureSet(uint32_t Unit, r_wm_Capture_t* RootCapt)
{
    r_wm_DevSys[Unit].RootCapt = RootCapt;
}

/***********************************************************
  Function: R_WM_Sys_CaptureCreate
*/
uint32_t R_WM_Sys_CaptureCreate(uint32_t Unit, const r_wm_Capture_t* Capt)
{
    uint32_t val;
    r_vdce_Fbformat_t fb_fmt;
    uint32_t fb_bpp;
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair;
    uint32_t should_return;
    
    /*  need to reconf all the windows because the layer assignment needs to start from the layer 0 where the VIN can be output to 
        in case of the same VDCE channel for the VIN and VOUT
        TODO VDCE channels for VIN and VOUT are different
    */
    if (0 == loc_WM_WinReconfAll(Unit))
    {
        return 0;
    }
    
    win_layer_pair = loc_WM_WinLayerPairGet(Unit, Capt->Window);

    if (win_layer_pair == 0)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
#if 0
    /* check if scaling applicable */
    if  (   (win_layer_pair->Layer == R_VDCE_LAYER_SCALER0)
            ||
            (win_layer_pair->Layer == R_VDCE_LAYER_SCALER1)
        )
    {
        /* todo: with "Capt->CapUnit" as parameter only downscaling is allowed */
        if (Capt->ScaledWidth != 0)
        {
            if (0 == R_VDCE_LayerImgScaleX(Capt->CapUnit, win_layer_pair->Layer, Capt->ScaledWidth, R_VDCE_SCALING_SMALLER))
            {
                return 0;
            }
        }
        if (Capt->ScaledHeight != 0)
        {
            if (0 == R_VDCE_LayerImgScaleY(Capt->CapUnit, win_layer_pair->Layer, Capt->ScaledHeight, R_VDCE_SCALING_SMALLER))
            {
                return 0;
            }
        }
    }
#endif
    
    if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_ITU656) != 0)
    {
        /*  setup the HW for the ITU mode:
            select ITU656 (CLK + DATA7..0) for analog camera connected to SAA71xx
        */
        val = R_DEV_READ_REG(32, VDCECTL);
        if(Capt->CapUnit == 1)
        {
            val &= ~VDCECTL_VISL1;
        }
        else if(Capt->CapUnit == 0)
        {
            val &= ~VDCECTL_VISL0;
        }
        else
        {
            /* Not avialable */
        }
        val &= ~VDCECTL_PXSL;

        R_DEV_WRITE_REG(32, VDCECTL, ((val & 0x7Fu) | 0x10000u));
        if(Capt->CapUnit == 1)
        {
            R_DEV_PinInit(loc_capture1PinConfig);
        }
        else if(Capt->CapUnit == 0)
        {
            R_DEV_PinInit(loc_capture0PinConfig);
        }
        else
        {
            /* Not avialable */
        }
    }
    else
    {
        /*  setup the HW for the RGB888 mode:
        */
        val = R_DEV_READ_REG(32, VDCECTL);
        if(Capt->CapUnit == 1)
        {
            val |= VDCECTL_VISL1;
        }
        else if(Capt->CapUnit == 0)
        {
            val |= VDCECTL_VISL0;
        }
        else
        {
            /* Not avialable */
        }
        val &= ~VDCECTL_PXSL;

        R_DEV_WRITE_REG(32, VDCECTL, ((val & 0x7Fu) | 0x10000u));
    }
    
    should_return = 0;
    switch (Capt->Window->ColorFmt)
    {
    case R_WM_COLORFMT_RGB0888:
    case R_WM_COLORFMT_ARGB8888:
        fb_fmt = R_VDCE_FB_FORMAT_RGB0888;
        fb_bpp = 4;
        break;
    case R_WM_COLORFMT_RGB565:
        fb_fmt = R_VDCE_FB_FORMAT_RGB565;
        fb_bpp = 2;
        break;
    default:
        should_return = 1;
        break;
    }
    if (should_return != 0)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, 0);
        return 0;
    }

    err = R_VDCE_CapModeSet(Capt->CapUnit, (r_vdce_CapMode_t)((uint32_t)Capt->Mode & (~0xe0000000u)));    /* get rid of 3 WM internal MSBs that do not map to VDCE opts */
    if (err != R_VDCE_ERR_OK)
    {
        return 0;
    }
    
    if (0 == ((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_WEAVE))
    {
        /* BOB Deinterlacing */
        err = R_VDCE_CapBufGeometrySetup    (   Capt->CapUnit,
                                                (uint32_t)(Capt->Window->Surface.Fb.Buffer[0].Data),
                                                0,
                                                Capt->StrideX,
                                                Capt->Width,
                                                Capt->Height,
                                                Capt->StartX,
                                                Capt->StartY1,
                                                fb_fmt
                                            );
    }
    else
    {
        /* WEAVE Deinterlacing */
        if (Capt->Window->Surface.Fb.BufNum > 1)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_CAPTURE_CREATE_FAILED, 0);
            return 0;
        }

        /* Interlace issue:
        With ITU656 and the PAL/NTSC standard field 2 (bottom field) has an extra line in the blanking period therefore capture for this field must be aligned correctly with field 1 (top field)
        
                    interlaced view (PAL):
        
                    Top field    - blank line 22
                    Bottom field - blank line 23
                    Top field    - Video line 1
                    Bottom field - Video line 2
                    Top field    - Video line 3
                    Bottom field - Video line 4
        
        Since each field is captured independently and referenced from a line number we have (in memory)
        
                        (Address)                  (Address + 1)
                    Top field - blank line 22   Bottom field - blank 22
                    Top field - Video line 1    Bottom field - blank 23 (extra blank line for bottom field)
                    Top field - Video line 3    Bottom field - Video line 2 
                                                Bottom field - Video line 4
                    
        When viewed on a monitor we end up with a synchronisation issue..
        
                    Top field -         blank line 22
                    Bottom field -      blank line 22
                    Top field -         Video line 1
                    Bottom field -      blank line 23 (extra blank line for bottom field)
                    Top field -         Video line 3
                    Bottom field -      Video line 2
        
        A simple solution is to set the top field capture address to occur after the bottom field, effectively shifting the top field image to align correctly...
        
                    Bottom field -      blank line 22
                    Top field -         blank line 22
                    Bottom field -      blank line 23 (extra blank line for bottom field)
                    Top field -         Video line 1
                    Bottom field -      Video line 2
                    Top field -         Video line 3
        
        */
        err = R_VDCE_CapBufGeometrySetup    (   Capt->CapUnit,
                                                (uint32_t)(Capt->Window->Surface.Fb.Buffer[0].Data + (Capt->StrideX * fb_bpp) ),
                                                (uint32_t)(Capt->Window->Surface.Fb.Buffer[0].Data),
                                                Capt->StrideX * 2,
                                                Capt->Width,
                                                Capt->Height,
                                                Capt->StartX,
                                                Capt->StartY1,
                                                fb_fmt
                                            );
    }
    if (err != R_VDCE_ERR_OK)
    {
        return 0;
    }
    
    if (R_VDCE_ERR_OK != R_VDCE_LayerVSyncDelaySet(Unit, win_layer_pair->Layer, Capt->Delay))
    {
        return 0;
    }

    if (((uint32_t)Capt->Mode & (uint32_t)R_WM_CAPMODE_YUV_ITU656) != 0)
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerMatrixBT601Set(Unit, R_VDCE_LAYER_INPUT))
        {
            return 0;
        }
    }
    
    /* set the interrupt service routines */
    if (R_VDCE_ERR_OK != R_VDCE_IntcCallbackSet(Capt->CapUnit, R_VDCE_INTC_CAP_VBLANK, loc_WM_Isr[Unit][R_WM_EVENT_VI_VBLANK]))
    {
        return 0;
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_CaptureDelete
*/
uint32_t R_WM_Sys_CaptureDelete(uint32_t Unit, const r_wm_Capture_t* Capt)
{
    return R_WM_Sys_CaptureEnable(Unit, Capt, 0);
}

/***********************************************************
  Function: R_WM_Sys_CaptureEnable
*/
uint32_t R_WM_Sys_CaptureEnable(uint32_t Unit, const r_wm_Capture_t* Cap, uint32_t Enabled)
{
    if (Enabled != 0)
    {
        if (0 == (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE) )
        {
            if (R_VDCE_ERR_OK != R_VDCE_CapEnable(Cap->CapUnit, Unit))
            {
                return 0;
            }
            if (R_VDCE_ERR_OK != R_VDCE_IntcEnable(Cap->CapUnit, R_VDCE_INTC_CAP_VBLANK))
            {
                return 0;
            }
            r_wm_DevSys[Unit].GlobalStatusFlags |= R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE;
        }
    }
    else
    {
        if (0 != (r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE) )
        {
            if (R_VDCE_ERR_OK != R_VDCE_IntcDisable(Cap->CapUnit, R_VDCE_INTC_CAP_VBLANK))
            {
                return 0;
            }
            if (R_VDCE_ERR_OK != R_VDCE_CapDisable(Cap->CapUnit))
            {
                return 0;
            }
            r_wm_DevSys[Unit].GlobalStatusFlags &= ~R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE;
        }
    }
    return 1;
}



