/*
****************************************************************************
PROJECT : WM
FILE    : $Id: r_sys_wm.h 13421 2017-05-08 13:49:09Z matthias.nippert $
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

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.

****************************************************************************
*/

#ifndef SYS_WM_H__
#define SYS_WM_H__

/***********************************************************
  Title: r_sys_wm.h
  
  D1x SYS WM Support Functions

  Declaration of local SYS WM support functions for the D1x device.
*/


/***********************************************************
  Section: Global Defines
*/




/***********************************************************
  Section: SYS WM Global Constants
*/

/***********************************************************
  Constant: R_WM_SYS_GLOBAL_STATUS_FLAG_*
  
  Definition of valid bits for type r_wm_DevSys_t.GlobalStatusFlags.
*/
/* Set if RLE Unit is active. This is used by the opposite WM Unit to correctly configure their shared resources. */
#define R_WM_SYS_GLOBAL_STATUS_FLAG_RLE0                (0x00000001uL)   /*Bit 0*/
#define R_WM_SYS_GLOBAL_STATUS_FLAG_RLE1                (0x00000002uL)   /*Bit 1*/
#define R_WM_SYS_GLOBAL_STATUS_FLAG_RLE2                (0x00000004uL)   /*Bit 2*/
#define R_WM_SYS_GLOBAL_STATUS_FLAG_RLE3                (0x00000008uL)   /*Bit 3*/
/* Set if a window was visible during the current frame. If not, special care is taken for Sprite engine updates. */
#define R_WM_SYS_GLOBAL_STATUS_FLAG_WINDOWS_VISIBLE     (0x00000010uL)   /*Bit 4*/
/* Set if capturing is activated. This is used to to prevent deletion of active capturing windows */
#define R_WM_SYS_GLOBAL_STATUS_FLAG_CAPTURE_ACTIVE      (0x00000020uL)   /*Bit 5*/
/* Set to shutdown the WM driver. This triggers early exit of some processing loops. */
#define R_WM_SYS_GLOBAL_STATUS_FLAG_SHUTDOWN            (0x80000000uL)   /*Bit 31*/

/***********************************************************
  Constant: R_WM_SYS_FRAME_STATUS_*
  
  Definition of valid bits for type r_wm_DevSys_t.FrameStatusFlags.
*/
/* A sprite configuration has changed and needs to be updated */
#define R_WM_SYS_FRAME_STATUS_SPEA_UPDATE_MASK          (0x00000fffuL)

#define R_WM_SYS_FRAME_STATUS_SPEA_RLE0_UPDATE_BIT      (0uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_RLE1_UPDATE_BIT      (1uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_RLE2_UPDATE_BIT      (2uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_RLE3_UPDATE_BIT      (3uL)

#define R_WM_SYS_FRAME_STATUS_SPEA_SU0_UPDATE_BIT       (4uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU1_UPDATE_BIT       (5uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU2_UPDATE_BIT       (6uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU3_UPDATE_BIT       (7uL)

/* The sprite order has changed and needs to be reassigned */
#define R_WM_SYS_FRAME_STATUS_SPEA_SU0_REASSIGN_BIT     (8uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU1_REASSIGN_BIT     (9uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU2_REASSIGN_BIT     (10uL)
#define R_WM_SYS_FRAME_STATUS_SPEA_SU3_REASSIGN_BIT     (11uL)

/* The CLUT or the layer order has changed and the CLUT must be updated */
#define R_WM_SYS_FRAME_STATUS_LAYER_CLUT_UPDATE_MASK    (0x0000f000uL)

#define R_WM_SYS_FRAME_STATUS_LAYER_0_CLUT_UPDATE_BIT   (12uL)
#define R_WM_SYS_FRAME_STATUS_LAYER_1_CLUT_UPDATE_BIT   (13uL)
#define R_WM_SYS_FRAME_STATUS_LAYER_2_CLUT_UPDATE_BIT   (14uL)
#define R_WM_SYS_FRAME_STATUS_LAYER_3_CLUT_UPDATE_BIT   (15uL)

