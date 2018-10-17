/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
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
#include "w_draw.h"
#include "w_mag.h"
#include "w_canio_pics_gen.h"
#include <strings.h>

r_drw2d_Device_t tclusterDrw2dDev;

//----------------------------------------------------------------------------------------------------------------------------
r_drw2d_Error_t W_Draw_Image_By_ID(uint16_t pic_id, uint16_t xpos, uint16_t ypos)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    uint32_t pitch = pics[pic_id].width;
    r_drw2d_Texture_t tex_argb32;
    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = pitch;
    tex_argb32.Buffer.Size.Width  = pics[pic_id].width;
    tex_argb32.Buffer.Size.Height = pics[pic_id].height;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    if(pics[pic_id].ram != 1) {
        memcpy((void *)(BASE_IMG_SDRAM + pics[pic_id].address), _PIC_BASE +pics[pic_id].address, pics[pic_id].length);
        pics[pic_id].address = (void *)(BASE_IMG_SDRAM + pics[pic_id].address);
        pics[pic_id].ram = 1;
    }

    tex_argb32.Buffer.Data = pics[pic_id].address + _PIC_BASE;



    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);    // R_DRW2D_BLENDMODE_SRC_OVER
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, R_DRW2D_2X(xpos), R_DRW2D_2X(ypos));

    /* Draw texture into rectangle */
    rect.Pos.X       = R_DRW2D_2X(xpos);
    rect.Pos.Y       = R_DRW2D_2X(ypos);
    rect.Size.Width  = R_DRW2D_2X(pics[pic_id].width);
    rect.Size.Height = R_DRW2D_2X(pics[pic_id].height);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
    return ret;  
}

#ifdef NOT_USED
r_drw2d_Error_t W_Draw_Images(pic_info_t * pinfo)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    uint32_t pitch = pinfo->width;
    r_drw2d_Texture_t tex_argb32;
    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = pitch;
    tex_argb32.Buffer.Size.Width  = pinfo->width;
    tex_argb32.Buffer.Size.Height = pinfo->height;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data = pinfo->pic;

    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, R_DRW2D_2X(pinfo->xpos), R_DRW2D_2X(pinfo->ypos));

    /* Draw texture into rectangle */
    rect.Pos.X       = R_DRW2D_2X(pinfo->xpos);
    rect.Pos.Y       = R_DRW2D_2X(pinfo->ypos);
    rect.Size.Width  = R_DRW2D_2X(pinfo->width);
    rect.Size.Height = R_DRW2D_2X(pinfo->height);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
    return ret;  
}
#endif

/*============================================================================
 Variables
============================================================================*/
void verify_memory() 
{
    uint64_t size = 32*1024*1024; //32MB
    uint8_t * start = (uint8_t *)0x40000000;
    uint32_t i;
    for(i = 0; i < size; i++) {
        *start = 0xAA;
        start += 1;
    }
    start = (uint8_t *)0x40000000;
    for(i = 0; i < size; i++) {
        if(*start != 0xAA) {
            DHD_PANIC(0, "invalid device handle");
        };
        start += 1;
    }
}
