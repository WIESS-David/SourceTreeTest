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


/***********************************************************
  Section: Local Defines
*/

#ifdef R_WM_SYS_ERROR_TRACKING
    #include "r_bsp_stdio_api.h"
#endif

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
  Variable : device
  
  Device, the WM driver is running on.
*/
extern r_dev_Device_t r_wm_Device;

/***********************************************************
  Section: Local Function Declarations
*/

  r_spea_Unit_t loc_WM_Sprite_GetSpriteUnitFromLayerNo(uint32_t Unit, uint32_t Layer);
static uint32_t loc_WM_Sprite_GetLayerNoFromSpriteUnit(uint32_t Unit, r_spea_Unit_t SpeaUnit);
static uint8_t  loc_WM_Sprite_GetSpriteNo(uint32_t Unit, const r_wm_Sprite_t *Sprite);

       void     loc_WM_Sprite_SetReassignFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);
static uint32_t loc_WM_Sprite_GetReassignFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);

       void     loc_WM_Sprite_SetUpdateFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);
static uint32_t loc_WM_Sprite_GetUpdateFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);

static uint32_t loc_WM_Sprite_ReassignAllSpritesOfLayer(uint32_t Unit, r_wm_Sprite_t* RootSprite, uint32_t Layer);



/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Variable : speaLayer2UnitAssignment
  
  Used by functions <loc_WM_Sprite_GetSpriteUnitFromLayerNo> 
  and <loc_WM_Sprite_GetLayerNoFromSpriteUnit>.
*/
static const r_spea_Unit_t speaLayer2UnitAssignment[R_WM_DEV_NUM][ R_WM_MAX_LAYERS] = {
    {R_SPEA_SU3, R_SPEA_SU0, R_SPEA_SU1, R_SPEA_SU2},
    {R_SPEA_SU3, R_SPEA_SU2, R_SPEA_SU1, R_SPEA_SU0}
};
static const r_spea_Unit_t rleLayer2UnitAssignment[R_WM_DEV_NUM][ R_WM_MAX_LAYERS] = {
    {R_SPEA_RLE0, R_SPEA_RLE1, R_SPEA_RLE2, R_SPEA_RLE3},
    {R_SPEA_RLE0, R_SPEA_RLE3, R_SPEA_RLE2, R_SPEA_RLE1}
};


/***********************************************************
  Section: WM Driver Local Functions
*/

/***********************************************************
  Function: loc_WM_Sprite_GetSpriteUnitFromLayerNo
*/
r_spea_Unit_t loc_WM_Sprite_GetSpriteUnitFromLayerNo(uint32_t Unit, uint32_t Layer)
{
    if ( (Layer >  R_WM_MAX_LAYERS) || (Unit > R_WM_DEV_NUM) )
    {
        return R_SPEA_SU_LAST;
    }
    
    if ( (r_wm_Device != R_DEV_R7F701441) && (Layer == 0) )
    {
        return R_SPEA_SU_LAST;
    }
    
    return speaLayer2UnitAssignment[Unit][Layer];
}

r_spea_Unit_t loc_WM_Sprite_GetRLEUnitFromLayerNo(uint32_t Unit, uint32_t Layer)
{
    if ( (Layer >  R_WM_MAX_LAYERS) || (Unit > R_WM_DEV_NUM) )
    {
        return R_SPEA_SU_LAST;
    }
    
    if ( (r_wm_Device != R_DEV_R7F701441) && (Layer > 0) )
    {
        return R_SPEA_SU_LAST;
    }
    
    return rleLayer2UnitAssignment[Unit][Layer];
}

/***********************************************************
  Function: loc_WM_Sprite_GetLayerNoFromSpriteUnit
*/
static uint32_t loc_WM_Sprite_GetLayerNoFromSpriteUnit(uint32_t Unit, r_spea_Unit_t SpeaUnit)
{
    uint32_t layer;

    if ( (r_wm_Device != R_DEV_R7F701441) && (SpeaUnit == R_SPEA_SU3) )
    {
        layer =  R_WM_MAX_LAYERS;
    }

    for(layer=0; layer< R_WM_MAX_LAYERS; layer++)
    {
        if (speaLayer2UnitAssignment[Unit][layer] == SpeaUnit)
        {
            break;
        }
    }

    if (layer >=  R_WM_MAX_LAYERS)
    {
        layer =  R_WM_MAX_LAYERS;
    }

    return layer;
}