/***********************************************************
  Constant: LOC_INVALID_LAYER
  
  Defines an invalid layer constant.
*/
#define LOC_INVALID_LAYER   0xffffffffu

/***********************************************************
  Constant: LOC_INVALID_SPRITE 
  
  Defines an invalid sprite constant.
*/
#define LOC_INVALID_SPRITE  (R_WM_MAX_SPRITES_PER_LAYER)

/***********************************************************
  Macro: LOC_USE_OS

  Flag signaling if the OS is used
*/
#ifdef USE_ROS
  #define LOC_USE_OS
#endif

/***********************************************************
  Macro: R_WM_SYS_ERROR_TRACKING 
  
  If active, defines a function to save the last occurred error.
*/
/* #define R_WM_SYS_ERROR_TRACKING */

#ifdef R_WM_SYS_ERROR_TRACKING
    
    #define R_WM_SYS_REPORT_ERROR(Unit,Error,AdditionalInfo)    \
                (loc_WM_Sys_SetLastError((Unit),(Error),(AdditionalInfo), __FILE__, __LINE__))
    
    void loc_WM_Sys_SetLastError(uint32_t Unit, r_wm_Error_t Error, uint32_t AdditionalInfo, char *File, uint32_t Line);

#else /* R_WM_SYS_ERROR_TRACKING */
    #define R_WM_SYS_REPORT_ERROR(x,y,z)
  
#endif /* R_WM_SYS_ERROR_TRACKING */




/***********************************************************
  Section: SYS WM Global Types
*/

/***********************************************************
  Type: r_wm_sys_vdce_WinLayerPair_t

  Reflects the layer position of a window or vice versa.
*/
typedef struct
{
    const r_wm_Window_t* Win;
    uint32_t Layer;
} r_wm_sys_vdce_WinLayerPair_t;

/***********************************************************
  Type: r_wm_DevSys_t

  Device instance specific data.
  
  All instance specific data is kept within that structure.

  Members:
  EvQueueRingBuf    -     Buffer for the messages of the window manager
  WinToLayerMap     -     Map for which whindow is assigned to which layer
  HwLayerEnabled    -     Which window is currently set to Enabled
  RootWin           -     pointer to first (lowest) window
  RootCapt          -     pointer to first (only) capture
  RegEvents         -     Keep track of registered events. Bitfield, check <r_wm_Event_t> for available bits.
  EventCb           -     Callback function for system events
  FrameStatusFlags  -     These flags may be set during the processing of a frame in order to toggle updates after finishing the current message queue.
                            There are changes written that need to be transferred to the shadow buffers of the SPEA
                          Bit 0: R_SPEA_RLE Register Update Required
                          Bit 1: R_SPEA_SU0 Register Update Required
                          Bit 2: R_SPEA_SU1 Register Update Required
                          Bit 3: R_SPEA_SU2 Register Update Required
                            There are changes in the order of the sprites (Create, Delete, Z-Order change)
                          Bit 4: R_SPEA_SU0 Reassignment Required
                          Bit 5: R_SPEA_SU1 Reassignment Required
                          Bit 6: R_SPEA_SU2 Reassignment Required
                          FrameStatusFlags is reset after each frame.
  GlobalStatusFlags -     Keep Track of different HW Status
  HorizontalMoveReserve - Keep track of how many pixels a layer can be moved into negative horizontal direction.
*/
typedef struct r_wm_DevSys_s
{
    r_cdi_RBuf_t                    EvQueueRingBuf;
    r_wm_sys_vdce_WinLayerPair_t    WinToLayerMap[ R_WM_MAX_LAYERS];
    uint32_t                        HwLayerEnabled[ R_WM_MAX_LAYERS];
    r_wm_Window_t                   *RootWin;
    r_wm_Capture_t                  *RootCapt;
    uint32_t                        RegEvents;
    void                            (*EventCb)(uint32_t Unit, const r_wm_Event_t *Event);
    uint32_t                        FrameStatusFlags;
    uint32_t                        GlobalStatusFlags;/* bit0 - RLE unit 0 in use, bit1 - RLE unit 1 in use */
    int32_t                         HorizontalMoveReserve;
} r_wm_DevSys_t;


