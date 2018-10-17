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

extern r_wm_WinCapbs_t r_wm_DevCabps[ R_WM_MAX_LAYERS];

/***********************************************************
  Section: Local Function Declarations
*/

/***********************************************************
  Function: R_WM_Priv_WindowIsRleLayer
  
  External function reference to r_wm_priv.h / r_wm_main.c
*/
uint32_t R_WM_Priv_WindowIsRleLayer(const r_wm_Window_t  *Window);



static r_vdce_Format_t loc_GetVdceFormat(r_wm_WinColorFmt_t Format);
#if 0
static uint32_t loc_WinGetCount(uint32_t Unit);
#endif
static uint32_t loc_SetRle(uint32_t Unit, uint32_t Layer, r_wm_WinColorFmt_t Fmt, uint32_t Buf);
static uint32_t loc_CheckHWLayerCapabilities(uint32_t Unit, uint32_t Layer, const r_wm_Window_t* Win);

static uint32_t loc_WinSetViewportAndGeom  (   uint32_t            Unit,
                                        const r_wm_Window_t *Win,
                                        uint32_t            Layer,
                                        int32_t             PosX,
                                        int32_t             PosY,
                                        int32_t             Pitch,
                                        int32_t             Width,
                                        int32_t             Height,
                                        int32_t             Bpp
                                    );

static uint32_t loc_WinSetFb (  uint32_t            Unit,
                                const r_wm_Window_t *Win,
                                int32_t             PosX,
                                int32_t             PosY,
                                const void          *Fb
                        );
static const void* loc_WinGetOnScreenFb(const r_wm_Window_t *Win);
static const r_vdce_Error_t loc_VDCE_LayerEnable(uint32_t Unit, uint32_t Layer);
static const r_vdce_Error_t loc_VDCE_LayerDisable(uint32_t Unit, uint32_t Layer);
static uint32_t loc_WindowIsYUVLayer(const r_wm_Window_t *Window);
static r_wm_WinCapbs_t loc_WM_Sys_WindowCapabilitiesGet ( uint32_t Unit, uint32_t Layer );

/***********************************************************
  Section: Local Variables
*/




/***********************************************************
  Section: Local Functions
*/


/***********************************************************
  Function: loc_GetVdceFormat
*/
static r_vdce_Format_t loc_GetVdceFormat(r_wm_WinColorFmt_t Format)
{
    r_vdce_Format_t  vdce_format;

    switch (Format) {
    case R_WM_COLORFMT_RGB565:
        vdce_format = R_VDCE_RGB565;
        break;
    case R_WM_COLORFMT_ARGB1555:
        vdce_format = R_VDCE_ARGB1555;
        break;
    case R_WM_COLORFMT_RGBA5551:
        vdce_format = R_VDCE_RGBA5551;
        break;
    case R_WM_COLORFMT_RGB0444:
    case R_WM_COLORFMT_ARGB4444:
        vdce_format = R_VDCE_ARGB4444;
        break;
    case R_WM_COLORFMT_RGB0888:
    case R_WM_COLORFMT_RLE24RGB0888:
        vdce_format = R_VDCE_RGB0888;
        break;
    case R_WM_COLORFMT_ARGB8888:
    case R_WM_COLORFMT_RLE24ARGB8888:
        vdce_format = R_VDCE_ARGB8888; 
        break;
    case R_WM_COLORFMT_RGBA8888:
       vdce_format = R_VDCE_RGBA8888;
       break;
    case R_WM_COLORFMT_CLUT8:
        vdce_format = R_VDCE_CLUT8;
        break;
    case R_WM_COLORFMT_CLUT4:
        vdce_format = R_VDCE_CLUT4;
        break;
    case R_WM_COLORFMT_CLUT1:
        vdce_format = R_VDCE_CLUT1;
        break;
    case R_WM_COLORFMT_YCBCR_422:
        vdce_format = R_VDCE_YCBCR_422;
        break;
    case R_WM_COLORFMT_YCBCR_444:
        vdce_format = R_VDCE_YCBCR_444;
        break;
    case R_WM_COLORFMT_YUV_YUYV:
        vdce_format = R_VDCE_YUV_YUYV;
        break;
    case R_WM_COLORFMT_YUV_UYVY:
        vdce_format = R_VDCE_YUV_UYVY;
        break;
    case R_WM_COLORFMT_YUV_YVYU:
        vdce_format = R_VDCE_YUV_YVYU;
        break;
    case R_WM_COLORFMT_YUV_VYUY:
        vdce_format = R_VDCE_YUV_VYUY;
        break;
    default:
        vdce_format = R_VDCE_FORMAT_LAST;
        break;
    }
    return vdce_format;
}

/***********************************************************
  Function: loc_WindowIsYUVLayer
*/
static uint32_t loc_WindowIsYUVLayer(const r_wm_Window_t *Window)
{
    uint32_t ret_val;
    switch (Window->ColorFmt)
    {
        case R_WM_COLORFMT_YUV_YUYV:
        case R_WM_COLORFMT_YUV_UYVY:
        case R_WM_COLORFMT_YUV_YVYU:
        case R_WM_COLORFMT_YUV_VYUY:
        case R_WM_COLORFMT_YCBCR_422:
        case R_WM_COLORFMT_YCBCR_444:
            ret_val = 1;
            break;
        default:
            ret_val = 0;
            break;
    }
    return ret_val;
}

#if 0
/***********************************************************
  Function: loc_WinGetCount
*/
static uint32_t loc_WinGetCount(uint32_t Unit)
{
    uint32_t n = 0;
    r_wm_Window_t* cur_win = r_wm_DevSys[Unit].RootWin;

    /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
       elements for this device family is R_WM_MAX_LAYERS. */
    while ( (cur_win != R_NULL) && (n <= R_WM_MAX_LAYERS) )
    {
        cur_win = cur_win->Next;
        ++n;
    }
    return n;
}
#endif

