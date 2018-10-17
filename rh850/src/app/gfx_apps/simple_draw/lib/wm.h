/***********************************************************************************************
PROJECT : simple_draw
FILE   : $Id: wm.h 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#ifndef WM_H_
#define WM_H_


#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
  Section: Global Functions
*/

void loc_InitWM(r_cdi_Heap_t *Cpu, r_cdi_Heap_t *Vid);
void HMI_InitWM(r_cdi_Heap_t *Cpu, r_cdi_Heap_t *Vid);

void *loc_CreateWindow  (   r_wm_WinMode_t      Mode,
                            uint32_t            PosX,
                            uint32_t            PosY,
                            uint32_t            PosZ,
                            uint32_t            Width,
                            uint32_t            Height,
                            uint32_t            Pitch,
                            r_wm_WinColorFmt_t  ColorFmt,
                            r_wm_Window_t*      Window
                        );
void loc_WaitVblank(void);

void *loc_CreateWindow_EXT(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window,
                          r_wm_WinBuffer_t *ExtFB);




void *loc_CreateRLEWindow(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window);

void *W_APPHMIWindow_Creat(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window);

void *Copy_And_Get(void **addr_pic, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* WM_H_ */
