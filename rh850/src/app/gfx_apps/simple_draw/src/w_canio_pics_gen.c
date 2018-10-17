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


pic_t pics[229];

void W_CANIO_PICS_Init()
{

	pics[0] = (pic_t){.pid=0, .address=512, .width=1312, .height=480, .length=2519040, .ram=0};
	pics[1] = (pic_t){.pid=1, .address=2519552, .width=34, .height=62, .length=8432, .ram=0};
	pics[2] = (pic_t){.pid=2, .address=2528256, .width=34, .height=62, .length=8432, .ram=0};
	pics[3] = (pic_t){.pid=3, .address=2536960, .width=34, .height=62, .length=8432, .ram=0};
	pics[4] = (pic_t){.pid=4, .address=2545664, .width=34, .height=62, .length=8432, .ram=0};
	pics[5] = (pic_t){.pid=5, .address=2554368, .width=34, .height=62, .length=8432, .ram=0};
	pics[6] = (pic_t){.pid=6, .address=2563072, .width=34, .height=62, .length=8432, .ram=0};
	pics[7] = (pic_t){.pid=7, .address=2571776, .width=34, .height=62, .length=8432, .ram=0};
	pics[8] = (pic_t){.pid=8, .address=2580480, .width=34, .height=62, .length=8432, .ram=0};
	pics[9] = (pic_t){.pid=9, .address=2589184, .width=34, .height=62, .length=8432, .ram=0};
	pics[10] = (pic_t){.pid=10, .address=2597888, .width=34, .height=62, .length=8432, .ram=0};
	pics[11] = (pic_t){.pid=11, .address=2606592, .width=196, .height=118, .length=92512, .ram=0};
	pics[12] = (pic_t){.pid=12, .address=2699264, .width=12, .height=20, .length=960, .ram=0};
	pics[13] = (pic_t){.pid=13, .address=2700288, .width=86, .height=48, .length=16512, .ram=0};
	pics[14] = (pic_t){.pid=14, .address=2717184, .width=14, .height=22, .length=1232, .ram=0};
	pics[15] = (pic_t){.pid=15, .address=2718720, .width=14, .height=22, .length=1232, .ram=0};
	pics[16] = (pic_t){.pid=16, .address=2720256, .width=14, .height=22, .length=1232, .ram=0};
	pics[17] = (pic_t){.pid=17, .address=2721792, .width=14, .height=22, .length=1232, .ram=0};
	pics[18] = (pic_t){.pid=18, .address=2723328, .width=14, .height=22, .length=1232, .ram=0};
	pics[19] = (pic_t){.pid=19, .address=2724864, .width=14, .height=22, .length=1232, .ram=0};
	pics[20] = (pic_t){.pid=20, .address=2726400, .width=14, .height=22, .length=1232, .ram=0};
	pics[21] = (pic_t){.pid=21, .address=2727936, .width=14, .height=22, .length=1232, .ram=0};
	pics[22] = (pic_t){.pid=22, .address=2729472, .width=14, .height=22, .length=1232, .ram=0};
	pics[23] = (pic_t){.pid=23, .address=2731008, .width=14, .height=22, .length=1232, .ram=0};
	pics[24] = (pic_t){.pid=24, .address=2732544, .width=86, .height=48, .length=16512, .ram=0};
	pics[25] = (pic_t){.pid=25, .address=2749440, .width=34, .height=22, .length=2992, .ram=0};
	pics[26] = (pic_t){.pid=26, .address=2752512, .width=34, .height=22, .length=2992, .ram=0};
	pics[27] = (pic_t){.pid=27, .address=2755584, .width=34, .height=22, .length=2992, .ram=0};
	pics[28] = (pic_t){.pid=28, .address=2758656, .width=86, .height=48, .length=16512, .ram=0};
	pics[29] = (pic_t){.pid=29, .address=2775552, .width=86, .height=48, .length=16512, .ram=0};
	pics[30] = (pic_t){.pid=30, .address=2792448, .width=302, .height=302, .length=364816, .ram=0};
	pics[31] = (pic_t){.pid=31, .address=3157504, .width=302, .height=302, .length=364816, .ram=0};
	pics[32] = (pic_t){.pid=32, .address=3522560, .width=12, .height=20, .length=960, .ram=0};
	pics[33] = (pic_t){.pid=33, .address=3523584, .width=12, .height=20, .length=960, .ram=0};
	pics[34] = (pic_t){.pid=34, .address=3524608, .width=12, .height=20, .length=960, .ram=0};
	pics[35] = (pic_t){.pid=35, .address=3525632, .width=12, .height=20, .length=960, .ram=0};
	pics[36] = (pic_t){.pid=36, .address=3526656, .width=12, .height=20, .length=960, .ram=0};
	pics[37] = (pic_t){.pid=37, .address=3527680, .width=12, .height=20, .length=960, .ram=0};
	pics[38] = (pic_t){.pid=38, .address=3528704, .width=12, .height=20, .length=960, .ram=0};
	pics[39] = (pic_t){.pid=39, .address=3529728, .width=12, .height=20, .length=960, .ram=0};
	pics[40] = (pic_t){.pid=40, .address=3530752, .width=12, .height=20, .length=960, .ram=0};
	pics[41] = (pic_t){.pid=41, .address=3531776, .width=12, .height=20, .length=960, .ram=0};
	pics[42] = (pic_t){.pid=42, .address=3532800, .width=30, .height=20, .length=2400, .ram=0};
	pics[43] = (pic_t){.pid=43, .address=3535360, .width=24, .height=20, .length=1920, .ram=0};
	pics[44] = (pic_t){.pid=44, .address=3537408, .width=12, .height=20, .length=960, .ram=0};
	pics[45] = (pic_t){.pid=45, .address=3538432, .width=24, .height=20, .length=1920, .ram=0};
	pics[46] = (pic_t){.pid=46, .address=3540480, .width=30, .height=20, .length=2400, .ram=0};
	pics[47] = (pic_t){.pid=47, .address=3543040, .width=14, .height=18, .length=1008, .ram=0};
	pics[48] = (pic_t){.pid=48, .address=3544064, .width=14, .height=18, .length=1008, .ram=0};
	pics[49] = (pic_t){.pid=49, .address=3545088, .width=14, .height=18, .length=1008, .ram=0};
	pics[50] = (pic_t){.pid=50, .address=3546112, .width=14, .height=18, .length=1008, .ram=0};
	pics[51] = (pic_t){.pid=51, .address=3547136, .width=14, .height=18, .length=1008, .ram=0};
	pics[52] = (pic_t){.pid=52, .address=3548160, .width=14, .height=18, .length=1008, .ram=0};
	pics[53] = (pic_t){.pid=53, .address=3549184, .width=14, .height=18, .length=1008, .ram=0};
	pics[54] = (pic_t){.pid=54, .address=3550208, .width=14, .height=18, .length=1008, .ram=0};
	pics[55] = (pic_t){.pid=55, .address=3551232, .width=14, .height=18, .length=1008, .ram=0};
	pics[56] = (pic_t){.pid=56, .address=3552256, .width=14, .height=18, .length=1008, .ram=0};
	pics[57] = (pic_t){.pid=57, .address=3553280, .width=302, .height=302, .length=364816, .ram=0};
	pics[58] = (pic_t){.pid=58, .address=3918336, .width=64, .height=70, .length=17920, .ram=0};
	pics[59] = (pic_t){.pid=59, .address=3936256, .width=136, .height=32, .length=17408, .ram=0};
	pics[60] = (pic_t){.pid=60, .address=3953664, .width=154, .height=72, .length=44352, .ram=0};
	pics[61] = (pic_t){.pid=61, .address=3998208, .width=302, .height=302, .length=364816, .ram=0};
	pics[62] = (pic_t){.pid=62, .address=4363264, .width=58, .height=54, .length=12528, .ram=0};
	pics[63] = (pic_t){.pid=63, .address=4376064, .width=58, .height=54, .length=12528, .ram=0};
	pics[64] = (pic_t){.pid=64, .address=4388864, .width=58, .height=54, .length=12528, .ram=0};
	pics[65] = (pic_t){.pid=65, .address=4401664, .width=58, .height=54, .length=12528, .ram=0};
	pics[66] = (pic_t){.pid=66, .address=4414464, .width=92, .height=76, .length=27968, .ram=0};
	pics[67] = (pic_t){.pid=67, .address=4442624, .width=138, .height=30, .length=16560, .ram=0};
	pics[68] = (pic_t){.pid=68, .address=4459520, .width=138, .height=30, .length=16560, .ram=0};
	pics[69] = (pic_t){.pid=69, .address=4476416, .width=138, .height=30, .length=16560, .ram=0};
	pics[70] = (pic_t){.pid=70, .address=4493312, .width=138, .height=30, .length=16560, .ram=0};
	pics[71] = (pic_t){.pid=71, .address=4510208, .width=138, .height=30, .length=16560, .ram=0};
	pics[72] = (pic_t){.pid=72, .address=4527104, .width=138, .height=30, .length=16560, .ram=0};
	pics[73] = (pic_t){.pid=73, .address=4544000, .width=46, .height=46, .length=8464, .ram=0};
	pics[74] = (pic_t){.pid=74, .address=4552704, .width=46, .height=46, .length=8464, .ram=0};
	pics[75] = (pic_t){.pid=75, .address=4561408, .width=46, .height=46, .length=8464, .ram=0};
	pics[76] = (pic_t){.pid=76, .address=4570112, .width=46, .height=46, .length=8464, .ram=0};
	pics[77] = (pic_t){.pid=77, .address=4578816, .width=46, .height=46, .length=8464, .ram=0};
	pics[78] = (pic_t){.pid=78, .address=4587520, .width=46, .height=46, .length=8464, .ram=0};
	pics[79] = (pic_t){.pid=79, .address=4596224, .width=46, .height=46, .length=8464, .ram=0};
	pics[80] = (pic_t){.pid=80, .address=4604928, .width=46, .height=46, .length=8464, .ram=0};
	pics[81] = (pic_t){.pid=81, .address=4613632, .width=46, .height=46, .length=8464, .ram=0};
	pics[82] = (pic_t){.pid=82, .address=4622336, .width=46, .height=46, .length=8464, .ram=0};
	pics[83] = (pic_t){.pid=83, .address=4631040, .width=46, .height=46, .length=8464, .ram=0};
	pics[84] = (pic_t){.pid=84, .address=4639744, .width=46, .height=46, .length=8464, .ram=0};
	pics[85] = (pic_t){.pid=85, .address=4648448, .width=46, .height=46, .length=8464, .ram=0};
	pics[86] = (pic_t){.pid=86, .address=4657152, .width=46, .height=46, .length=8464, .ram=0};
	pics[87] = (pic_t){.pid=87, .address=4665856, .width=46, .height=46, .length=8464, .ram=0};
	pics[88] = (pic_t){.pid=88, .address=4674560, .width=46, .height=46, .length=8464, .ram=0};
	pics[89] = (pic_t){.pid=89, .address=4683264, .width=46, .height=46, .length=8464, .ram=0};
	pics[90] = (pic_t){.pid=90, .address=4691968, .width=46, .height=46, .length=8464, .ram=0};
	pics[91] = (pic_t){.pid=91, .address=4700672, .width=46, .height=46, .length=8464, .ram=0};
	pics[92] = (pic_t){.pid=92, .address=4709376, .width=46, .height=46, .length=8464, .ram=0};
	pics[93] = (pic_t){.pid=93, .address=4718080, .width=46, .height=46, .length=8464, .ram=0};
	pics[94] = (pic_t){.pid=94, .address=4726784, .width=46, .height=46, .length=8464, .ram=0};
	pics[95] = (pic_t){.pid=95, .address=4735488, .width=46, .height=46, .length=8464, .ram=0};
	pics[96] = (pic_t){.pid=96, .address=4744192, .width=46, .height=46, .length=8464, .ram=0};
	pics[97] = (pic_t){.pid=97, .address=4752896, .width=46, .height=46, .length=8464, .ram=0};
	pics[98] = (pic_t){.pid=98, .address=4761600, .width=46, .height=46, .length=8464, .ram=0};
	pics[99] = (pic_t){.pid=99, .address=4770304, .width=46, .height=46, .length=8464, .ram=0};
	pics[100] = (pic_t){.pid=100, .address=4779008, .width=46, .height=46, .length=8464, .ram=0};
	pics[101] = (pic_t){.pid=101, .address=4787712, .width=46, .height=46, .length=8464, .ram=0};
	pics[102] = (pic_t){.pid=102, .address=4796416, .width=46, .height=46, .length=8464, .ram=0};
	pics[103] = (pic_t){.pid=103, .address=4805120, .width=46, .height=46, .length=8464, .ram=0};
	pics[104] = (pic_t){.pid=104, .address=4813824, .width=46, .height=46, .length=8464, .ram=0};
	pics[105] = (pic_t){.pid=105, .address=4822528, .width=46, .height=46, .length=8464, .ram=0};
	pics[106] = (pic_t){.pid=106, .address=4831232, .width=46, .height=46, .length=8464, .ram=0};
	pics[107] = (pic_t){.pid=107, .address=4839936, .width=46, .height=46, .length=8464, .ram=0};
	pics[108] = (pic_t){.pid=108, .address=4848640, .width=46, .height=46, .length=8464, .ram=0};
	pics[109] = (pic_t){.pid=109, .address=4857344, .width=46, .height=46, .length=8464, .ram=0};
	pics[110] = (pic_t){.pid=110, .address=4866048, .width=46, .height=46, .length=8464, .ram=0};
	pics[111] = (pic_t){.pid=111, .address=4874752, .width=46, .height=46, .length=8464, .ram=0};
	pics[112] = (pic_t){.pid=112, .address=4883456, .width=46, .height=46, .length=8464, .ram=0};
	pics[113] = (pic_t){.pid=113, .address=4892160, .width=46, .height=46, .length=8464, .ram=0};
	pics[114] = (pic_t){.pid=114, .address=4900864, .width=46, .height=46, .length=8464, .ram=0};
	pics[115] = (pic_t){.pid=115, .address=4909568, .width=54, .height=50, .length=10800, .ram=0};
	pics[116] = (pic_t){.pid=116, .address=4920832, .width=54, .height=50, .length=10800, .ram=0};
	pics[117] = (pic_t){.pid=117, .address=4932096, .width=110, .height=110, .length=48400, .ram=0};
	pics[118] = (pic_t){.pid=118, .address=4980736, .width=16, .height=12, .length=768, .ram=0};
	pics[119] = (pic_t){.pid=119, .address=4981760, .width=36, .height=40, .length=5760, .ram=0};
	pics[120] = (pic_t){.pid=120, .address=4987904, .width=36, .height=40, .length=5760, .ram=0};
	pics[121] = (pic_t){.pid=121, .address=4994048, .width=36, .height=40, .length=5760, .ram=0};
	pics[122] = (pic_t){.pid=122, .address=5000192, .width=36, .height=40, .length=5760, .ram=0};
	pics[123] = (pic_t){.pid=123, .address=5006336, .width=16, .height=12, .length=768, .ram=0};
	pics[124] = (pic_t){.pid=124, .address=5007360, .width=240, .height=40, .length=38400, .ram=0};
	pics[125] = (pic_t){.pid=125, .address=5045760, .width=172, .height=28, .length=19264, .ram=0};
	pics[126] = (pic_t){.pid=126, .address=5065216, .width=172, .height=28, .length=19264, .ram=0};
	pics[127] = (pic_t){.pid=127, .address=5084672, .width=172, .height=28, .length=19264, .ram=0};
	pics[128] = (pic_t){.pid=128, .address=5104128, .width=172, .height=28, .length=19264, .ram=0};
	pics[129] = (pic_t){.pid=129, .address=5123584, .width=90, .height=58, .length=20880, .ram=0};
	pics[130] = (pic_t){.pid=130, .address=5144576, .width=90, .height=58, .length=20880, .ram=0};
	pics[131] = (pic_t){.pid=131, .address=5165568, .width=90, .height=58, .length=20880, .ram=0};
	pics[132] = (pic_t){.pid=132, .address=5186560, .width=160, .height=6, .length=3840, .ram=0};
	pics[133] = (pic_t){.pid=133, .address=5190656, .width=160, .height=6, .length=3840, .ram=0};
	pics[134] = (pic_t){.pid=134, .address=5194752, .width=302, .height=302, .length=364816, .ram=0};
	pics[135] = (pic_t){.pid=135, .address=5559808, .width=302, .height=302, .length=364816, .ram=0};
	pics[136] = (pic_t){.pid=136, .address=5924864, .width=60, .height=28, .length=6720, .ram=0};
	pics[137] = (pic_t){.pid=137, .address=5932032, .width=26, .height=24, .length=2496, .ram=0};
	pics[138] = (pic_t){.pid=138, .address=5934592, .width=24, .height=20, .length=1920, .ram=0};
	pics[139] = (pic_t){.pid=139, .address=5936640, .width=36, .height=32, .length=4608, .ram=0};
	pics[140] = (pic_t){.pid=140, .address=5941248, .width=32, .height=28, .length=3584, .ram=0};
	pics[141] = (pic_t){.pid=141, .address=5944832, .width=30, .height=24, .length=2880, .ram=0};
	pics[142] = (pic_t){.pid=142, .address=5947904, .width=30, .height=24, .length=2880, .ram=0};
	pics[143] = (pic_t){.pid=143, .address=5950976, .width=30, .height=26, .length=3120, .ram=0};
	pics[144] = (pic_t){.pid=144, .address=5954560, .width=28, .height=24, .length=2688, .ram=0};
	pics[145] = (pic_t){.pid=145, .address=5957632, .width=28, .height=24, .length=2688, .ram=0};
	pics[146] = (pic_t){.pid=146, .address=5960704, .width=28, .height=26, .length=2912, .ram=0};
	pics[147] = (pic_t){.pid=147, .address=5963776, .width=60, .height=28, .length=6720, .ram=0};
	pics[148] = (pic_t){.pid=148, .address=5970944, .width=26, .height=24, .length=2496, .ram=0};
	pics[149] = (pic_t){.pid=149, .address=5973504, .width=24, .height=20, .length=1920, .ram=0};
	pics[150] = (pic_t){.pid=150, .address=5975552, .width=36, .height=32, .length=4608, .ram=0};
	pics[151] = (pic_t){.pid=151, .address=5980160, .width=32, .height=28, .length=3584, .ram=0};
	pics[152] = (pic_t){.pid=152, .address=5983744, .width=30, .height=24, .length=2880, .ram=0};
	pics[153] = (pic_t){.pid=153, .address=5986816, .width=30, .height=24, .length=2880, .ram=0};
	pics[154] = (pic_t){.pid=154, .address=5989888, .width=30, .height=26, .length=3120, .ram=0};
	pics[155] = (pic_t){.pid=155, .address=5993472, .width=28, .height=24, .length=2688, .ram=0};
	pics[156] = (pic_t){.pid=156, .address=5996544, .width=28, .height=24, .length=2688, .ram=0};
	pics[157] = (pic_t){.pid=157, .address=5999616, .width=28, .height=26, .length=2912, .ram=0};
	pics[158] = (pic_t){.pid=158, .address=6002688, .width=60, .height=28, .length=6720, .ram=0};
	pics[159] = (pic_t){.pid=159, .address=6009856, .width=26, .height=24, .length=2496, .ram=0};
	pics[160] = (pic_t){.pid=160, .address=6012416, .width=24, .height=20, .length=1920, .ram=0};
	pics[161] = (pic_t){.pid=161, .address=6014464, .width=36, .height=32, .length=4608, .ram=0};
	pics[162] = (pic_t){.pid=162, .address=6019072, .width=32, .height=28, .length=3584, .ram=0};
	pics[163] = (pic_t){.pid=163, .address=6022656, .width=30, .height=24, .length=2880, .ram=0};
	pics[164] = (pic_t){.pid=164, .address=6025728, .width=30, .height=24, .length=2880, .ram=0};
	pics[165] = (pic_t){.pid=165, .address=6028800, .width=30, .height=26, .length=3120, .ram=0};
	pics[166] = (pic_t){.pid=166, .address=6032384, .width=28, .height=24, .length=2688, .ram=0};
	pics[167] = (pic_t){.pid=167, .address=6035456, .width=28, .height=24, .length=2688, .ram=0};
	pics[168] = (pic_t){.pid=168, .address=6038528, .width=28, .height=26, .length=2912, .ram=0};
	pics[169] = (pic_t){.pid=169, .address=6041600, .width=60, .height=28, .length=6720, .ram=0};
	pics[170] = (pic_t){.pid=170, .address=6048768, .width=26, .height=24, .length=2496, .ram=0};
	pics[171] = (pic_t){.pid=171, .address=6051328, .width=24, .height=20, .length=1920, .ram=0};
	pics[172] = (pic_t){.pid=172, .address=6053376, .width=36, .height=32, .length=4608, .ram=0};
	pics[173] = (pic_t){.pid=173, .address=6057984, .width=32, .height=28, .length=3584, .ram=0};
	pics[174] = (pic_t){.pid=174, .address=6061568, .width=30, .height=24, .length=2880, .ram=0};
	pics[175] = (pic_t){.pid=175, .address=6064640, .width=30, .height=24, .length=2880, .ram=0};
	pics[176] = (pic_t){.pid=176, .address=6067712, .width=30, .height=26, .length=3120, .ram=0};
	pics[177] = (pic_t){.pid=177, .address=6071296, .width=28, .height=24, .length=2688, .ram=0};
	pics[178] = (pic_t){.pid=178, .address=6074368, .width=28, .height=24, .length=2688, .ram=0};
	pics[179] = (pic_t){.pid=179, .address=6077440, .width=28, .height=26, .length=2912, .ram=0};
	pics[180] = (pic_t){.pid=180, .address=6080512, .width=302, .height=302, .length=364816, .ram=0};
	pics[181] = (pic_t){.pid=181, .address=6445568, .width=92, .height=32, .length=11776, .ram=0};
	pics[182] = (pic_t){.pid=182, .address=6457344, .width=92, .height=32, .length=11776, .ram=0};
	pics[183] = (pic_t){.pid=183, .address=6469120, .width=54, .height=50, .length=10800, .ram=0};
	pics[184] = (pic_t){.pid=184, .address=6480384, .width=50, .height=74, .length=14800, .ram=0};
	pics[185] = (pic_t){.pid=185, .address=6495232, .width=50, .height=74, .length=14800, .ram=0};
	pics[186] = (pic_t){.pid=186, .address=6510080, .width=50, .height=74, .length=14800, .ram=0};
	pics[187] = (pic_t){.pid=187, .address=6524928, .width=50, .height=74, .length=14800, .ram=0};
	pics[188] = (pic_t){.pid=188, .address=6539776, .width=50, .height=74, .length=14800, .ram=0};
	pics[189] = (pic_t){.pid=189, .address=6554624, .width=50, .height=74, .length=14800, .ram=0};
	pics[190] = (pic_t){.pid=190, .address=6569472, .width=50, .height=74, .length=14800, .ram=0};
	pics[191] = (pic_t){.pid=191, .address=6584320, .width=50, .height=74, .length=14800, .ram=0};
	pics[192] = (pic_t){.pid=192, .address=6599168, .width=50, .height=74, .length=14800, .ram=0};
	pics[193] = (pic_t){.pid=193, .address=6614016, .width=50, .height=74, .length=14800, .ram=0};
	pics[194] = (pic_t){.pid=194, .address=6628864, .width=82, .height=28, .length=9184, .ram=0};
	pics[195] = (pic_t){.pid=195, .address=6638080, .width=302, .height=302, .length=364816, .ram=0};
	pics[196] = (pic_t){.pid=196, .address=7003136, .width=46, .height=46, .length=8464, .ram=0};
	pics[197] = (pic_t){.pid=197, .address=7011840, .width=46, .height=46, .length=8464, .ram=0};
	pics[198] = (pic_t){.pid=198, .address=7020544, .width=16, .height=26, .length=1664, .ram=0};
	pics[199] = (pic_t){.pid=199, .address=7022592, .width=16, .height=26, .length=1664, .ram=0};
	pics[200] = (pic_t){.pid=200, .address=7024640, .width=16, .height=26, .length=1664, .ram=0};
	pics[201] = (pic_t){.pid=201, .address=7026688, .width=16, .height=26, .length=1664, .ram=0};
	pics[202] = (pic_t){.pid=202, .address=7028736, .width=16, .height=26, .length=1664, .ram=0};
	pics[203] = (pic_t){.pid=203, .address=7030784, .width=16, .height=26, .length=1664, .ram=0};
	pics[204] = (pic_t){.pid=204, .address=7032832, .width=16, .height=26, .length=1664, .ram=0};
	pics[205] = (pic_t){.pid=205, .address=7034880, .width=16, .height=26, .length=1664, .ram=0};
	pics[206] = (pic_t){.pid=206, .address=7036928, .width=16, .height=26, .length=1664, .ram=0};
	pics[207] = (pic_t){.pid=207, .address=7038976, .width=16, .height=26, .length=1664, .ram=0};
	pics[208] = (pic_t){.pid=208, .address=7041024, .width=8, .height=26, .length=832, .ram=0};
	pics[209] = (pic_t){.pid=209, .address=7042048, .width=8, .height=26, .length=832, .ram=0};
	pics[210] = (pic_t){.pid=210, .address=7043072, .width=8, .height=10, .length=320, .ram=0};
	pics[211] = (pic_t){.pid=211, .address=7043584, .width=8, .height=10, .length=320, .ram=0};
	pics[212] = (pic_t){.pid=212, .address=7044096, .width=8, .height=10, .length=320, .ram=0};
	pics[213] = (pic_t){.pid=213, .address=7044608, .width=8, .height=10, .length=320, .ram=0};
	pics[214] = (pic_t){.pid=214, .address=7045120, .width=8, .height=10, .length=320, .ram=0};
	pics[215] = (pic_t){.pid=215, .address=7045632, .width=8, .height=10, .length=320, .ram=0};
	pics[216] = (pic_t){.pid=216, .address=7046144, .width=8, .height=10, .length=320, .ram=0};
	pics[217] = (pic_t){.pid=217, .address=7046656, .width=8, .height=10, .length=320, .ram=0};
	pics[218] = (pic_t){.pid=218, .address=7047168, .width=8, .height=10, .length=320, .ram=0};
	pics[219] = (pic_t){.pid=219, .address=7047680, .width=8, .height=10, .length=320, .ram=0};
	pics[220] = (pic_t){.pid=220, .address=7048192, .width=20, .height=10, .length=800, .ram=0};
	pics[221] = (pic_t){.pid=221, .address=7049216, .width=6, .height=10, .length=240, .ram=0};
	pics[222] = (pic_t){.pid=222, .address=7049728, .width=20, .height=10, .length=800, .ram=0};
	pics[223] = (pic_t){.pid=223, .address=7050752, .width=38, .height=42, .length=6384, .ram=0};
	pics[224] = (pic_t){.pid=224, .address=7057408, .width=38, .height=42, .length=6384, .ram=0};
	pics[225] = (pic_t){.pid=225, .address=7064064, .width=44, .height=44, .length=7744, .ram=0};
	pics[226] = (pic_t){.pid=226, .address=7072256, .width=1000, .height=130, .length=520000, .ram=0};
	pics[227] = (pic_t){.pid=227, .address=7592448, .width=1000, .height=130, .length=520000, .ram=0};
	pics[228] = (pic_t){.pid=228, .address=8112640, .width=1312, .height=480, .length=216678, .ram=0};
}