/***********************************************************
  Function: loc_WM_WinLayerPairGet
*/
r_wm_sys_vdce_WinLayerPair_t* loc_WM_WinLayerPairGet(uint32_t Unit, const r_wm_Window_t* Win)
{
    uint32_t i;
    for (i = 0; i <  R_WM_MAX_LAYERS; ++i)
    {
        if (r_wm_DevSys[Unit].WinToLayerMap[i].Win == Win)
        {
            return &r_wm_DevSys[Unit].WinToLayerMap[i];
        }
    }
    return R_NULL;
}

/***********************************************************
  Function: loc_WinLayerPairGetByLayer
  
  Return a WinLayerPair based on the given Layer.
  Return NULL, if layer has no window.
*/
r_wm_sys_vdce_WinLayerPair_t* loc_WinLayerPairGetByLayer(uint32_t Unit, const uint32_t Layer)
{
    uint32_t i;
    for (i = 0; i <  R_WM_MAX_LAYERS; ++i)
    {
        if (r_wm_DevSys[Unit].WinToLayerMap[i].Layer == Layer)
        {
            return &r_wm_DevSys[Unit].WinToLayerMap[i];
        }
    }
    return R_NULL;
}

/***********************************************************
  Function: loc_SetRle
*/
static uint32_t loc_SetRle(uint32_t Unit, uint32_t Layer, r_wm_WinColorFmt_t Fmt, uint32_t Buf)
{
    r_spea_RleConfig_t rle_conf;
    uint32_t err_exit = 0;
    r_spea_Unit_t rle_unit;
    switch (Fmt)
    {
        case R_WM_COLORFMT_RLE24ARGB8888:
        case R_WM_COLORFMT_RLE24RGB0888:
            rle_conf.ColorMode = R_SPEA_CM_24BPP;
            break;
        case R_WM_COLORFMT_RLE18ARGB8888:
        case R_WM_COLORFMT_RLE18RGB0888:
            rle_conf.ColorMode = R_SPEA_CM_18BPP;
            break;
        case R_WM_COLORFMT_RLE8CLUT8:
        case R_WM_COLORFMT_RLE8CLUT4:
        case R_WM_COLORFMT_RLE8CLUT1:
            rle_conf.ColorMode = R_SPEA_CM_8BPP;
            break;
        default:
            err_exit = 1;
            rle_conf.ColorMode = R_SPEA_CM_24BPP;
            break;
    }
    if (err_exit == 1)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_COLORFMT, 0);
        return 0;   /* R_WM_ERR_COLORFMT */
    }

    /* Both RLE units have VideoOutID 0 set as default in HW.
       R_SPEA_SetRle is not allowed to set the same VideoOutID
       as the other unit already has. Furthermore, R_SPEA_UnitEnable
       starts both units, so if the other unit operation is not desired,
       its VideoOutID needs to be set to some invalid value.
    */
    
    /* check if the other unit is already in use,
       if not, it needs to be setup with an invalid VideoOutID,
       if yes, than it already has a (valid) different VideoOutID
    */
    rle_unit = loc_WM_Sprite_GetRLEUnitFromLayerNo(Unit, Layer);
    if  ((r_wm_DevSys[Unit ^ 1u].GlobalStatusFlags & (R_WM_SYS_GLOBAL_STATUS_FLAG_RLE0<<rle_unit)) == 0uL)
    {
        /* some foo defaults */
        rle_conf.ColorMode = R_SPEA_CM_24BPP;
        rle_conf.VirtStart = 0x30000000u;
        rle_conf.MemStart = 0xffffffffu;
        rle_conf.VideoOutID = 0xff; /* an invalid Id */
        if (R_SPEA_ERR_OK != R_SPEA_SetRle(rle_unit, Unit ^ 1u, &rle_conf))
        {
            return 0;
        }
    }
   
    
    rle_conf.VirtStart = 0x30000000u;
    rle_conf.MemStart = Buf;
    rle_conf.VideoOutID = Unit;
    if (R_SPEA_ERR_OK != R_SPEA_SetRle(rle_unit, Unit, &rle_conf))
    {
        return 0;
    }
    
    /*  R_SPEA_UnitEnable actually turns both units on, but by supplying the
        invalid VideoOutID for the one we don't need, it is effectively 'turned off'
    */
    if (R_SPEA_ERR_OK != R_SPEA_UnitEnable(rle_unit, rle_unit, 1))
    {
        return 0;
    }

    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_RLEn_UPDATE_BIT */
    loc_WM_Sprite_SetUpdateFlag(Unit, rle_unit);

    /* set the flag that the unit is in use */
    r_wm_DevSys[Unit].GlobalStatusFlags |= (R_WM_SYS_GLOBAL_STATUS_FLAG_RLE0<<rle_unit);
    
    return 1;
}

