/************************************************************************************************
PROJECT : simple_draw app
FILE    : $Id: wm.c 11006 2016-11-07 16:32:44Z michael.golczewski $
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
#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "r_drw2d_os.h"
#include "r_ddb_api.h"
#include "r_cdi_api.h"
#include "r_wm_api.h"
#include "r_bsp_stdio_api.h"

#include "config.h"
#include "w_system.h"
#include "wm.h"
#include "main.h"

#include <strings.h>

#include "r_config_d1x.h"
#include "r_vdce_api.h"
#include "w_mag.h"
#include "w_canio_pics_gen.h"
#include "w_cluster_task.h"

extern void W_SYS_Main_Entry();

/*******************************************************************************
  Section: Local defines
*/
#define WM_MSG_QUEUE_LEN 32


/*******************************************************************************
  Section: Local variables
  
*/
static r_wm_Msg_t wm_msg_queue[WM_MSG_QUEUE_LEN];
//static r_wm_Msg_t wm_msg_hmi[WM_MSG_QUEUE_LEN];

//TODO: refine this with all image struct
void * addr_pic_background = (void *)_PIC_C_BG_RLE; //_PIC_BASE

r_wm_WinBuffer_t RLE_buf;
void* sdram_img_addr = BASE_IMG_SDRAM;

/*******************************************************************************
  Section: Local functions

*/


/*******************************************************************************
  Function: loc_InitWM

    See: main.h for details
*/
void loc_InitWM(r_cdi_Heap_t *Cpu, r_cdi_Heap_t *Vid)
{
    r_wm_Error_t wmErr;
    wmErr = R_WM_DevInit(LOC_WM_UNIT, wm_msg_queue, WM_MSG_QUEUE_LEN, 0, (void*)Cpu, (void*)Vid);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenTimingSetByName(LOC_WM_UNIT, (const int8_t*)LOC_DISPLAY_NAME);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenColorFormatSet(LOC_WM_UNIT, (R_WM_OUTCOLORFMT_RGB888));
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    wmErr = R_WM_ScreenEnable(LOC_WM_UNIT);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    R_WM_ScreenBgColorSet(LOC_WM_UNIT, 0, 0, 0);
    if(R_WM_ERR_OK != wmErr)
    {
        loc_Error(1);
    }
}

void *loc_CreateWindow(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window)
{
    r_wm_Error_t wmErr;

    /* clear */
    memset(Window, 0, sizeof(r_wm_Window_t));

    Window->Mode      = Mode;
    Window->Status    = R_WM_WINSTATUS_NOT_INITIALIZED;
    Window->ColorFmt  = ColorFmt;
    Window->Alpha     = 0xff;
    Window->PosX      = PosX;
    Window->PosY      = PosY;
    Window->PosZ      = PosZ;
    Window->Width     = Width;
    Window->Height    = Height;
    Window->Pitch     = Pitch;

    if(Mode == R_WM_WINMODE_FB)
    {
        
        Window->Surface.Fb.BufNum    = 2;
        Window->Surface.Fb.Buffer    = 0;
        Window->Surface.Fb.BufMode   = R_WM_WINBUF_ALLOC_INTERNAL;
    }

    wmErr = R_WM_WindowCreate(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
        if (R_WM_ERR_MALLOC_FAILED == wmErr)
        {
            R_BSP_STDIO_Printf("11Not enough space for Window Framebuffers! "
                "Please consider changing VOWE to Ringbuffer mode to reduce the ringbuffer size or reduce display resolution.\n");
        }
        loc_Error(1);
    }
    wmErr = R_WM_WindowEnable(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }

    return Window;
}