/***********************************************************
  Function: loc_WM_Sprite_SetReassignFlag
*/
void loc_WM_Sprite_SetReassignFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit)
{
    uint32_t mask = 0;
    switch (SpeaUnit)
    {
        case R_SPEA_SU0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU0_REASSIGN_BIT;
            break;
        case R_SPEA_SU1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU1_REASSIGN_BIT;
            break;
        case R_SPEA_SU2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU2_REASSIGN_BIT;
            break;
        case R_SPEA_SU3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU3_REASSIGN_BIT;
            break;
        default:
            break;
    }
    r_wm_DevSys[Unit].FrameStatusFlags |= mask;
}

/***********************************************************
  Function: loc_WM_Sprite_GetReassignFlag
*/
static uint32_t loc_WM_Sprite_GetReassignFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit)
{
    uint32_t mask = 0;
    switch (SpeaUnit)
    {
        case R_SPEA_SU0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU0_REASSIGN_BIT;
            break;
        case R_SPEA_SU1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU1_REASSIGN_BIT;
            break;
        case R_SPEA_SU2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU2_REASSIGN_BIT;
            break;
        case R_SPEA_SU3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU3_REASSIGN_BIT;
            break;
        default:
            break;
    }
    if ( (r_wm_DevSys[Unit].FrameStatusFlags & mask) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************
  Function: loc_WM_Sprite_SetUpdateFlag
*/
void loc_WM_Sprite_SetUpdateFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit)
{
    uint32_t mask = 0;
    switch (SpeaUnit)
    {
        case R_SPEA_RLE0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE0_UPDATE_BIT;
            break;
        case R_SPEA_RLE1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE1_UPDATE_BIT;
            break;
        case R_SPEA_RLE2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE2_UPDATE_BIT;
            break;
        case R_SPEA_RLE3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE3_UPDATE_BIT;
            break;
        case R_SPEA_SU0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU0_UPDATE_BIT;
            break;
        case R_SPEA_SU1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU1_UPDATE_BIT;
            break;
        case R_SPEA_SU2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU2_UPDATE_BIT;
            break;
        case R_SPEA_SU3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU3_UPDATE_BIT;
            break;
        default:
            break;

    }
    r_wm_DevSys[Unit].FrameStatusFlags |= mask;
}

/***********************************************************
  Function: loc_WM_Sprite_GetUpdateFlag
*/
static uint32_t loc_WM_Sprite_GetUpdateFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit)
{
    uint32_t mask = 0;
    switch (SpeaUnit)
    {
        case R_SPEA_RLE0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE0_UPDATE_BIT;
            break;
        case R_SPEA_RLE1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE1_UPDATE_BIT;
            break;
        case R_SPEA_RLE2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE2_UPDATE_BIT;
            break;
        case R_SPEA_RLE3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_RLE3_UPDATE_BIT;
            break;
        case R_SPEA_SU0:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU0_UPDATE_BIT;
            break;
        case R_SPEA_SU1:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU1_UPDATE_BIT;
            break;
        case R_SPEA_SU2:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU2_UPDATE_BIT;
            break;
        case R_SPEA_SU3:
            mask = 1uL << R_WM_SYS_FRAME_STATUS_SPEA_SU3_UPDATE_BIT;
            break;
        default:
            break;

    }
    if ( (r_wm_DevSys[Unit].FrameStatusFlags & mask) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/***********************************************************
  Function: loc_WM_Sprite_GetSpriteNo
*/
static uint8_t loc_WM_Sprite_GetSpriteNo(uint32_t Unit, const r_wm_Sprite_t *Sprite)
{
    const r_wm_Sprite_t* tmp_sprite = Sprite->Window->Surface.SpritesRoot;
    uint8_t sprite_no = 0;
    /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
       elements for this device family is R_WM_MAX_SPRITES_PER_LAYERS. */
    while   (   (tmp_sprite != R_NULL)
                &&
                (sprite_no < R_WM_MAX_SPRITES_PER_LAYER)
            )
    {
        if (tmp_sprite == Sprite)
        {
            return sprite_no;
        }
        ++sprite_no;
        tmp_sprite = tmp_sprite->Next;
    }
    return LOC_INVALID_SPRITE;
}


/***********************************************************
  Group: WM SPEA Update Functions
*/

/***********************************************************
  Function: loc_WM_Sprite_ReassignAllSpritesOfLayer
  
  Rewrite the sprite configuration for the sprite unit identified by the given RootSprite
*/
static uint32_t loc_WM_Sprite_ReassignAllSpritesOfLayer(uint32_t Unit, r_wm_Sprite_t* RootSprite, uint32_t Layer)
{
    /* reassign all the sprites  */
    r_spea_SpriteConfig_t sprite_conf;
    uint32_t sprite_no = 0;
    r_wm_Sprite_t* sprite = RootSprite;
    
    r_spea_Unit_t spea_unit;
    
    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, Layer);
    
    /* Iteration of NULL-terminated Linked-List. Maximum Number of expected 
       elements for this device family is R_WM_MAX_SPRITES_PER_LAYER. */
    while   (   (sprite != R_NULL)
                &&
                (sprite_no < R_WM_MAX_SPRITES_PER_LAYER)
            )
    {
        sprite_conf.PosX = sprite->PosX;
        sprite_conf.PosY = sprite->PosY;
        sprite_conf.Width = sprite->Width;
        sprite_conf.Height = sprite->Height;
        sprite_conf.SrcAddr = (uint32_t)sprite->Data; 
        sprite_conf.VSync = Unit;
        if (sprite->Status == R_WM_SPRITESTATUS_ENABLED)
        {
            sprite_conf.Enable = 1;
        }
        else
        {
            sprite_conf.Enable = 0;
        }
        if (R_SPEA_ERR_OK != R_SPEA_SetSprite(
                                0, spea_unit-R_SPEA_SU0, sprite_no, 
                                &sprite_conf, R_WM_ColorFmtBitsPerPixGet(sprite->Window->ColorFmt)))
        {
            return 0;
        }
        ++sprite_no;
        sprite = sprite->Next;
    }
    
    for (; sprite_no < R_WM_MAX_SPRITES_PER_LAYER; ++sprite_no)
    {
        if (R_SPEA_ERR_OK != R_SPEA_SpriteEnable(0, spea_unit-R_SPEA_SU0, sprite_no, 0))
        {
            return 0;
        }
    }
    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
    loc_WM_Sprite_SetUpdateFlag(Unit, spea_unit);
    
    return 1;
}

/***********************************************************
  Function: loc_ReassignAllSprites
*/
uint32_t loc_WM_Sprite_ReassignAllSprites(uint32_t Unit)
{
    r_spea_Unit_t spea_unit;
    uint32_t layer;
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair;
    
    /* Rewrite the Sprite configuration for those units that got changed during the last frame */
    for (spea_unit = R_SPEA_SU0; spea_unit <= R_SPEA_SU3; ++spea_unit)
    {
        /* Only D1M1A has Sprite Unit SU3 */
        if ( (r_wm_Device != R_DEV_R7F701441) && (spea_unit == R_SPEA_SU3) )
        {
            break;
        }
        /* Read respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_REASSIGN_BIT */
        if (loc_WM_Sprite_GetReassignFlag(Unit, spea_unit) != 0)
        {
            /* Update respective window's sprites */
            layer = loc_WM_Sprite_GetLayerNoFromSpriteUnit(Unit, spea_unit);
            win_layer_pair = loc_WinLayerPairGetByLayer(Unit, layer);
            
            if ( (win_layer_pair == 0) || (win_layer_pair->Win->Mode != R_WM_WINMODE_SPRITES) )
            {
                /* Layer is not in use or is no sprite layer (anymore), deinitialise all sprites */
                if (0 == loc_WM_Sprite_ReassignAllSpritesOfLayer(Unit, R_NULL, layer))
                {
                    return 0;
                }
            }
            else /*if (win_layer_pair != 0)*/
            {
                if (0 == loc_WM_Sprite_ReassignAllSpritesOfLayer(Unit, win_layer_pair->Win->Surface.SpritesRoot, win_layer_pair->Layer))
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/***********************************************************
  Function: loc_WM_Sprite_UpdateAllSprites
*/
uint32_t loc_WM_Sprite_UpdateAllSprites(uint32_t Unit)
{
    r_spea_Unit_t spea_unit;

    /* Set update request bit for those Sprite Unit Registers, that have been written to in the last frame */
    for (spea_unit = R_SPEA_SU0; spea_unit <= R_SPEA_SU3; ++spea_unit)
    {
        /* Only D1M1A has Sprite Unit SU3 */
        if ( (r_wm_Device != R_DEV_R7F701441) && (spea_unit == R_SPEA_SU3) )
        {
            break;
        }
        /* Read respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
        if (loc_WM_Sprite_GetUpdateFlag(Unit, spea_unit) != 0)
        {
            if (R_SPEA_ERR_OK != R_SPEA_UpdateUnit(0, spea_unit, Unit))
            {
                return 0;
            }
        }
    }
    return 1;
}

/***********************************************************
  Function: loc_WM_Sprite_UpdateAllRLEs
*/
uint32_t loc_WM_Sprite_UpdateAllRLEs(uint32_t Unit)
{
    r_spea_Unit_t spea_unit;
    
    /* Process update request for those RLE Unit Registers, if it has been written to in the last frame */
    for (spea_unit = R_SPEA_RLE0; spea_unit <= R_SPEA_RLE3; ++spea_unit)
    {
        /* Only D1M1A has other RLE Units than RLE0 */
        if ( (r_wm_Device != R_DEV_R7F701441) && (spea_unit != R_SPEA_RLE0) )
        {
            break;
        }
        if (loc_WM_Sprite_GetUpdateFlag(Unit, spea_unit) != 0)
        {
            /* For RLE's (rather than Sprites) only parameter <Unit> is relevant. */
            if (R_SPEA_ERR_OK != R_SPEA_UpdateUnit(spea_unit, spea_unit, Unit))
            {
                return 0;
            }
        }
    }
    return 1;
}

/***********************************************************
  Function: loc_WM_Sprite_SynchronizeSprites
*/
uint32_t loc_WM_Sprite_SynchronizeSprites(uint32_t Unit)
{
    /* If no window was visible in this frame, pending SPEA Update Requests cannot be completed immediately.
       => Manually create a SYNC signal. 
          Flickering or artifacts are no problem as nothing is visible on-screen! */
    if ( ((r_wm_DevSys[Unit].FrameStatusFlags & R_WM_SYS_FRAME_STATUS_SPEA_UPDATE_MASK) != 0)
        && ((r_wm_DevSys[Unit].GlobalStatusFlags & R_WM_SYS_GLOBAL_STATUS_FLAG_WINDOWS_VISIBLE) == 0) )
    {
        uint32_t vdcectl = R_DEV_READ_REG(32, VDCECTL);
        
    #if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
        R_WM_Profile_Log_CheckSPEAUpd();
    #endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */

        /* Check UM 37.6 "Hardware and Software Update" about the use of the UPDTn Bit */
        /* Set SYNC(Unit) Signal to 1 */
        R_DEV_WRITE_REG(32, VDCECTL, vdcectl | ((0 == Unit) ? VDCECTL_UPDT0 : VDCECTL_UPDT1) );
        
        /* Set to 0 to create falling edge */
        R_DEV_WRITE_REG(32, VDCECTL, vdcectl);
        
        
    #if defined R_WM_MSQ_DEQUEUE_PROFILING || defined R_WM_MSQ_ENQUEUE_PROFILING
        R_WM_Profile_Log_SPEASWSync();
        for (vdcectl=0; vdcectl<10; vdcectl++) {
            R_WM_Profile_Log_CheckSPEAUpd();
        }
    #endif /* R_WM_MSQ_DEQUEUE_PROFILING || R_WM_MSQ_ENQUEUE_PROFILING */
    }
    return 1;
}



/***********************************************************
  Section: WM Driver Support Functions
  
  Comments see: <WM Support Functions>
*/


/***********************************************************
  Group: WM Video Output Sprite interface
*/


/***********************************************************
  Function: R_WM_Sys_SpriteCreate
*/
uint32_t R_WM_Sys_SpriteCreate(uint32_t Unit, const r_wm_Sprite_t* Sprite)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Sprite->Window);
    r_spea_Unit_t spea_unit;

    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }
    
    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    loc_WM_Sprite_SetReassignFlag(Unit, spea_unit);

    /* make the sprite initially invisible */
    R_WM_Sys_SpriteEnable(Unit, Sprite, 0);
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_SpriteEnable
*/
uint32_t R_WM_Sys_SpriteEnable(uint32_t Unit, const r_wm_Sprite_t* Sprite, uint32_t Enabled)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Sprite->Window);
    uint8_t sprite_no = LOC_INVALID_SPRITE;
    r_spea_Unit_t spea_unit;
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }

    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    sprite_no = loc_WM_Sprite_GetSpriteNo(Unit, Sprite);
    if (sprite_no == LOC_INVALID_SPRITE)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, 0);
        return 0;
    }
    
    if (R_SPEA_ERR_OK != R_SPEA_SpriteEnable(0, spea_unit-R_SPEA_SU0, sprite_no, Enabled))
    {
        return 0;
    }
    
    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
    loc_WM_Sprite_SetReassignFlag(Unit, spea_unit);
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_SpriteDelete
*/
uint32_t R_WM_Sys_SpriteDelete(uint32_t Unit, const r_wm_Sprite_t* Sprite)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Sprite->Window);
    r_spea_Unit_t spea_unit;
    
    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    loc_WM_Sprite_SetReassignFlag(Unit, spea_unit);

    return 1;
}