/***********************************************************
  Function: loc_CheckHWLayerCapabilities
*/
static uint32_t loc_CheckHWLayerCapabilities(uint32_t Unit, uint32_t Layer, const r_wm_Window_t* Win)
{
    r_wm_Capture_t * capt;
    uint32_t         n_capt;
    
    /* Parameter checks */
    if ( ( Unit >= R_WM_DEV_NUM ) || ( Layer >=  R_WM_MAX_LAYERS ) || ( 0 == Win ) )
    {
        return 0;
    }
    
    /* Only configured layers (D1MA) / Only layer 0 (Other Devices) may be an RLE layer */
    if ( 0 != R_WM_Priv_WindowIsRleLayer(Win) )
    {
        if ( R_WM_WINCAPBS_RLE != loc_WM_Sys_WindowCapabilitiesGet(Unit, Layer) )
        {
            return 0;
        }
    }
    
    /* Only layer 0 and 1 may be YUV layers */
    if ( ( 0 != loc_WindowIsYUVLayer(Win) ) && ( Layer > 1 ) )
    {
        return 0;
    }
    
    /* Only configured layers (D1MA) / Only layer 0 (Other Devices) may be a Sprite layer */
    if ( Win->Mode == R_WM_WINMODE_SPRITES )
    {
        if ( R_WM_WINCAPBS_SPRITES != loc_WM_Sys_WindowCapabilitiesGet(Unit, Layer) )
        {
            return 0;
        }
    }
    
    /* Special Case Layer 0 */
    if ( Layer == 0 )
    {
        /* Layer 0 can't do alpha blending by constant alpha */
        if ( Win->Alpha != 0xFFu )
        {
            return 0;
        }
    
        /* Layer 0 can't do alpha blending by pixel alpha */
        /* Due to legacy reasons, this will not be evaluated here. 
           Textures using alpha channel that are placed on layer 0 will ignore alpha. */
        /* If alpha of RGBA should always be evaulated, workaround on API level is 
           to define dummy layer and place it on lowest z-Index but don't enable it. */
    
        /* Layer 0 can't do alpha blending by CLUT alpha */
        if (    ( Win->ColorFmt == R_WM_COLORFMT_CLUT8 )
             || ( Win->ColorFmt == R_WM_COLORFMT_CLUT4 )
             || ( Win->ColorFmt == R_WM_COLORFMT_CLUT1 ) )
        {
            uint32_t i;
            for ( i=0; i<Win->ClutNumEntries; i++ )
            {
                if ( Win->Clut[i].A != 0xFF )
                {
                    /* Found (semi-)transparent pixel, this is not compatible with layer 0 */
                    return 0;
                }
            }
        }
        
        /* Layer 0 can't do color keying */
        if ( Win->ColorKey.Enabled == 1 ) 
        {
            return 0;
        }

    }

    /* If current window is used for capturing, check layer assignment */
    capt = r_wm_DevSys[Unit].RootCapt;
    n_capt = 0;
    /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
       elements for this device family is R_WM_MAX_CAPTURE_LAYERS. */
    while ( (capt != 0) && (n_capt <= R_WM_MAX_CAPTURE_LAYERS) )
    {
        if (capt->Window == Win)
        {
            /* VIn->VOn (e.g. VI0->VO0) can only be placed on L0 */
            if ( (capt->CapUnit == Unit) && (Layer != 0) )
            {
                return 0;
            }
            /* VIk->VOn (e.g. VI1->VO0) can only be placed on L1 */
            if ( (capt->CapUnit != Unit) && (Layer != 1) )
            {
                return 0;
            }
        }
        n_capt++;
        capt = capt->Next;
    }
    
    return 1;
}

