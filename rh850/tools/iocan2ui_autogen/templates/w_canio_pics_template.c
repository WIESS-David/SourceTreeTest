#include "r_dev_api.h"
#include "nios.h"
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
#include "r_drw2d_ctx_dhd.h"
#include "r_bsp_stdio_api.h"
#include "w_system.h"
#include "w_mag.h"
#include "w_canio_pics_gen.h"


pic_t pics[{{ pic_size }}];

void W_CANIO_PICS_Init()
{
{% for pic in pics %}
	pics[{{ loop.index0 }}] = (pic_t){.pid={{ loop.index0 }}, .address={{ pic[4] }}, .width={{ pic[1] }}, .height={{ pic[2] }}, .length={{ pic[3] }}, .ram=0};{% endfor %}
}