/***********************************************************
  Function: R_WM_Sys_SpriteMove
*/
uint32_t R_WM_Sys_SpriteMove(uint32_t Unit, const r_wm_Sprite_t* Sprite, uint32_t PosX, uint32_t PosY, uint32_t PosZ)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Sprite->Window);
    uint32_t sprite_no = LOC_INVALID_SPRITE;
    
    r_spea_Unit_t spea_unit;
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }

    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    sprite_no = loc_WM_Sprite_GetSpriteNo(Unit, Sprite);
    if (sprite_no == LOC_INVALID_SPRITE)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, 0);
        return 0;
    }
    
    if (R_SPEA_ERR_OK != R_SPEA_SetSpritePos(
                            0, spea_unit-R_SPEA_SU0, sprite_no, 
                            PosX, PosY, R_WM_ColorFmtBitsPerPixGet(Sprite->Window->ColorFmt)))
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_SPEA_INTERNAL, 0);
        return 0;
    }

    /* If z-Pos of the spite has changes, all sprite registers have to be reassigned. */
    if (Sprite->PosZ != PosZ)
    {
        loc_WM_Sprite_SetReassignFlag(Unit, spea_unit);
    }
    
    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
    loc_WM_Sprite_SetUpdateFlag(Unit, spea_unit);
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_SpriteBufSet
*/
uint32_t R_WM_Sys_SpriteBufSet(uint32_t Unit, const r_wm_Sprite_t *Sprite, void* Buf)
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Sprite->Window);
    r_spea_SpriteConfig_t sprite_conf;
    uint32_t sprite_no = LOC_INVALID_SPRITE;
    
    r_spea_Unit_t spea_unit;
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }

    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    sprite_no = loc_WM_Sprite_GetSpriteNo(Unit, Sprite);
    if (sprite_no == LOC_INVALID_SPRITE)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_RANGE_WM, 0);
        return 0;
    }
    
    sprite_conf.PosX = Sprite->PosX;
    sprite_conf.PosY = Sprite->PosY;
    sprite_conf.Width = Sprite->Width;
    sprite_conf.Height = Sprite->Height;
    sprite_conf.SrcAddr = (uint32_t)Buf;
    sprite_conf.VSync = Unit;
    if (Sprite->Status == R_WM_SPRITESTATUS_ENABLED)
    {
        sprite_conf.Enable = 1;
    }
    else
    {
        sprite_conf.Enable = 0;
    }
    sprite_conf.VSync = Unit;
    
    if (R_SPEA_ERR_OK != R_SPEA_SetSprite(
                            0, spea_unit-R_SPEA_SU0, sprite_no, 
                            &sprite_conf, R_WM_ColorFmtBitsPerPixGet(Sprite->Window->ColorFmt)))
    {
        return 0;
    }
    
    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
    loc_WM_Sprite_SetUpdateFlag(Unit, spea_unit);
    return 1;
}