/***********************************************************
  Function: loc_WM_Window_UpdateAllCLUTs
*/
uint32_t loc_WM_Window_UpdateAllCLUTs(uint32_t Unit)
{
    uint32_t layer;

    /* Check if a CLUT update is pending for any layer */
    if ((r_wm_DevSys[Unit].FrameStatusFlags & R_WM_SYS_FRAME_STATUS_LAYER_CLUT_UPDATE_MASK) != 0)
    {
        for (layer = 0; layer <  R_WM_MAX_LAYERS; layer++)
        {
            /* Read respective R_WM_SYS_FRAME_STATUS_LAYER_0_CLUT_UPDATE_BIT */
            if ((r_wm_DevSys[Unit].FrameStatusFlags & (1uL << (R_WM_SYS_FRAME_STATUS_LAYER_0_CLUT_UPDATE_BIT+layer))) != 0)
            {
                r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WinLayerPairGetByLayer(Unit, layer);
                
                if (0 < win_layer_pair->Win->ClutNumEntries)
                {
                    /* -Disable Layer
                       -Update CLUT
                       -Enable Layer
                     */
                    if (1 == r_wm_DevSys[Unit].HwLayerEnabled[layer])
                    {
                        if (R_VDCE_ERR_OK != R_VDCE_LayerDisable(Unit, layer) )
                        {
                            return 0;
                        }
                    }
                    if (R_VDCE_ERR_OK != R_VDCE_LayerClutSet(Unit, layer, 0, win_layer_pair->Win->ClutNumEntries, (r_vdce_ClutARGB32_t*)(win_layer_pair->Win->Clut)))
                    {
                        return 0;
                    }
                    if (1 == r_wm_DevSys[Unit].HwLayerEnabled[layer])
                    {
                        if (R_VDCE_ERR_OK != R_VDCE_LayerEnable(Unit, layer) )
                        {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}



/***********************************************************
  Function: loc_WM_WinReconfAll
*/
uint32_t loc_WM_WinReconfAll(uint32_t Unit)
{
    uint32_t slot;
    uint32_t layer;
    const r_wm_Window_t* cur_win;
    r_wm_Window_t* RootWin = r_wm_DevSys[Unit].RootWin;
    r_vdce_CkARGB32_t ck_in;
    r_vdce_CkARGB32_t ck_out;
    r_vdce_Error_t vdce_err;
    const void* fb;
    
    r_wm_DevSys[Unit].FrameStatusFlags = 0;
    
    layer = 0;
    slot = 0;
    cur_win = RootWin;

    /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
       elements for this device family is R_WM_MAX_LAYERS. */
    while ( (cur_win != R_NULL) && (layer < R_WM_MAX_LAYERS) )
    {
        /* Find first layer to match requested HW features */
        while (0 == loc_CheckHWLayerCapabilities(Unit, layer, cur_win) )
        {
            loc_VDCE_LayerDisable(Unit, layer);
            layer++;
            
            /* If requested HW features cannot be mapped, abort! 
               Feature oriented layer mapping is user scope. */
            if(layer >=  R_WM_MAX_LAYERS)
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, 0);
                return 0;
            }
        }
        
        r_wm_DevSys[Unit].WinToLayerMap[slot].Win = cur_win; 
        r_wm_DevSys[Unit].WinToLayerMap[slot].Layer = layer;
        loc_VDCE_LayerDisable(Unit, layer);
        
        /* chroma key */
        if (cur_win->ColorKey.Enabled != 0)
        {
            ck_in.R = cur_win->ColorKey.In.RgbKey.Red;
            ck_in.G = cur_win->ColorKey.In.RgbKey.Green;
            ck_in.B = cur_win->ColorKey.In.RgbKey.Blue;
            ck_in.A = 0x00;
            
            ck_out.R = cur_win->ColorKey.Out.Red;
            ck_out.G = cur_win->ColorKey.Out.Green;
            ck_out.B = cur_win->ColorKey.Out.Blue;
            ck_out.A = cur_win->ColorKey.Out.Alpha;
            
            /* Layer Alpha and Chroma Keying are exclusive. 
               Chroma Keying overrides Layer Alpha. */
            if((0 < layer) && (layer < 4))
            {
                if (R_VDCE_ERR_OK != R_VDCE_LayerAlphaConstDisable(Unit,layer))
                {
                    return 0;
                }
            }
            if (R_VDCE_ERR_OK != R_VDCE_LayerChromaKeyEnable(Unit, layer, &ck_in, &ck_out))
            {
                return 0;
            }
        }
        
        if (R_VDCE_ERR_OK != R_VDCE_LayerFormatSet(Unit, layer, loc_GetVdceFormat(cur_win->ColorFmt)))
        {
            return 0;
        }
        if (0 < cur_win->ClutNumEntries)
        {
            r_wm_DevSys[Unit].FrameStatusFlags |= (1uL << (R_WM_SYS_FRAME_STATUS_LAYER_0_CLUT_UPDATE_BIT+layer));
        }
		
		/* update the scaling */
        if  (   (layer == R_VDCE_LAYER_SCALER0)
            ||
            (layer == R_VDCE_LAYER_SCALER1)
        )
        {
            /* capture scaling */
            if (r_wm_DevSys[Unit].RootCapt->Window == cur_win)
            {
                if (r_wm_DevSys[Unit].RootCapt->ScaledWidth != 0)
                {
                    if (0 == R_VDCE_LayerImgScaleX(
                                    Unit /*r_wm_DevSys[Unit].RootCapt->CapUnit */, 
                                    Unit ^ r_wm_DevSys[Unit].RootCapt->CapUnit, 
                                    r_wm_DevSys[Unit].RootCapt->ScaledWidth, 
                                    R_VDCE_SCALING_SMALLER))
                    {
                        return 0;
                    }
                }
                if (r_wm_DevSys[Unit].RootCapt->ScaledHeight != 0)
                {
                    if (0 == R_VDCE_LayerImgScaleY(
                                    Unit /*r_wm_DevSys[Unit].RootCapt->CapUnit*/, 
                                    Unit ^ r_wm_DevSys[Unit].RootCapt->CapUnit, 
                                    r_wm_DevSys[Unit].RootCapt->ScaledHeight, 
                                    R_VDCE_SCALING_SMALLER))
                    {
                        return 0;
                    }
                }
            }
            else /* window scaling */
            {
                /* Scaling */
                /* Layer 0 & 1 only when scaling is enabled */
                if( (layer > 1) && ((cur_win->ScaledWidth != 0) || (cur_win->ScaledHeight != 0)) )
                {
                    /* error, Layer 0 & 1 only*/
                    return 0;
                }
                else
                {
                    /* Width */
                    /* ScaledWidth of 0 means disabled */
                    if( cur_win->ScaledWidth != 0)
                    {
                        if( cur_win->ScaledWidth < cur_win->Width) /* scale up */
                        {
                            if (cur_win->ScaledWidth != R_VDCE_LayerImgScaleX(Unit, layer, cur_win->ScaledWidth, R_VDCE_SCALING_LARGER))
                            {
                                return 0;
                            }
                        }
                        else if( cur_win->ScaledWidth > cur_win->Width) /* scale down */
                        {
                            if (cur_win->ScaledWidth != R_VDCE_LayerImgScaleX(Unit, layer, cur_win->ScaledWidth, R_VDCE_SCALING_SMALLER))
                            {
                                return 0;
                            }
                        }
                        else
                        {
                            /* nothing */
                        }
                    }
                    else /* no scaling */
                    {
                        if(layer > 1)
                        {
                            /* nothing */
                        }
                        else
                        {
                            R_VDCE_LayerImgScaleX(Unit, layer, cur_win->Width, R_VDCE_SCALING_DISABLE);
                        }
                    }
            
                    /* Scaling */
                    /* Height */
                    /* ScaledHeight of 0 means disabled */
                    if( cur_win->ScaledHeight != 0)
                    {
                        if( cur_win->ScaledHeight < cur_win->Height) /* scale up */
                        {
                            if (cur_win->ScaledHeight != R_VDCE_LayerImgScaleY(Unit, layer, cur_win->ScaledHeight, R_VDCE_SCALING_LARGER))
                            {
                                return 0;
                            }
                        }
                        else if( cur_win->ScaledHeight > cur_win->Height) /* scale down */
                        {
                            if (cur_win->ScaledHeight != R_VDCE_LayerImgScaleY(Unit, layer, cur_win->ScaledHeight, R_VDCE_SCALING_SMALLER))
                            {
                                return 0;
                            }
                        }
                        else
                        {
                            /* nothing */
                        }
                    }
                    else /* no scaling */
                    {
                        if(layer > 1)
                        {
                            /* nothing */
                        }
                        else
                        {
                            R_VDCE_LayerImgScaleY(Unit, layer, cur_win->Height, R_VDCE_SCALING_DISABLE);
                        }
                    }
                }
            }
        }

        /* Layer 0 and Layer 4 cannot be assigned constant Alpha */
        if((0 < layer) && (layer < 4) && (cur_win->ColorKey.Enabled == 0))
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerAlphaConstEnable(Unit, layer, cur_win->Alpha))
            {
                return 0;
            }
        }
        if((0 < layer) && (layer < 4))
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerAlphaChannelEnable(Unit, layer))
            {
                return 0;
            }
            
            if (R_FALSE != cur_win->UsePremultipliedAlpha)
            {
                vdce_err = R_VDCE_LayerPremultipliedAlphaEnable(Unit, layer);
            }
            else
            {
                vdce_err = R_VDCE_LayerPremultipliedAlphaDisable(Unit, layer);
            }
            if (vdce_err != R_VDCE_ERR_OK)
            {
                return 0;
            }
        }
        
        /* Set vertical mirroring */
        /* Enable vertical mirroring */
        if ( 0 != ( ((uint32_t)cur_win->Flags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR) ) )
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerModeSet(Unit, layer, R_VDCE_LAYER_MODE_V_MIRORING) )
            {
                return 0;
            }
        }
        /* Disable vertical mirroring */
        else
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerModeSet(Unit, layer, R_VDCE_LAYER_MODE_NONE) )
            {
                return 0;
            }
        }

        
        /* set the layer base address from the ON_SCREEN fb */
        if (cur_win->Mode != R_WM_WINMODE_SPRITES)
        {
            fb = loc_WinGetOnScreenFb(cur_win);
            if (fb != 0)
            {
                if (0 == loc_WinSetFb(Unit, cur_win, cur_win->PosX, cur_win->PosY, fb))
                {
                    return 0;
                }
            }
        }
        else
        {
            if (0 == loc_WinSetFb(Unit, cur_win, cur_win->PosX, cur_win->PosY, 0 /* will be set in the function */))
            {
                return 0;
            }
            if (cur_win->Surface.SpritesRoot != 0)
            {
                r_spea_Unit_t spea_unit;
                
                spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, layer);
                loc_WM_Sprite_SetReassignFlag(Unit, spea_unit);
            }
        }
        /* This will also enable the layer */
        if (0 == loc_WinSetViewportAndGeom( Unit,
                                            cur_win,
                                            layer,
                                            cur_win->PosX,
                                            cur_win->PosY,
                                            cur_win->Pitch,
                                            cur_win->Width,
                                            cur_win->Height,
                                            R_WM_ColorFmtBitsPerPixGet(cur_win->ColorFmt) ))
        {
            return 0;
        }

        cur_win = cur_win->Next;
        ++layer;
        ++slot;
    }
    
    /* Turn off remaining layers */
    for (; slot <  R_WM_MAX_LAYERS; ++slot)
    {
        r_wm_DevSys[Unit].WinToLayerMap[slot].Win = R_NULL; 
        r_wm_DevSys[Unit].WinToLayerMap[slot].Layer = LOC_INVALID_LAYER;
    }
    for (; layer <  R_WM_MAX_LAYERS; ++layer)
    {
        loc_VDCE_LayerDisable(Unit, layer);
    }
    return 1;
}

