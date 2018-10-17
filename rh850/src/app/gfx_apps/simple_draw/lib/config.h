/***********************************************************************************************
PROJECT : simple_draw
FILE   : $Id: config.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================ 
DESCRIPTION
config of the simple_draw
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
#ifndef CONFIG_H_
#define CONFIG_H_
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Global Defines
*/

/*******************************************************************************
  Define: DISPLAY CONFIGURATION

  0 : VOWE frame buffer mode.
  1 : VOWE ring buffer mode.
*/
#ifdef _MARS
#define LOC_DISPLAY_NAME   "JDI_1280x480_30Hz"
#else if _MAG
#define LOC_DISPLAY_NAME "BOE_1280x480_30Hz"
#endif
//#define LOC_DISPLAY_NAME   "VESA_640x480_60Hz"
//#define LOC_DISPLAY_NAME   "VESA_800x600_60Hz"
//#define LOC_DISPLAY_NAME   "1152x1152_60Hz"
//#define LOC_DISPLAY_NAME   "NEC_QVGA_TOUCH_240x320"

#define LOC_DISPLAY_WIDTH  1280
#define LOC_DISPLAY_STRIDE 1280
#define LOC_DISPLAY_HEIGHT 480
#define LOC_DISPLAY_BPP    4

/*******************************************************************************
  Define: LOC_RING_BUFFER_MODE

  0 : VOWE frame buffer mode.
  1 : VOWE ring buffer mode.
*/
#define LOC_RING_BUFFER_MODE  (1)

/*******************************************************************************
  Define: LOC_DL_CHANGE

  0 : not change display list in executing.
  1 : change display list in executing.
*/
#define LOC_DL_CHANGE         (1)



  extern uint32_t loc_VRAM0;
  extern uint32_t loc_VRAM0_SIZE;

/*******************************************************************************
  Define: HEAP_SIZE

  Drawing engine driver CPU heap memory.

  Byte size of the drawing engine driver local heap. It
  is used for internal structures and for preparation of
  the drawing engine command lists. Its minimum size depends 
  on  the size of local command lists, number of drawing
  contexts and number of video memory blocks.
*/
#define LOC_HEAP_SIZE           (LOC_CPU_HP_BLOCK_NB * LOC_CPU_HP_BLOCK_SIZE)

/*******************************************************************************
  Define: CPU_HP_BLOCK_NB

  Number of blocks the CPU heap is divided into
  Heap Memory is allocated blockwise.
*/
#define LOC_CPU_HP_BLOCK_NB     (0x800)

/*******************************************************************************
  Define: CPU_HP_BLOCK_SIZE

  Size of one CPU heap block 
*/
#define LOC_CPU_HP_BLOCK_SIZE   (0x40)


/*******************************************************************************
  Define: ERRN_NG
  
  Error Constant set to -1
  Is used as return value when an error occurs.
*/
#define ERR_NG -1

/* VOWE work buffer is allocated before VOWE DL buffer */
#define LOC_VOWE_FRAME_BUF_SIZE  (LOC_DISPLAY_STRIDE * LOC_DISPLAY_HEIGHT * 2 * 4)
#define LOC_VOWE_RING_BUF_SIZE   (0x00020000)
#define LOC_VOWE_RING_BUFF_DELAY (50)

/* VOWE DL buffer(x2) is allocated in bottom of VRAM */
#define LOC_VOWE_DL_BUF_SIZE     (0x00010000)

#define LOC_WM_UNIT     0
#define LOC_DHD_UNIT    0
#define LOC_DRW2D_UNIT  0
#define LOC_VOWE_UNIT   0

#define LOC_VOWE_ENABLE 0

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H_ */
