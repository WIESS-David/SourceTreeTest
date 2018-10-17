//============================================================================================================
//
// Header file for the system flag definitions
//
//============================================================================================================
#ifndef _W_DRAW_H_
#define _W_DRAW_H_

#include "r_typedefs.h"

/* NOT USED
typedef struct
{
    void * pic;
    uint32_t xpos;
    uint32_t ypos;
    uint32_t zlayer;
    uint32_t width;
    uint32_t height;
} pic_info_t;

r_drw2d_Error_t W_Draw_Images(pic_info_t * pinfo);
*/

r_drw2d_Error_t W_Draw_Image_By_ID(uint16_t pic_id, uint16_t xpos, uint16_t ypos);

#endif