/***********************************************************
  Function: loc_WinSetViewportAndGeom
*/
static uint32_t loc_WinSetViewportAndGeom ( uint32_t     Unit,
                                            const r_wm_Window_t *Win,
                                            uint32_t            Layer,
                                            int32_t             PosX,
                                            int32_t             PosY,
                                            int32_t             Pitch,
                                            int32_t             Width,
                                            int32_t             Height,
                                            int32_t             Bpp
                                          )
{
    r_vdce_Error_t err;
    int32_t pix_per_align_step = (128 / Bpp) * 8;
    if (PosY < 0)
    {
        Height += PosY; /* shorten the height */
        PosY = 0;
    }
    if (PosX < 0)
    {
        /* Instead of directly adjusting the layer's position on the screen, 
           it is also possible to change the base address of the layer.
           By increasing the base address you effectively move the layer to the left.
           
           A crucial point of this method is, the VDCE's Layer Base Adress must 
           be 128Byte aligned. This is 32px for 4BPP, 64px for 2BPP, etc...
           So this method works best, if there is a reserve of 32px, 64px, etc 
           in the blanking width to be used with conventional moving.
           
           As a consequence, moving by changing the base address can only be used 
           when the reserve is equal or bigger than 32px, 64px, etc..
        */
        if ( (0 > (r_wm_DevSys[Unit].HorizontalMoveReserve + PosX)) && 
             (pix_per_align_step>r_wm_DevSys[Unit].HorizontalMoveReserve) )
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_WIN_MOVE_FAILED, 0);
            return 0;
        }
        /* Shorten the width aligned to 128Byte steps */
        Width -= (int32_t)((uint32_t)-PosX & ~(uint32_t)(pix_per_align_step - 1u));
        PosX = PosX % pix_per_align_step;
    }

    if((Width > 0)&&(Height > 0))
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerMemGeometrySet(Unit, Layer, Pitch, Height))
        {
            return 0;
        }
        if (R_VDCE_ERR_OK != R_VDCE_LayerViewPortSet(Unit, Layer, 0, 0, PosX, PosY, Width, Height))
        {
            return 0;
        }
    }

    if (Win->Status == R_WM_WINSTATUS_ENABLED)
    {
        if((Width <= 0)||(Height <= 0))
        {
            err = loc_VDCE_LayerDisable(Unit, Layer);
        }
        else
        {
            err = loc_VDCE_LayerEnable(Unit, Layer);
        }
        if (R_VDCE_ERR_OK != err)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, err);
            return 0;
        }
    }
    return 1;
}

/***********************************************************
  Function: loc_WinSetFb
*/
static uint32_t loc_WinSetFb   (   uint32_t            Unit,
                            const r_wm_Window_t *Win,
                            int32_t             PosX,
                            int32_t             PosY,
                            const void          *Fb
                        )
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    uint32_t Bpp = R_WM_ColorFmtBitsPerPixGet(Win->ColorFmt);
    int32_t add_h = 0;
    int32_t add_v = 0;
    if (Bpp == 0)
    {
        Bpp = 1;
    }
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, 0);
        return 0;
    }

    if (R_WM_Priv_WindowIsRleLayer(Win) == 1)
    {
        /* Only Certain Windows (D1M1A) / only the Root window (Other Devices) may be an RLE window */
        if ( R_WM_WINCAPBS_RLE != loc_WM_Sys_WindowCapabilitiesGet ( Unit, win_layer_pair->Layer ) )
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, 0);
            return 0; /* R_WM_ERR_NO_PHYS_WINDOW */
        }

        /* RLE Buffers must be 128-byte aligned */
        if (0 != ((uint32_t)Fb&0x0000007Fu) )
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_ADDRESS_ALIGNMENT, 0);
            return 0; /* R_WM_ERR_ADDRESS_ALIGNMENT */
        }

        if (0 == loc_SetRle(Unit, win_layer_pair->Layer, Win->ColorFmt, (uint32_t)Fb))
        {
            return 0;
        }

        /* set the address of the virtual layer */
        Fb = (void*)0x30000000u;
    }
    else if (Win->Mode == R_WM_WINMODE_SPRITES)
    {
        /* set the address of the virtual layer */
        Fb = (void*)0x30000000u;
    }
    else
    {
    }
    
    /* check if the negative start position workaround is needed */
    if (R_WM_Priv_WindowIsRleLayer(Win) == 0)
    {
        if  (PosY < 0)
        {
            /* Move the Window up out of the screen by changing the start address stribe-by-stride */
            add_v = -PosY;
        }
        if  (PosX < 0)
        {
            /* Instead of directly adjusting the layer's position on the screen, 
               it is also possible to change the base address of the layer.
               By increasing the base address you effectively move the layer to the left.
               
               A crucial point of this method is, the VDCE's Layer Base Adress must 
               be 128Byte aligned. This is 32px for 4BPP, 64px for 2BPP, etc...
               So this method works best, if there is a reserve of 32px, 64px, etc 
               in the blanking width to be used with conventional moving.
               
               As a consequence, moving by changing the base address can only be used 
               when the reserve is equal or bigger than 32px, 64px, etc..
            */
            int32_t pix_per_align_step = (128 / Bpp) * 8;
            
            /* If PosX exceeds conventional moving range, check if requested PosX can be served by address shifting */
            if ( (0 > (r_wm_DevSys[Unit].HorizontalMoveReserve + PosX)) && 
                 (pix_per_align_step>r_wm_DevSys[Unit].HorizontalMoveReserve) )
            {
                R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_WIN_MOVE_FAILED, 0);
                return 0;
            }
            
            /* Increase address aligned to 128Byte steps */
            add_h = (int32_t)((uint32_t)(-PosX) & ~(uint32_t)(pix_per_align_step - 1u));
        }
        Fb = Fb +  (((add_h + (add_v * Win->Pitch)) * Bpp) / 8);
    }
    
    if (R_VDCE_ERR_OK != R_VDCE_LayerBaseSet(Unit, win_layer_pair->Layer, (uint32_t)Fb))
    {
        return 0;
    }
    return 1;
}