/***********************************************************
  Function: R_WM_Sys_WindowDeleteAllSprites
*/
uint32_t R_WM_Sys_WindowDeleteAllSprites    (   uint32_t Unit,
                                                const r_wm_Window_t* Win
                                            )
{
    r_wm_sys_vdce_WinLayerPair_t* win_layer_pair = loc_WM_WinLayerPairGet(Unit, Win);
    uint32_t sprite;
    r_spea_Unit_t spea_unit;
    
    if (win_layer_pair == R_NULL)
    {
        R_WM_SYS_REPORT_ERROR(Unit, R_WM_ERR_NULL_PTR_ARGUMENT, 0);
        return 0;
    }

    spea_unit = loc_WM_Sprite_GetSpriteUnitFromLayerNo(Unit, win_layer_pair->Layer);
    
    for (sprite = 0; sprite < R_WM_MAX_SPRITES_PER_LAYER; ++sprite)
    {
        if (R_SPEA_ERR_OK != R_SPEA_SpriteEnable(0, spea_unit-R_SPEA_SU0, sprite, 0))
        {
            return 0;
        }
    }
    
    /* Set respective R_WM_SYS_FRAME_STATUS_SPEA_SUn_UPDATE_BIT */
    loc_WM_Sprite_SetUpdateFlag(Unit, spea_unit);
    return 1;
}