void *loc_CreateWindow_EXT(r_wm_WinMode_t      Mode,
                          uint32_t            PosX,
                          uint32_t            PosY,
                          uint32_t            PosZ,
                          uint32_t            Width,
                          uint32_t            Height,
                          uint32_t            Pitch,
                          r_wm_WinColorFmt_t  ColorFmt,
                          r_wm_Window_t*      Window,
                          r_wm_WinBuffer_t *ExtFB)
{
    r_wm_Error_t wmErr;
    
    /* clear */
    memset(Window, 0, sizeof(r_wm_Window_t));



	Window->Mode		= R_WM_WINMODE_FB;
	Window->Status	= R_WM_WINSTATUS_NOT_INITIALIZED;
	Window->ColorFmt	= R_WM_COLORFMT_ARGB8888;
	Window->Alpha	= 0xff;
	Window->PosX		= 00;
	Window->PosY		= 00;
	Window->PosZ		= 00;
	Window->Width	= Width;
	Window->Height	= Height;
	Window->Pitch	= Pitch;
	Window->Surface.Fb.BufMode 	= R_WM_WINBUF_ALLOC_EXTERNAL;
	Window->Surface.Fb.BufNum	= 2;
	Window->Surface.Fb.Buffer	= ExtFB;

	
	
    wmErr = R_WM_WindowCreate(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
        if (R_WM_ERR_MALLOC_FAILED == wmErr)
        {
            R_BSP_STDIO_Printf("Not enough space for Window Framebuffers! "
                "Please consider changing VOWE to Ringbuffer mode to reduce the ringbuffer size or reduce display resolution.\n");
        }
        loc_Error(1);
    }
    wmErr = R_WM_WindowEnable(LOC_WM_UNIT, Window);
    if(R_WM_ERR_OK != wmErr)
    {
       loc_Error(1);
    }
    return Window;
}

void *Copy_And_Get(void **addr_pic, uint32_t size ) 
{
    if (*addr_pic >= (void *)_PIC_BASE)
    {
        memcpy((void *)(BASE_IMG_SDRAM + (*addr_pic - (void *)_PIC_BASE)), *addr_pic, size);
        *addr_pic = (void *)(BASE_IMG_SDRAM + (*addr_pic - (void *)_PIC_BASE));
    }
    return *addr_pic;
/*
    if (*addr_pic >= (void *)_PIC_START_ADDR)
    {
        memcpy((void *)(sdram_img_addr ), *addr_pic, size);
        *addr_pic = (void *)(sdram_img_addr);
    }
    sdram_img_addr = *addr_pic + size;    
    return *addr_pic;
*/
}

void *loc_CreateRLEWindow(r_wm_WinMode_t      Mode,
                          uint32_t              PosX,
                          uint32_t              PosY,
                          uint32_t              PosZ,
                          uint32_t              Width,
                          uint32_t              Height,
                          uint32_t              Pitch,
                          r_wm_WinColorFmt_t    ColorFmt,
                          r_wm_Window_t*        Window
                          )
{
    r_wm_Error_t wmErr;
    /* clear */
    memset(Window, 0, sizeof(r_wm_Window_t));

    Window->Mode      = Mode;
    Window->Status    = R_WM_WINSTATUS_NOT_INITIALIZED;
    Window->ColorFmt  = ColorFmt;
    Window->Alpha     = 0xff;
    Window->PosX      = PosX;
    Window->PosY      = PosY;
    Window->PosZ      = PosZ;
    Window->Width     = Width;
    Window->Height    = Height;
    Window->Pitch     = (((Width * 4 + 127)& ~127) + 128) / 4;;

	RLE_buf.Status = R_WM_WINBUF_FREE;
    //RLE_buf.Data   = _PIC_C_BG_RLE; 
    RLE_buf.Data   = (void *)Copy_And_Get(&addr_pic_background, 217190); //_PIC_C_BG_RLE, size


	if( Mode == R_WM_WINMODE_FB )
	{
		Window->Surface.Fb.BufMode = R_WM_WINBUF_ALLOC_EXTERNAL;
		Window->Surface.Fb.BufNum = 1;
		Window->Surface.Fb.Buffer = &RLE_buf;
	}

    
	wmErr = R_WM_WindowCreate(LOC_WM_UNIT, Window);
	if(R_WM_ERR_OK != wmErr)
	{
		if (R_WM_ERR_MALLOC_FAILED == wmErr)
		{
			R_BSP_STDIO_Printf("RLE Not enough space for Window Framebuffers! "
				"Please consider changing VOWE to Ringbuffer mode to reduce the ringbuffer size or reduce display resolution.\n");
		}
		loc_Error(1);
	}
	wmErr = R_WM_WindowEnable(LOC_WM_UNIT, Window);
	if(R_WM_ERR_OK != wmErr)
	{
	   loc_Error(1);
	}
	
	return Window;
}