/***********************************************************
  Function: loc_WinGetOnScreenFb
*/
static const void* loc_WinGetOnScreenFb(const r_wm_Window_t *Win)
{
    uint32_t i;
    for (i = 0; i < Win->Surface.Fb.BufNum; ++i)
    {
        if (Win->Surface.Fb.Buffer[i].Status == R_WM_WINBUF_ON_SCREEN)
        {
            return Win->Surface.Fb.Buffer[i].Data;
        }
    }
    return 0;
}


/***********************************************************
  Function: loc_VDCE_LayerEnable
*/
static const r_vdce_Error_t loc_VDCE_LayerEnable(uint32_t Unit, uint32_t Layer)
{
    r_vdce_Error_t vdce_err = R_VDCE_ERR_OK;
    if (0 == r_wm_DevSys[Unit].HwLayerEnabled[Layer])
    {
        vdce_err = R_VDCE_LayerEnable(Unit, Layer);
        
        if (R_VDCE_ERR_OK == vdce_err)
        {
            r_wm_DevSys[Unit].HwLayerEnabled[Layer] = 1;
        }
    }
    
    return vdce_err;
}

/***********************************************************
  Function: loc_VDCE_LayerDisable
*/
static const r_vdce_Error_t loc_VDCE_LayerDisable(uint32_t Unit, uint32_t Layer)
{
    r_vdce_Error_t vdce_err = R_VDCE_ERR_OK;
    if (1 == r_wm_DevSys[Unit].HwLayerEnabled[Layer])
    {
        vdce_err = R_VDCE_LayerDisable(Unit, Layer);

        if (R_VDCE_ERR_OK == vdce_err)
        {
            r_wm_DevSys[Unit].HwLayerEnabled[Layer] = 0;
        }
    }
    
    return vdce_err;
}




/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/


/***********************************************************
  Group: WM Basic interface functions
*/


   
/***********************************************************
  Function: R_WM_Sys_DevRootWinSet
*/
void R_WM_Sys_DevRootWindowSet(uint32_t Unit, r_wm_Window_t* RootWin)
{
    r_wm_DevSys[Unit].RootWin = RootWin;
}


/***********************************************************
  Group: WM Video Output Window interface
*/


/***********************************************************
  Function: R_WM_Sys_WindowSetFb
*/
uint32_t R_WM_Sys_WindowSetFb(uint32_t Unit, const r_wm_Window_t* Win, const void* Fb)
{
    return loc_WinSetFb(Unit, Win, Win->PosX, Win->PosY, Fb);
}