/***********************************************************
  Section: SYS WM Global Variables
*/

/***********************************************************
  Variable: r_wm_DevSys

  Array for device instance specific data of type <r_wm_DevSys_t>
*/
extern r_wm_DevSys_t r_wm_DevSys[R_WM_DEV_NUM];  /* will be initialized to zero by compiler/loader*/ 


/***********************************************************
  Variable: loc_WM_Isr

  List of all WM internal interrupt callbacks.
*/
extern void (*const loc_WM_Isr[R_WM_DEV_NUM][R_WM_EVENT_LAST])(void);




/***********************************************************
  Section: SYS WM Global Functions
*/

/***********************************************************
  Function: loc_WM_Sprite_GetSpriteUnitFromLayerNo
  
  Return the SPEA unit assigned to the given layer 
  as defined by r_spea_Unit_t.
*/
r_spea_Unit_t loc_WM_Sprite_GetSpriteUnitFromLayerNo(uint32_t Unit, uint32_t Layer);
r_spea_Unit_t loc_WM_Sprite_GetRLEUnitFromLayerNo(uint32_t Unit, uint32_t Layer);

/***********************************************************
  Function: loc_WM_Sprite_SetReassignFlag
  
  Register the Sprite Unit defined by r_spea_Unit_t to be rewritten with the next frame.
*/
void loc_WM_Sprite_SetReassignFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);

/***********************************************************
  Function: loc_WM_Sprite_SetUpdateFlag
  
  Register the Sprite Unit defined by r_spea_Unit_t to be updated with the next frame.
*/
void loc_WM_Sprite_SetUpdateFlag(uint32_t Unit, r_spea_Unit_t SpeaUnit);

/***********************************************************
  Function: loc_ReassignAllSprites
  
  Rewrites all SPEA Registers that need to be updated
*/
uint32_t loc_WM_Sprite_ReassignAllSprites(uint32_t Unit);

/***********************************************************
  Function: loc_UpdateAllSprites
  
  Triggers a HW refresh for all previously written SPEA Sprite registers.
*/
uint32_t loc_WM_Sprite_UpdateAllSprites(uint32_t Unit);

/***********************************************************
  Function: loc_UpdateAllRLEs
  
  Triggers a HW refresh for all previously written SPEA RLE registers.
*/
uint32_t loc_WM_Sprite_UpdateAllRLEs(uint32_t Unit);

/***********************************************************
  Function: loc_SynchronizeSprites
  
  If no window was visible in this frame, pending SPEA Update Requests cannot be completed immediately.
   => Manually create a SYNC signal. 
      Flickering or artifacts are no problem as nothing is visible on-screen.
*/
uint32_t loc_WM_Sprite_SynchronizeSprites(uint32_t Unit);

/***********************************************************
  Function: loc_WM_Window_UpdateAllCLUTs
  
  Rewrites all CLUT RAMs that need to be updated.
*/
uint32_t loc_WM_Window_UpdateAllCLUTs(uint32_t Unit);

/***********************************************************
  Function: loc_WM_WinLayerPairGet
  
  Return a WinLayerPair based on the given Window.
  Return NULL, if window has no layer.
*/
r_wm_sys_vdce_WinLayerPair_t* loc_WM_WinLayerPairGet(uint32_t Unit, const r_wm_Window_t* Win);
r_wm_sys_vdce_WinLayerPair_t* loc_WinLayerPairGetByLayer(uint32_t Unit, const uint32_t Layer);

/***********************************************************
  Function: loc_WM_WinReconfAll
  
  Transfer all changes done to the window configuration down to the hardware.
*/
uint32_t loc_WM_WinReconfAll(uint32_t Unit);

#endif /* #define SYS_WM_H__ */