/***********************************************************
  Function: R_WM_Sys_WindowCapabilitiesSet
*/
uint32_t R_WM_Sys_WindowCapabilitiesSet (   r_wm_WinCapbs_t Capability0,
                                            r_wm_WinCapbs_t Capability1,
                                            r_wm_WinCapbs_t Capability2,
                                            r_wm_WinCapbs_t Capability3
                                         )
{
    r_wm_DevCabps[0] = Capability0;
    r_wm_DevCabps[1] = Capability1;
    r_wm_DevCabps[2] = Capability2;
    r_wm_DevCabps[3] = Capability3;
    
    if (R_SPEA_ERR_OK != R_SPEA_UnitCapabilitiesSet (
        ( (Capability0 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE ),
        ( (Capability1 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE ),
        ( (Capability2 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE ),
        ( (Capability3 == R_WM_WINCAPBS_RLE) ? R_SPEA_SPEACAPBS_RLE : R_SPEA_SPEACAPBS_SPRITE ) ) )
    {
        return 0;
    }
    
    return 1;
}

static r_wm_WinCapbs_t loc_WM_Sys_WindowCapabilitiesGet ( uint32_t Unit, uint32_t Layer )
{
    r_wm_WinCapbs_t retval;
    
    if (0 == Unit)
    {
        /* Linear mapping for Unit 0 */
        retval = r_wm_DevCabps[Layer];
    }
    else
    {
        /* Different mapping for Unit 1 */
        switch(Layer) {
        case 0:
            retval = r_wm_DevCabps[0];
            break;
        case 1:
            retval = r_wm_DevCabps[3];
            break;
        case 2:
            retval = r_wm_DevCabps[2];
            break;
        case 3:
            retval = r_wm_DevCabps[1];
            break;
        default:
            break;
        }
    }
    return retval;
}

/***********************************************************
  Function: R_WM_Sys_WindowCreate
*/
uint32_t R_WM_Sys_WindowCreate(uint32_t Unit, const r_wm_Window_t* Win)
{
    return loc_WM_WinReconfAll(Unit);
}

/***********************************************************
  Function: R_WM_Sys_WindowDelete
*/
uint32_t R_WM_Sys_WindowDelete(uint32_t Unit, const r_wm_Window_t* Win)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    /* if the layer is enabled, disable the layer */
    if (Win->Status == R_WM_WINSTATUS_ENABLED)
    {
        loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
    }
    win_layer_pair->Win = R_NULL;
    win_layer_pair->Layer = LOC_INVALID_LAYER;
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowEnable
*/
uint32_t R_WM_Sys_WindowEnable(uint32_t Unit, const r_wm_Window_t* Win, uint32_t Enabled)
{
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }

    if (Enabled != 0)
    {
        /* do not allow enabling the layer out of screen limits */
        if (Win->PosY < 0)
        {
            if ((-Win->PosY) >= Win->Height)
            {
                return 1;
            }
        }
        if (Win->PosX < 0)
        {
            if ((-Win->PosX) >= Win->Width)
            {
                return 1;
            }
        }
        err = loc_VDCE_LayerEnable(Unit, win_layer_pair->Layer);
        if (R_VDCE_ERR_OK != err)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, err);
            return 0;
        }
    }
    else
    {
        err = loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
        if (R_VDCE_ERR_OK != err)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, err);
            return 0;
        }
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowPosSet
*/
uint32_t R_WM_Sys_WindowPosSet(uint32_t Unit, const r_wm_Window_t* Win, int32_t PosX, int32_t PosY, int32_t PosZ)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair;
    const void* fb;
    if (Win->PosZ != PosZ)
    {
        /* the Z has changed, the windows need to be reassigned to the layers */
        if (0 == loc_WM_WinReconfAll(Unit))
        {
            return 0;
        }
    }
    win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    if (0 == loc_WinSetViewportAndGeom( Unit,
                                        Win,
                                        win_layer_pair->Layer,
                                        PosX,
                                        PosY,
                                        Win->Pitch,
                                        Win->Width,
                                        Win->Height,
                                        R_WM_ColorFmtBitsPerPixGet(Win->ColorFmt) ))
    {
        return 0;
    }
    /* due to negative offset workaround, the framebuffer start address might need correction */
    if (Win->Mode != R_WM_WINMODE_SPRITES)
    {
        fb = loc_WinGetOnScreenFb(Win);
        if (fb == 0)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
            return 0;
        }
    }
    else
    {
        fb = 0;
    }
    
    if (0 == loc_WinSetFb(Unit, Win, PosX, PosY, fb))
    {
        return 0;
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowGeomSet
*/
uint32_t R_WM_Sys_WindowGeomSet (   uint32_t Unit,
                                    const r_wm_Window_t* Win,
                                    uint32_t Pitch,
                                    uint32_t Width,
                                    uint32_t Height
                                )
{
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    if (win_layer_pair->Layer == LOC_INVALID_LAYER)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, 0);
        return 0;
    }
    
    /* The Layer must be disabled for geometry configuration */
    loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
    
    if (0 == loc_WinSetViewportAndGeom( Unit,
                                        Win,
                                        win_layer_pair->Layer,
                                        Win->PosX,
                                        Win->PosY,
                                        Pitch,
                                        Width,
                                        Height,
                                        R_WM_ColorFmtBitsPerPixGet(Win->ColorFmt) ))
    {
        return 0;
    }

    /* If the layer was enabled before configuration, enable it again */
    if (Win->Status == R_WM_WINSTATUS_ENABLED)
    {
        err = loc_VDCE_LayerEnable(Unit, win_layer_pair->Layer);
        if (R_VDCE_ERR_OK != err)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, err);
            return 0;
        }
    }

    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowColorFmtSet
*/
uint32_t R_WM_Sys_WindowColorFmtSet (   uint32_t                Unit,
                                        const r_wm_Window_t*    Win,
                                        r_wm_WinColorFmt_t      ColorFmt
                                    )
{
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    if (win_layer_pair->Layer == LOC_INVALID_LAYER)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, 0);
        return 0;
    }
    
    if ( R_WM_ColorFmtBitsPerPixGet(Win->ColorFmt) != R_WM_ColorFmtBitsPerPixGet(ColorFmt) )
    {
        if (Win->Mode == R_WM_WINMODE_SPRITES)
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerMemGeometrySet(Unit, win_layer_pair->Layer, Win->Pitch, Win->Height))
            {
                return 0;
            }
        }
    }
    
    /* The Layer must be disabled for color format configuration */
    loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
    
    if (R_VDCE_ERR_OK != R_VDCE_LayerFormatSet(Unit, win_layer_pair->Layer, loc_GetVdceFormat(ColorFmt)))
    {
        return 0;
    }
    
    /* If the layer was enabled before configuration, enable it again */
    if (Win->Status == R_WM_WINSTATUS_ENABLED)
    {
        err = loc_VDCE_LayerEnable(Unit, win_layer_pair->Layer);
        if (R_VDCE_ERR_OK != err)
        {
            R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_VOUT_INTERNAL, err);
            return 0;
        }
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowAlphaSet
*/
uint32_t R_WM_Sys_WindowAlphaSet(uint32_t Unit, const r_wm_Window_t* Win, uint8_t Alpha)
{
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    if (win_layer_pair->Win->ColorKey.Enabled != 0)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NO_PHYS_WINDOW, 0);
        return 0;
    }
    
    if ((0 < win_layer_pair->Layer) && (win_layer_pair->Layer < 4))
    {
        err = R_VDCE_LayerAlphaConstEnable(Unit, win_layer_pair->Layer, Alpha);
        if (err != R_VDCE_ERR_OK)
        {
            return 0;
        }
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowPremultipliedAlphaEnable
*/
uint32_t R_WM_Sys_WindowPremultipliedAlphaEnable (uint32_t Unit, const r_wm_Window_t* Win, uint8_t Enabled)
{
    r_vdce_Error_t err;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    if ((0 < win_layer_pair->Layer) && (win_layer_pair->Layer < 4))
    {
        /* The Layer must be disabled for premultiplied alpha configuration */
        loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
        
        if (R_FALSE != Enabled)
        {
            err = R_VDCE_LayerPremultipliedAlphaEnable(Unit, win_layer_pair->Layer);
        }
        else
        {
            err = R_VDCE_LayerPremultipliedAlphaDisable(Unit, win_layer_pair->Layer);
        }

        if (err != R_VDCE_ERR_OK)
        {
            return 0;
        }
        
        /* If the layer was enabled before configuration, enable it again */
        if (Win->Status == R_WM_WINSTATUS_ENABLED)
        {
            err = loc_VDCE_LayerEnable(Unit, win_layer_pair->Layer);
            if (R_VDCE_ERR_OK != err)
            {
                return 0;
            }
        }
    }
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowFlagsUpdate
*/
uint32_t R_WM_Sys_WindowFlagsUpdate (uint32_t Unit, const r_wm_Window_t* Win, r_wm_WinFlags_t SetFlags, r_wm_WinFlags_t ClearFlags)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    if ( 0 != ( ((uint32_t)SetFlags) & ((uint32_t)ClearFlags) ) )
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SYS_WIN_FLAG_UPDATE_FAILED, 0);
        return 0;
    }
    
    if (1 == r_wm_DevSys[Unit].HwLayerEnabled[win_layer_pair->Layer])
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerDisable(Unit, win_layer_pair->Layer))
        {
            return 0;
        }
    }
    
    /* Enable vertical mirroring */
    if ( 0 != ( ((uint32_t)SetFlags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR) ) )
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerModeSet(Unit, win_layer_pair->Layer, R_VDCE_LAYER_MODE_V_MIRORING) )
        {
            return 0;
        }
    }

    /* Disable vertical mirroring */
    if ( 0 != ( ((uint32_t)ClearFlags) & ((uint32_t)R_WM_WINFLAG_V_MIRROR) ) )
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerModeSet(Unit, win_layer_pair->Layer, R_VDCE_LAYER_MODE_NONE) )
        {
            return 0;
        }
    }
    
    if (1 == r_wm_DevSys[Unit].HwLayerEnabled[win_layer_pair->Layer])
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerEnable(Unit, win_layer_pair->Layer))
        {
            return 0;
        }
    }

    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowClutSet
*/
uint32_t R_WM_Sys_WindowClutSet(uint32_t Unit, const r_wm_Window_t* Win, uint32_t NumEntries, const r_wm_ClutEntry_t* Clut)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    if (0 == NumEntries)
    {
        return 0;
    }
    else
    {
        /* Layer 0 can't do alpha blending by CLUT alpha */
        if (win_layer_pair->Layer == 0)
        {
            uint32_t i;
            for ( i=0; i<NumEntries; i++ )
            {
                if ( Clut[i].A != 0xFF )
                {
                    /* CLUTs using alpha values cause changes in Layer assignment */
                    ((r_wm_Window_t*)Win)->Clut = Clut;
                    ((r_wm_Window_t*)Win)->ClutNumEntries = NumEntries;
                    
                    if (0 == loc_WM_WinReconfAll(Unit))
                    {
                        return 0;
                    }
                    return 1;
                }
            }
        }
        
        r_wm_DevSys[Unit].FrameStatusFlags |= (1uL << (R_WM_SYS_FRAME_STATUS_LAYER_0_CLUT_UPDATE_BIT+win_layer_pair->Layer));
    }
    
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowColorKeyEnable
*/
uint32_t R_WM_Sys_WindowColorKeyEnable(uint32_t Unit, const r_wm_Window_t* Win, uint32_t Enabled)
{
    r_vdce_Error_t err;
    r_vdce_CkARGB32_t ck_in;
    r_vdce_CkARGB32_t ck_out;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    if (0 == win_layer_pair->Layer)
    {
        /* R_WM_ERR_COLOR_KEYING_NOT_SUPPORTED_FOR_LAYER */
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_COLOR_KEYING_NOT_SUPPORTED_FOR_LAYER, 0);
        return 0;
    }
    
    /* The Layer must be disabled for Colorkey configuration */
    loc_VDCE_LayerDisable(Unit, win_layer_pair->Layer);
    
    if (Enabled != 0)
    {
        ck_in.R = Win->ColorKey.In.RgbKey.Red;
        ck_in.G = Win->ColorKey.In.RgbKey.Green;
        ck_in.B = Win->ColorKey.In.RgbKey.Blue;
        ck_in.A = 0x00;
        
        ck_out.R = Win->ColorKey.Out.Red;
        ck_out.G = Win->ColorKey.Out.Green;
        ck_out.B = Win->ColorKey.Out.Blue;
        ck_out.A = Win->ColorKey.Out.Alpha;
        
        /* Layer Alpha and Chroma Keying are exclusive. 
           Chroma Keying overrides Layer Alpha. */
        if((0 < win_layer_pair->Layer) && (win_layer_pair->Layer < 4))
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerAlphaConstDisable(Unit, win_layer_pair->Layer))
            {
                return 0;
            }
        }
        if (R_VDCE_ERR_OK != R_VDCE_LayerChromaKeyEnable(Unit, win_layer_pair->Layer, &ck_in, &ck_out))
        {
            return 0;
        }
    }
    else
    {
        if (R_VDCE_ERR_OK != R_VDCE_LayerChromaKeyDisable(Unit, win_layer_pair->Layer))
        {
            return 0;
        }

        /* Layer Alpha and Chroma Keying are exclusive. 
           Layer Alpha can be enabled again. */
        if((0 < win_layer_pair->Layer) && (win_layer_pair->Layer < 4))
        {
            if (R_VDCE_ERR_OK != R_VDCE_LayerAlphaConstEnable(Unit, win_layer_pair->Layer, win_layer_pair->Win->Alpha))
            {
                return 0;
            }
        }
    }
    
    
    /* If the layer was enabled before configuration, enable it again */
    if (Win->Status == R_WM_WINSTATUS_ENABLED)
    {
        err = loc_VDCE_LayerEnable(Unit, win_layer_pair->Layer);
        if (R_VDCE_ERR_OK != err)
        {
            return 0;
        }
    }
    
    return 1;
}


