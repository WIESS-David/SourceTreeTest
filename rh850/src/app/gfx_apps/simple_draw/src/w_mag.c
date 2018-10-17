/*============================================================================
 Includes
============================================================================*/
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
#include "w_draw.h"
#include "main.h"

void * addr_pic_logo = (void *)_PIC_S_LOGO;
void * addr_pic_logo_black = (void *)_PIC_S_BLACK;

static struct r_drw2d_DeviceDHD_s tclusterDHDDev;
extern r_drw2d_Device_t tclusterDrw2dDev;
extern uint32_t             loc_GPU_Memory_Addr;
extern r_cdi_Heap_t         loc_lRAM_heap;
extern r_cdi_Heap_t         loc_VRAM_heap;
extern mars_info_def        gtmars_info;
extern queue_task_def       gtqueue_manager;

r_wm_Window_t tcluster_rle_win;
r_wm_Window_t tcluster_draw2d_window;
//static r_wm_Window_t tcluster_sprite_window;
r_wm_WinBuffer_t tcluster_window_buffer[2];
r_wm_Sprite_t tclusterIcon_array[12];
r_drw2d_Framebuffer_t tcluster_draw2d_fb;

extern const uint8_t c8sport_gear_d_pic[19600];
extern const uint8_t c8sport_gear_r_pic[19600];
extern const uint8_t c8standard_gear_r_pic[3600];
extern const uint8_t c8standard_gear_d_pic[3600];

extern const uint8_t Km_hPIC[7680] ;
extern const uint8_t mphPIC[7560];
uint8_t u8cluter_icon_init = 0;

extern const uint8_t Icon1_ImgData[3600];
extern const uint8_t Icon2_ImgData[3600];
extern const uint8_t Icon3_ImgData[3600];
extern const uint8_t Icon4_ImgData[3600];
extern const uint8_t Icon5_ImgData[3600];
extern const uint8_t Icon6_ImgData[3600];
extern const uint8_t Icon7_ImgData[3600];
extern const uint8_t Icon8_ImgData[3600];
extern const uint8_t Icon9_ImgData[3600];
extern const uint8_t Icon10_ImgData[3600];
extern const uint8_t RightIcon_ImgData[3600] ;
extern const uint8_t LeftIcon_ImgData[3600] ;
extern const uint8_t PointS_ImgData[1440] ;
extern const uint8_t PointB_ImgData[6400] ;
extern const uint8_t u8logo_background_ImgData[2457600];
//extern const uint8_t u8logo_move_ImgData[520000];
extern const uint8_t u8small_logo_ImgData[543744];
uint8_t u8cluster_demo_iconflag = 0;

extern const uint8_t u8test_black_RData[521184];
extern const uint8_t u8test_black_RABData[521184];


const uint16_t u16leftline_parameter[11][4] = {// width,hight,x,y
                                        {60, 28, 397, 452},
                                        {36, 32, 436, 421},
                                        {32, 28, 453, 393},
                                        {30, 24, 467, 369},
                                        {30, 24, 479, 345},
                                        {30, 26, 491, 320},
                                        {28, 24, 505, 296},
                                        {28, 24, 517, 272},
                                        {28, 26, 529, 247},
                                        {26, 24, 543, 223},
                                        {24, 20, 555, 204}
                                        };
                                        

const uint16_t u16rightline_parameter[11][4] = {// width,hight,x,y
                                                {60,28,827,452},
                                                {36,32,812,421},
                                                {32,28,799,393},
                                                {30,24,787,369},
                                                {30,24,775,345},
                                                {30,26,763,320},
                                                {28,24,751,296},
                                                {28,24,739,272},
                                                {28,26,727,247},
                                                {26,24,715,223},
                                                {24,20,705,204}
                                            };

const handle_t tgreenline_left_pic_addr[11] = { (handle_t)PIC_ID_C_PEDAL_L_G_1,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_2,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_3,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_4,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_5,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_6,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_7,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_8,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_9,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_10,
                                                (handle_t)PIC_ID_C_PEDAL_L_G_11
                                                };
const handle_t tgreenline_right_pic_addr[11] = {(handle_t)PIC_ID_C_PEDAL_R_G_1,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_2,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_3,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_4,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_5,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_6,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_7,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_8,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_9,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_10,
                                                (handle_t)PIC_ID_C_PEDAL_R_G_11
                                                };
const handle_t tblueline_right_pic_addr[11] = {  (handle_t)PIC_ID_C_PEDAL_R_B_1,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_2,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_3,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_4,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_5,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_6,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_7,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_8,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_9,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_10,
                                                (handle_t)PIC_ID_C_PEDAL_R_B_11
                                                };
const handle_t tblueline_left_pic_addr[11] = {  (handle_t)PIC_ID_C_PEDAL_L_B_1,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_2,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_3,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_4,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_5,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_6,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_7,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_8,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_9,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_10,
                                                (handle_t)PIC_ID_C_PEDAL_L_B_11
                                                };

const handle_t tcluster_speed_num_address[10] = {
                                            (handle_t)PIC_ID_C_SPEED_0,
                                            (handle_t)PIC_ID_C_SPEED_1,
                                            (handle_t)PIC_ID_C_SPEED_2,
                                            (handle_t)PIC_ID_C_SPEED_3,
                                            (handle_t)PIC_ID_C_SPEED_4,
                                            (handle_t)PIC_ID_C_SPEED_5,
                                            (handle_t)PIC_ID_C_SPEED_6,
                                            (handle_t)PIC_ID_C_SPEED_7,
                                            (handle_t)PIC_ID_C_SPEED_8,
                                            (handle_t)PIC_ID_C_SPEED_9
                                            };
const handle_t tcluster_door_pic_addr[6] = {(handle_t)PIC_ID_C_DOOR_BG,
                                            (handle_t)PIC_ID_C_DOOR_FL,
                                            (handle_t)PIC_ID_C_DOOR_FR,
                                            (handle_t)PIC_ID_C_DOOR_RL,
                                            (handle_t)PIC_ID_C_DOOR_RR,
                                            (handle_t)PIC_ID_C_DOOR_TAIL
                                            };
const uint16_t tcluster_door_pic_parameter[6][4] = {// xpos,ypos,width,hight
                                                        {110,134,302,302},
                                                        {174,233,58,54},
                                                        {289,233,58,54},
                                                        {174,284,58,54},
                                                        {289,284,58,54},
                                                        {215,346,92,76}
                                                    };

uint16_t u16chart_value[20] = {120, 180, 130, 190, 110, 180, 100, 200, 150, 170,
                               100, 140,180, 220, 250, 0, 120, 120, 180, 120};
uint16_t u16chart_draw_posy[20];
uint16_t u16char_draw_posx[20] = {914, 926, 938, 950, 962, 974, 986, 998, 1010, 1022,
                                1034, 1046, 1058, 1070, 1082, 1094, 1106, 1118, 1130, 1142};

uint32_t u32_Sys_Timer_Count = 0;
uint32_t u32_FPS_Count = 0;
extern void * addr_pic_background;
extern void W_SYS_Main_Entry(void);
extern void loc_JcuaRun(void);
extern void W_APPMCU_Status(uint8_t u8temp);
extern void W_APPBKL_Adjust(uint8_t u8duty);
extern void* loc_StartSFMA(void);


//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon1(void)
{

    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_01_SEATBELT_C, 10, 125);
    else if(gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.u8cabin_status_byte == 0x07)
        return;
    else 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_01_SEATBELT_W, 10, 125);    
}

void W_Cluster_Icon2(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_02_EPS_C, 10, 170);
}

void W_Cluster_Icon3(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_03_ABS_C, 10, 225);    
}

void W_Cluster_Icon4(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_04_AIRBAG_C, 10, 275);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon5(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_05_LOW_VOLTAGE_C, 10, 325);
    else if(gtmars_info.tcar_info.tcan_id134_info.tfault_code2_info.battery_12v_under_voltage_fault  == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_05_LOW_VOLTAGE_W, 10, 325);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon6(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_06_COOLANT_C, 10, 375);
    else if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.over_temperature_fault == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_06_COOLANT_W, 10, 375);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon7(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_07_HIGH_VOLTAGE_C, 60, 125);
    else if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.hv_sys_fault == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_07_HIGH_VOLTAGE_W, 60, 125);
    else 
        return;    
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon8(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_08_MOTOR_C, 60, 175);
    else if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.drive_sys_fault == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_08_MOTOR_W, 60, 175);
    else 
        return;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon9(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_09_TRANSMISSION_C, 60, 225);
    else if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.transmission_sys_fault == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_09_TRANSMISSION_W, 60, 225);
    else 
        return;  
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon10(void)
{
    if(gtmars_info.tcluster_info.u8acc_on_check_over == _G_FAIL )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_10_TIRE_C, 60, 275);
    else if(gtmars_info.tcar_info.tcan_id340_info.toperation2_mode_flag_info.tpms_flag  == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_10_TIRE_W, 60, 275);
    else 
        return;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon13(void)
{
    if(gtmars_info.tcar_info.tcan_id434_info.toperation1_mode_flag_info.head_lamp_malfunction_flag == _G_TRUE )
        W_Draw_Image_By_ID(PIC_ID_C_ICON_13_LAMP_MALFUNCTIONS, 1225, 125);
    else if(gtmars_info.tcar_info.tcan_id434_info.tlamp_status_info.u8head_lamp_auto == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_13_HEADLAMP_AUTO, 1225, 125);
    else if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.tail_lamp == _G_TRUE) ||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.tail_lamp == _G_TRUE))
        W_Draw_Image_By_ID(PIC_ID_C_ICON_13_POSITION_LAMP, 1225, 125);
    else 
        return;    
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon14(void)
{
    if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.head_lamp_high == _G_TRUE) ||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_h == _G_TRUE))
        W_Draw_Image_By_ID(PIC_ID_C_ICON_14_HEADLAMP_HIGH, 1225, 175);
    else if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.head_lamp_low == _G_TRUE )||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.head_lamp_l == _G_TRUE))
        W_Draw_Image_By_ID(PIC_ID_C_ICON_14_HEADLAMP_LOW, 1225, 175);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon15(void)
{
    if((gtmars_info.tcar_info.tcan_id434_info.twiper_status_info.u8wiper_status_byte && 0x03) >= 0x01)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_15_WIPER_HIGHLOW, 1225, 225);
    else if(gtmars_info.tcar_info.tcan_id434_info.twiper_status_info.wiper_intermittent == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_15_WIPER_INTERMITTENT, 1225, 225);
    else if(gtmars_info.tcar_info.tcan_id434_info.twiper_status_info.screen_washing_on == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_15_WIPER_WASHING, 1225, 225);
    else if(gtmars_info.tcar_info.tcan_id434_info.twiper_status_info.wiper_auto_on == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_15_WIPER_AUTO, 1225, 225);
    else if(gtmars_info.tcar_info.tcan_id434_info.toperation1_mode_flag_info.wiper_malfunction_flag == _G_TRUE ) 
        W_Draw_Image_By_ID(PIC_ID_C_ICON_15_WIPER_W, 1225, 225);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon16(void)  //FIXME: use W_Draw_Images
{
    if(gtmars_info.tcar_info.tcan_id134_info.tpowertrain_status_info.cruise_driving == 0x01)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_16_CRUISE_ACTIVATED_G, 1225, 275);
    else if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.cruising_sys_fault == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_16_CRUISE_W, 1225, 275);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon17(void)  //FIXME: use W_Draw_Images
{
    if((gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.u8door_close_status_byte && 0x0F) >= 0x01)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_17_DOOR_W, 1225, 325);
    else if (gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.rear_Tailgate  == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_17_DOOR_TAIL_W, 1225, 325);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon18(void)  //FIXME: use W_Draw_Images
{
    if(gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.brake_sys_fault == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_18_BRAKE_W, 1225, 375);
    else if (gtmars_info.tcar_info.tcan_id134_info.tfault_code2_info.parkin_brake_fault == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_ICON_18_BRAKE_P, 1225, 375);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon19(void)  //FIXME: use W_Draw_Images
{
    if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.front_fog == _G_TRUE) ||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.f_fog_lamp == _G_TRUE))
        W_Draw_Image_By_ID(PIC_ID_C_ICON_19_FOGLAMP_F, 1175, 125);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon20(void)  //FIXME: use W_Draw_Images
{
    if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.rear_fog == _G_TRUE) ||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_fog_lamp == _G_TRUE))
        W_Draw_Image_By_ID(PIC_ID_C_ICON_20_FOGLAMP_R, 1175, 175);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Icon_Process()
{
    W_Cluster_Icon1();
    W_Cluster_Icon2();
    W_Cluster_Icon3();
    W_Cluster_Icon4();
    W_Cluster_Icon5();
    W_Cluster_Icon6();
    W_Cluster_Icon7();
    W_Cluster_Icon8();
    W_Cluster_Icon9();
    W_Cluster_Icon10();
    W_Cluster_Icon13(); // tail lamp
    W_Cluster_Icon14(); // head lamp hight / head lamp low
    W_Cluster_Icon15();
    W_Cluster_Icon16();
    W_Cluster_Icon17();
    W_Cluster_Icon18();
    W_Cluster_Icon19(); // front fog lamp
    W_Cluster_Icon20(); // rear fog lamp
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Menu_Button()
{
    W_Draw_Image_By_ID(PIC_ID_C_MENU_BUTTON, 5, 1);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Menu_BG()
{
    W_Draw_Image_By_ID(PIC_ID_C_MENU_WORD_BG, 104, 36);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Battery_BG()
{
    W_Draw_Image_By_ID(PIC_ID_C_BATTERY, 1168, 24);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Demo_Battery()
{
    W_Draw_Image_By_ID(PIC_ID_C_BATTERY_G, 1168, 24);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Battery_Uint()
{
    W_Draw_Image_By_ID(PIC_ID_C_BATTERY_ICON_G, 1133, 36);
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Battery_Demo_Num(uint8_t bat_num)
{
    if(bat_num == 0)
        W_Draw_Image_By_ID(PIC_ID_C_BATTERY_8, 1105, 36);
    else
        W_Draw_Image_By_ID(PIC_ID_C_BATTERY_5, 1119, 36);
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Battery_Process()
{
    W_Cluster_Battery_BG();
    W_Cluster_Demo_Battery();
    W_Cluster_Battery_Demo_Num(0);
    W_Cluster_Battery_Demo_Num(1);
    W_Cluster_Battery_Uint();
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_SpeedUint()
{
    W_Draw_Image_By_ID(PIC_ID_C_SPEED_KMH, 602, 320);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Draw_Turnlight(uint8_t u8disp_status)
{
    if(u8disp_status == 0)
        W_Draw_Image_By_ID(PIC_ID_C_TURN_L, 397, 36);
    else
        W_Draw_Image_By_ID(PIC_ID_C_TURN_R, 835, 36);    
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Disp_Turnlight(void)
{
    if(((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.left_turn == _G_TRUE) &&
       (gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.right_turn == _G_TRUE)) ||
       ((gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp == _G_TRUE) &&
       (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp == _G_TRUE)))
    {
        W_Cluster_Draw_Turnlight(0);
        W_Cluster_Draw_Turnlight(1);
    }
    if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.left_turn == _G_TRUE) ||
       (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.l_turn_lamp == _TRUE))
        W_Cluster_Draw_Turnlight(0);
    else if((gtmars_info.tcar_info.tcan_id287_info.tlamp_form_can_info.right_turn == _G_TRUE) ||
        (gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.r_turn_lamp == _G_TRUE))
        W_Cluster_Draw_Turnlight(1);
}

void W_Draw_Line(uint16_t x, uint16_t y, uint16_t x2, uint16_t y2, 
        uint16_t width, uint32_t color) 
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_LineStyle_t LineStyle;
    r_drw2d_Point_t Points[2] = {
        { 0, 0}, { 0,00} };
    Points[0].X = R_DRW2D_2X(x);
    Points[0].Y = R_DRW2D_2X(y);
    Points[1].X = R_DRW2D_2X(x2);
    Points[1].Y = R_DRW2D_2X(y2);
    LineStyle.LineJoin = R_DRW2D_LINEJOIN_NONE;
    LineStyle.LineCap = R_DRW2D_LINECAP_SQUARE;
    LineStyle.Width = R_DRW2D_2X(width);
    LineStyle.MiterLimit = R_DRW2D_2X(1);
    LineStyle.IsClosed = R_FALSE;
    ret |= R_DRW2D_CtxLineStyle(tclusterDrw2dDev, &LineStyle);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_SOLID);
    ret |= R_DRW2D_CtxFgColor(tclusterDrw2dDev, color);
    ret |= R_DRW2D_DrawLines(tclusterDrw2dDev, Points, 2);
}

void W_Cluster_Debug_OSD() 
{
    const uint16_t L_WIDTH = 300;
    const uint16_t baseX = 0;
    const uint16_t baseY = 0;

    W_Draw_Line(baseX, baseY, baseX + L_WIDTH, baseY, 5, 0xffff6000);
    /* 0 ~ 30 map to 0 ~ L_WIDTH pixels */
    W_Draw_Line(baseX, baseY+10, baseX + (uint16_t)(L_WIDTH * ((float32_t)u32_FPS_Count*100 / u32_Sys_Timer_Count / 30)), baseY+10, 5, 0xffff0000);
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Dispay_Num(uint8_t u8number, uint16_t u16drawx)
{
    W_Draw_Image_By_ID(tcluster_speed_num_address[u8number], u16drawx, 237);
}

//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Sport_Speed(void)
{
    uint8_t u8num[4];
    uint8_t u8speed;
    if(gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed >= 40)
        u8speed = gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed - 40;
    else
        u8speed = 0;
    u8num[0] = (u8speed / 100);
    u8num[1] = ((u8speed %100) / 10);
    u8num[2] = ((u8speed %100) %10) ;
    if(u8speed < 10)
        W_Cluster_Dispay_Num(u8num[2],618);
    else if (u8speed >9 && u8speed < 100)
    {
        W_Cluster_Dispay_Num(u8num[1],592);
        W_Cluster_Dispay_Num(u8num[2],642);
    }
    else
    {
        W_Cluster_Dispay_Num(u8num[0],567);
        W_Cluster_Dispay_Num(u8num[1],617);
        W_Cluster_Dispay_Num(u8num[2],667);
    }
    W_Cluster_SpeedUint();
}
//----------------------------------------------------------------------------------------------------------------------------
static r_drw2d_Boolean_t W_DRW2DErrorHandler(r_drw2d_Error_t  Error, void *UserData)
{
    while(1)
    {
    }
    //return R_TRUE;
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_CreateWindows(void)
{
    tcluster_window_buffer[0].Status	= R_WM_WINBUF_FREE;
    tcluster_window_buffer[1].Status	= R_WM_WINBUF_FREE;
    tcluster_window_buffer[0].Data      = (void*)D1M1A_SDRAM0;  
    tcluster_window_buffer[1].Data      = (void*)D1M1A_SDRAM1;
    loc_CreateRLEWindow(R_WM_WINMODE_FB,
                         0,
                         0,
                         0,
                         1280,//30,//1280,//320,  //1024, //1280, //400,
                         480,//30,//480,//240,   //384,  //480,  //400,
                         1312,//64,//1312,//352,  //1056, //1312, //448,
                         R_WM_COLORFMT_RLE24ARGB8888,
                         &tcluster_rle_win);
                 
    loc_CreateWindow_EXT(R_WM_WINMODE_FB,
            			 0,
            			 0,
            			 1,
            			 _CLUSTER_WIDTH,
            			 _CLUSTER_Hight,
            			 _CLUSTER_STRIDE,
            			 R_WM_COLORFMT_ARGB8888,
            			 &tcluster_draw2d_window,
            			 tcluster_window_buffer);

/*
    loc_CreateWindow(R_WM_WINMODE_SPRITES,
                     0,
                     0,
                     2,
                     1000,
                     400,
                     2048, // HW restriction 
                     R_WM_COLORFMT_ARGB8888,
                     &tcluster_sprite_window);      
                     */     
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_WM_End(uint8_t WMUnit, uint8_t id)
{
    R_WM_FrameEndMark(WMUnit, id);
    R_WM_FrameWait(WMUnit, id);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_MAG_WindowSwap(void)
{
    R_DRW2D_GpuFinish(tclusterDrw2dDev, R_DRW2D_FINISH_WAIT);
    R_WM_WindowSwap(LOC_WM_UNIT, &tcluster_draw2d_window);
    //R_WM_WindowSwap(LOC_WM_UNIT, &tcluster_rle_win); /* NO NEED TO CALL THIS */
    W_Cluster_WM_End(LOC_WM_UNIT,0);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Draw2DInit(void)
{
    //verify_memory(); 
    /* start up gfx engine */
    R_DRW2D_Init();
    R_DRW2D_GlobalErrCallbackSet(&W_DRW2DErrorHandler, R_NULL);
    R_DRW2D_Open(LOC_DRW2D_UNIT, R_DRW2D_UNIT_DHD0, &tclusterDHDDev, &tclusterDrw2dDev);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_WM_Init(void)
{
    W_BSP_SYS_GFX_Init();
    loc_SetupMemManager();
    R_UTIL_DHD_Init(LOC_DHD_UNIT);
    R_UTIL_DHD_Config((dhd_gpu_ptr_t)loc_GPU_Memory_Addr, DHD_MEMORY_SIZE, &loc_lRAM_heap);
    loc_InitWM(&loc_lRAM_heap, &loc_VRAM_heap);
    W_Cluster_WM_End( LOC_WM_UNIT, 0);
#ifdef _MAG
    loc_StartSFMA();
#endif
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_IconClose(void)
{
    uint8_t u8icon_num;
    for(u8icon_num = 0; u8icon_num < 12; u8icon_num ++)
    {
        R_WM_SpriteDisable(LOC_WM_UNIT, &tclusterIcon_array[u8icon_num]);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_IconDelete(void)
{
    uint8_t u8icon_num;
    for(u8icon_num = 0; u8icon_num < 12; u8icon_num ++)
    {
        R_WM_SpriteDelete(LOC_WM_UNIT, &tclusterIcon_array[u8icon_num]);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Draw2d_Clear(uint32_t u32clear_color)
{
    /* Set green-ish clear color and fill framebuffer */
    R_DRW2D_CtxBgColor(tclusterDrw2dDev, u32clear_color);
    R_DRW2D_FramebufferClear(tclusterDrw2dDev);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Door_Background(void)
{
    W_Draw_Image_By_ID(tcluster_door_pic_addr[_DOOR_BG], tcluster_door_pic_parameter[_DOOR_BG][0], tcluster_door_pic_parameter[_DOOR_BG][1]);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Door_Open(uint8_t u8door_position)
{
    W_Draw_Image_By_ID(tcluster_door_pic_addr[u8door_position], tcluster_door_pic_parameter[u8door_position][0], tcluster_door_pic_parameter[u8door_position][1]);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Door_Process(void)
{
    
    W_Cluster_Door_Background();
    if(gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.front_left == _G_TRUE)
        W_Cluster_Door_Open(_DOOR_FRONT_LEFT);
    if(gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.front_right == _G_TRUE)
        W_Cluster_Door_Open(_DOOR_FRONT_RIGHT);
    if(gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.rear_left == _G_TRUE)
        W_Cluster_Door_Open(_DOOR_REAR_LEFT);
    if(gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.rear_right == _G_TRUE)
        W_Cluster_Door_Open(_DOOR_REAR_RIGHT);
    if(gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.rear_Tailgate == _G_TRUE)
        W_Cluster_Door_Open(_DOOR_REAR_TAIL);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Shifter(void)
{
    if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_P)
        W_Draw_Image_By_ID(PIC_ID_C_GEAR_P, 573, 392);
    else if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_R)
        W_Draw_Image_By_ID(PIC_ID_C_GEAR_R, 573, 392);
    else if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_N)
        W_Draw_Image_By_ID(PIC_ID_C_GEAR_N, 573, 392);
    else if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8Shifter_Status == _SHIFTER_D)
        W_Draw_Image_By_ID(PIC_ID_C_GEAR_D, 573, 392);
    else
        W_Draw_Image_By_ID(PIC_ID_C_GEAR_OFF, 573, 392);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Car_Mode(void)
{
    if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8driving_porfile  == _DRIVE_TURTLE)
        W_Draw_Image_By_ID(PIC_ID_C_MODE_TURTLE, 598, 158);
    else if(gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8driving_porfile == _DRIVE_SPORT)
        W_Draw_Image_By_ID(PIC_ID_C_MODE_SPORT, 598, 158);
    else
        W_Draw_Image_By_ID(PIC_ID_C_MODE_ECO, 598, 158);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Car_Ready(void)
{
    if(gtmars_info.tcar_info.tcan_id53e_info.tsys_consisterncy_check1.ready_to_drive_light_on == _G_TRUE)
        W_Draw_Image_By_ID(PIC_ID_C_READY_OFF, 596, 428);
    else
        W_Draw_Image_By_ID(PIC_ID_C_READY_ON, 596, 428);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Test(void)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = 34;
    tex_argb32.Buffer.Size.Width  = 34;
    tex_argb32.Buffer.Size.Height = 62;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)_PIC_B_0;

    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    xPos = R_DRW2D_2X(100);
    yPos = R_DRW2D_2X(100);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, xPos, yPos);

    /* Draw texture into rectangle */
    rect.Pos.X       = R_DRW2D_2X(100);
    rect.Pos.Y       = R_DRW2D_2X(100);
    rect.Size.Width  = R_DRW2D_2X(34);
    rect.Size.Height = R_DRW2D_2X(62);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_DrawGreenPedal(uint8_t u8line_temp,uint8_t u8line_dir)
{
    uint8_t u8disp_cnt;
    
    for(u8disp_cnt = 0; u8disp_cnt < u8line_temp; u8disp_cnt++)
        W_Draw_Image_By_ID(((u8line_dir == 0)? tgreenline_right_pic_addr[u8disp_cnt] : tgreenline_left_pic_addr[u8disp_cnt]), ((u8line_dir == 0)? u16rightline_parameter[u8disp_cnt][2] : u16leftline_parameter[u8disp_cnt][2]), ((u8line_dir == 0)? u16rightline_parameter[u8disp_cnt][3] : u16leftline_parameter[u8disp_cnt][3]));
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_DrawBluePedal(uint8_t u8line_temp,uint8_t u8line_dir)
{
    uint8_t u8disp_cnt;

    for(u8disp_cnt = 0; u8disp_cnt < u8line_temp; u8disp_cnt++)
        W_Draw_Image_By_ID(((u8line_dir == 0)? tblueline_right_pic_addr[u8disp_cnt] : tblueline_left_pic_addr[u8disp_cnt]), ((u8line_dir == 0)? u16rightline_parameter[u8disp_cnt][2] : u16leftline_parameter[u8disp_cnt][2]), ((u8line_dir == 0)? u16rightline_parameter[u8disp_cnt][3] : u16leftline_parameter[u8disp_cnt][3]));
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Padel(void)
{
    uint8_t u8line_cnt;

    if(gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map > 0)
    {
        u8line_cnt = gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map / 9;
        W_Cluster_DrawBluePedal(u8line_cnt,0);
        W_Cluster_DrawBluePedal(u8line_cnt,1);
    }
    else if(gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map > 0)
    {
        u8line_cnt = gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map / 9;
        W_Cluster_DrawGreenPedal(u8line_cnt,0);
        W_Cluster_DrawGreenPedal(u8line_cnt,1);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Chart_BG(void)
{
    W_Draw_Image_By_ID(PIC_ID_C_POWER_BG,869,134);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Chart_Convert(void)
{
    uint8_t u8temp;

    for(u8temp = 0; u8temp < 20; u8temp ++)
    {
        u16chart_draw_posy[u8temp] = 185 + (u16chart_value[u8temp] / 10) * 7;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Chart_Porcess(void)
{   
    uint8_t u8temp;
    W_Cluster_Chart_BG();
    W_Cluster_Chart_Convert();
    for(u8temp = 0; u8temp < 19; u8temp ++)
    {
         W_Draw_Line( (907 + u8temp * 13), u16chart_draw_posy[u8temp],
                    (907 + (u8temp + 1) * 13), u16chart_draw_posy[u8temp +1],
                    2,0xffff0000);

/*        W_Draw_Line(u16char_draw_posx[u8temp], u16chart_draw_posy[u8temp],
                    u16char_draw_posx[u8temp + 1], u16chart_draw_posy[u8temp +1],
                    2,0xffff0000);
*/    }
    
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Parameter_Init(void)
{
    tcluster_draw2d_fb.Flags  = (r_drw2d_FramebufferFlags_t)0;
    tcluster_draw2d_fb.Handle = 0; /* not used currently */
    tcluster_draw2d_fb.Buffer.Pitch       = _CLUSTER_STRIDE;
    tcluster_draw2d_fb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tcluster_draw2d_fb.Buffer.Size.Width  = _CLUSTER_WIDTH;
    tcluster_draw2d_fb.Buffer.Size.Height = _CLUSTER_Hight;
    tcluster_draw2d_fb.Buffer.Data        = 0;
    gtmars_info.tcluster_info.u8acc_on_check_over = _G_FAIL;
}

void W_MAG_Init(void) {
    W_CANIO_PICS_Init();
    W_Cluster_Parameter_Init();
    W_Cluster_WM_Init();
    W_Cluster_CreateWindows();
    W_Cluster_Draw2DInit();
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Close_Window(void)
{
    R_WM_WindowDisable(LOC_WM_UNIT, &tcluster_rle_win);
    R_WM_WindowDisable(LOC_WM_UNIT, &tcluster_draw2d_window);
    //R_WM_WindowDisable(LOC_WM_UNIT, &tcluster_sprite_window);
    W_Cluster_WM_End(LOC_WM_UNIT,0);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_CLuster_Logo(void)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;
    static uint16_t u16logo_reveal = 0;
    static void * Pic_Addr = (void *)_PIC_S_LOGO;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = 944; //944
    tex_argb32.Buffer.Size.Width  = 944; //944
    tex_argb32.Buffer.Size.Height = 144; //144
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    //tex_argb32.Buffer.Data = Copy_And_Get(&Pic_Addr, 520000); //_PIC_S_LOGO u8small_logo_ImgData
    tex_argb32.Buffer.Data = (void *)u8small_logo_ImgData; //TODO: move to ram, _PIC_S_LOGO u8small_logo_ImgData

    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    xPos = R_DRW2D_2X((1280-944)/2); 
    yPos = R_DRW2D_2X((480-144)/2);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, xPos, yPos);


    /* Draw texture into rectangle */
    rect.Pos.X       = R_DRW2D_2X((1280-944)/2);
    rect.Pos.Y       = R_DRW2D_2X((480-144)/2);
    rect.Size.Width  = R_DRW2D_2X(u16logo_reveal);
    rect.Size.Height = R_DRW2D_2X(144);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }

    Task_Done(gtqueue_manager.u8current_point);
    if(gtmars_info.tcar_info.tcar_power_light_info.tcar_power_info.acc_status == 1)
    {
        if(u16logo_reveal < 1000)
        {
            u16logo_reveal += 50;
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL5, _EVENT_FIRST);
        }
        else
            W_SYS_Reset();

    }
    else
    {
        if(u16logo_reveal < 1000)
        {
            u16logo_reveal += 50;
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL5, _EVENT_FIRST);
        }
        else
        {
            u16logo_reveal  = 0;
            W_Task_ChangeEvent(_TYPE_CLUSTER, _LEVEL4, _EVENT_LOOP);
            gtmars_info.tcluster_info.u16timer2 = _TIME_5000ms;
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_MAG_Logo_Disp(void)
{
    tcluster_draw2d_fb.Buffer.Data = R_WM_WindowNewDrawBufGet(LOC_WM_UNIT, &tcluster_draw2d_window);
    R_DRW2D_FramebufferSet(tclusterDrw2dDev, &tcluster_draw2d_fb);
    R_DRW2D_CtxImgQuality(tclusterDrw2dDev,R_DRW2D_IMGQUALITY_HIGH);

    W_Cluster_Draw2d_Clear(0xff000000); //solid black
    W_CLuster_Logo();
    //W_Cluster_Logo_Black();

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Trip_BG(void)
{
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_BG, 110, 134);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Trip_Timer(uint32_t u32_time_in_seconds)
{
    uint8_t u8num[2];
    uint32_t u32_hour,u32_min,u32_sec;

    u32_hour = u32_time_in_seconds / 3600;
    u32_min = (u32_time_in_seconds % 3600) / 60;
    u32_sec = (u32_time_in_seconds % 3600) % 60;
    //
    // PM
    //
    if(u32_hour >= 12)
    {
        u8num[0] = (uint8_t)((u32_hour - 12) / 10);
        u8num[1] = (uint8_t)((u32_hour - 12) % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_PM, 288, 159);
    }
    //
    // AM
    //
    else
    {
        u8num[0] = (uint8_t)(u32_hour / 10);
        u8num[1] = (uint8_t)(u32_hour % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_AM, 288, 159);
    }
    //
    // hour
    //
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_0 + u8num[0], 248, 159);
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_0 + u8num[1], 256, 159);
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_COLON, 264, 159);
    //
    // minute
    //
    if(u32_min >= 10)
    {
        u8num[0] = (uint8_t)(u32_min / 10);
        u8num[1] = (uint8_t)(u32_min % 10);
    }
    else
    {
        u8num[0] = 0;
        u8num[1] = (uint8_t)(u32_min % 10);
    }
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_0 + u8num[0], 272, 159);
    W_Draw_Image_By_ID(PIC_ID_C_TRIP_S_0 + u8num[1], 280, 159);

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Trip_Icon(uint8_t u8Icon_Num)
{
    if(u8Icon_Num == 0)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_ICON_A, 158, 182);
    }
    else
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_ICON_B, 158, 182);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripA_KM(uint16_t u16_distance)
{
    uint16_t u16_digi,u16_deci;
    uint8_t u8_num[4];
    u16_digi = (u16_distance / 100) % 1000;
    u16_deci = (u16_distance % 100);
    u8_num[0] = (uint8_t)(u16_digi / 100);
    u8_num[1] = (uint8_t)((u16_digi / 10) % 10);
    u8_num[2] = (uint8_t)(u16_digi % 10);
    u8_num[3] = (uint8_t)(u16_deci / 10);
    if(u16_digi >= 100)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],158,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],174,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],190,231);
    }
    else if (u16_digi < 10)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],158,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_POINT,        174,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],182,231);
    }
    else
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],164,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],180,231);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripA_MIN(uint8_t min)
{
    uint8_t u8_num[2];
    u8_num[0] = min / 10;
    u8_num[1] = min % 10;
    if(u8_num[0] == 0)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],253,231);
    }
    else
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],245,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],261,231);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripA_Whkm(uint16_t u16_capacity, uint16_t u16_distance)
{
    uint32_t u32_wh_d_km = 0;
    uint8_t u8_num[3];
    if(u16_distance == 0)
    {
        u32_wh_d_km = 0;
    }
    else
    {
        u32_wh_d_km = (uint32_t) (u16_capacity / u16_distance) * 1000;
    }
    if(u32_wh_d_km >= 1000)
    {
        u8_num[0] = 9;
        u8_num[1] = 9;
        u8_num[2] = 9;
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],321,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],337,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],353,231);
    }
    else if(u32_wh_d_km >= 100)
    {
        u8_num[0] = (uint8_t)(u32_wh_d_km / 100);
        u8_num[1] = (uint8_t)((u32_wh_d_km / 10) % 10);
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],321,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],337,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],353,231);
    }
    else if(u32_wh_d_km < 10)
    {
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],337,231);
    }
    else
    {
        u8_num[1] = (uint8_t)((u32_wh_d_km / 10) % 10);
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],329,231);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],345,231);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripB_KM(uint16_t u16_distance)
{
    uint16_t u16_digi,u16_deci;
    uint8_t u8_num[4];
    u16_digi = (u16_distance / 100) % 1000;
    u16_deci = (u16_distance % 100);
    u8_num[0] = (uint8_t)(u16_digi / 100);
    u8_num[1] = (uint8_t)((u16_digi / 10) % 10);
    u8_num[2] = (uint8_t)(u16_digi % 10);
    u8_num[3] = (uint8_t)(u16_deci / 10);
    if(u16_digi >= 100)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],158,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],174,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],190,312);
    }
    else if (u16_digi < 10)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],158,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_POINT,        174,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],182,312);
    }
    else
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],164,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],180,312);
    }

}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripB_kWh(uint16_t u16_capacity)
{
    uint16_t u16_digi,u16_deci;
    uint8_t u8_num[4];
    u16_digi = (u16_capacity / 100) % 1000;
    u16_deci = (u16_capacity % 100);
    u8_num[0] = (uint8_t)(u16_digi / 100);
    u8_num[1] = (uint8_t)((u16_digi / 10) % 10);
    u8_num[2] = (uint8_t)(u16_digi % 10);
    u8_num[3] = (uint8_t)(u16_deci / 10);
    if(u16_digi >= 100)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],237,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],253,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],269,312);
    }
    else if (u16_digi < 10)
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],237,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_POINT,        253,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],261,312);
    }
    else
    {
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],245,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],261,312);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_TripB_Whkm(uint16_t u16_capacity, uint16_t u16_distance)
{
    uint32_t u32_wh_d_km = 0;
    uint8_t u8_num[3];
    if(u16_distance == 0)
    {
        u32_wh_d_km = 0;
    }
    else
    {
        u32_wh_d_km = (uint32_t) (u16_capacity / u16_distance) * 1000;
    }
    if(u32_wh_d_km >= 1000)
    {
        u8_num[0] = 9;
        u8_num[1] = 9;
        u8_num[2] = 9;
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],321,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],337,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],353,312);
    }
    else if(u32_wh_d_km >= 100)
    {
        u8_num[0] = (uint8_t)(u32_wh_d_km / 100);
        u8_num[1] = (uint8_t)((u32_wh_d_km / 10) % 10);
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],321,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],337,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],353,312);
    }
    else if(u32_wh_d_km < 10)
    {
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],337,312);
    }
    else
    {
        u8_num[1] = (uint8_t)((u32_wh_d_km / 10) % 10);
        u8_num[2] = (uint8_t)(u32_wh_d_km % 10);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],329,312);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],345,312);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Trip_Total_Mileage(uint32_t u32_mileage)
{
    uint8_t u8_num[7];

    if(u32_mileage < 10) // 0 ~ 9
    {
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],253,395);
    }
    else if(u32_mileage < 100) // 10 ~ 99
    {
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],245,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],261,395);
    }
    else if(u32_mileage < 1000) // 100 ~ 999
    {
        u8_num[4] = (uint8_t)((u32_mileage /100) % 10);
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[4],237,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],253,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],269,395);
    }
    else if(u32_mileage < 10000) // 1,000 ~ 9,999
    {
        u8_num[3] = (uint8_t)((u32_mileage /1000) % 10);
        u8_num[4] = (uint8_t)((u32_mileage /100) % 10);
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],229,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_COMMA,        245,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[4],253,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],269,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],285,395);
    }
    else if(u32_mileage < 100000) // 10,000 ~ 99,999
    {
        u8_num[2] = (uint8_t)((u32_mileage /10000) % 10);
        u8_num[3] = (uint8_t)((u32_mileage /1000) % 10);
        u8_num[4] = (uint8_t)((u32_mileage /100) % 10);
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],221,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],237,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_COMMA,        253,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[4],261,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],277,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],293,395);
    }
    else if(u32_mileage < 1000000) // 100,000 ~ 999,999
    {
        u8_num[1] = (uint8_t)((u32_mileage /100000) % 10);
        u8_num[2] = (uint8_t)((u32_mileage /10000) % 10);
        u8_num[3] = (uint8_t)((u32_mileage /1000) % 10);
        u8_num[4] = (uint8_t)((u32_mileage /100) % 10);
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],213,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],229,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],245,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_COMMA,        261,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[4],269,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],285,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],301,395);
    }
    else // 1,000,000 ~
    {
        u8_num[0] = (uint8_t)((u32_mileage /1000000) % 10);
        u8_num[1] = (uint8_t)((u32_mileage /100000) % 10);
        u8_num[2] = (uint8_t)((u32_mileage /10000) % 10);
        u8_num[3] = (uint8_t)((u32_mileage /1000) % 10);
        u8_num[4] = (uint8_t)((u32_mileage /100) % 10);
        u8_num[5] = (uint8_t)((u32_mileage /10) % 10);
        u8_num[6] = (uint8_t)((u32_mileage % 10));
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[0],197,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_COMMA,        213,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[1],221,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[2],237,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[3],253,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_COMMA,        269,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[4],277,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[5],293,395);
        W_Draw_Image_By_ID(PIC_ID_C_TRIP_L_0 + u8_num[6],309,395);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Trip_Process(void)
{
    W_Cluster_Trip_BG();

//    W_Cluster_Trip_Timer(gtmars_info.tcar_info.tcan_id56b_info.u32time_in_seconds);

//    W_Cluster_Trip_Icon(0); //

//    W_Cluster_TripA_KM(gtmars_info.tcar_info.tcan_id53e_info.u16remained_distance);

//    W_Cluster_TripA_MIN(24); //

//    W_Cluster_TripA_Whkm(gtmars_info.tcar_info.tcan_id53f_info.u16battery_remain_capacity,gtmars_info.tcar_info.tcan_id53e_info.u16remained_distance);

//    W_Cluster_TripB_KM(gtmars_info.tcar_info.tcan_id53e_info.u16total_distance);

//    W_Cluster_TripB_kWh(gtmars_info.tcar_info.tcan_id53f_info.u16battery_design_capacity); 

//    W_Cluster_TripB_Whkm(gtmars_info.tcar_info.tcan_id53f_info.u16battery_design_capacity,gtmars_info.tcar_info.tcan_id53e_info.u16total_distance);

//    W_Cluster_Trip_Total_Mileage(gtmars_info.tcar_info.tcan_id53e_info.u32mileague);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Demo_Trip(void)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = 302;
    tex_argb32.Buffer.Size.Width  = 302;
    tex_argb32.Buffer.Size.Height = 302;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)_PIC_C_BG_TRIP;
    
    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    xPos = R_DRW2D_2X(110);
    yPos = R_DRW2D_2X(134);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, xPos, yPos);

    /* Draw texture into rectangle */
    rect.Pos.X       = xPos;
    rect.Pos.Y       = yPos;
    rect.Size.Width  = R_DRW2D_2X(302);
    rect.Size.Height = R_DRW2D_2X(302);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Demo_Chart(void)
{
    W_Draw_Image_By_ID(PIC_ID_C_BG_POWER, 869, 134);
/*    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = 302;
    tex_argb32.Buffer.Size.Width  = 302;
    tex_argb32.Buffer.Size.Height = 302;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)_PIC_C_BG_POWER;
    
  
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    xPos = R_DRW2D_2X(869);
    yPos = R_DRW2D_2X(134);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, xPos, yPos);

  
    rect.Pos.X       = xPos;
    rect.Pos.Y       = yPos;
    rect.Size.Width  = R_DRW2D_2X(302);
    rect.Size.Height = R_DRW2D_2X(302);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
*/
}
//----------------------------------------------------------------------------------------------------------------------------
void W_Cluster_Eric_Test(void)
{
    r_drw2d_Error_t ret = R_DRW2D_ERR_OK;
    r_drw2d_Rect_t  rect;
    r_drw2d_Texture_t tex_argb32;
    uint32_t xPos, yPos;

    tex_argb32.Handle             = R_NULL;
    tex_argb32.Flags              = R_DRW2D_TEX_VT | R_DRW2D_TEX_BILINEAR;
    tex_argb32.Buffer.Pitch       = 44;
    tex_argb32.Buffer.Size.Width  = 44;
    tex_argb32.Buffer.Size.Height = 44;
    tex_argb32.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex_argb32.Buffer.Data        = (void*)_PIC_ERIC_TEST;

    /* Set texture */
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxIdentity(tclusterDrw2dDev);
    ret |= R_DRW2D_CtxTextureSet(tclusterDrw2dDev, 0, &tex_argb32);
    ret |= R_DRW2D_CtxFillMode(tclusterDrw2dDev, R_DRW2D_FILLMODE_TEXTURE);
    ret |= R_DRW2D_CtxBlendMode(tclusterDrw2dDev, R_DRW2D_BLENDMODE_SRC_OVER);
    xPos = R_DRW2D_2X(100);
    yPos = R_DRW2D_2X(100);
    ret |= R_DRW2D_CtxTextureTranslate(tclusterDrw2dDev, xPos, yPos);

    /* Draw texture into rectangle */
    rect.Pos.X       = xPos;
    rect.Pos.Y       = yPos;
    rect.Size.Width  = R_DRW2D_2X(44);
    rect.Size.Height = R_DRW2D_2X(44);

    ret |= R_DRW2D_DrawRect(tclusterDrw2dDev, &rect);
    ret |= R_DRW2D_CtxTextureIdentity(tclusterDrw2dDev);
    if (R_DRW2D_ERR_OK != ret)
    {
       while(1);
    }
}

//----------------------------------------------------------------------------------------------------------------------------
void W_MAG_Draw2D_Process(void)
{
    tcluster_draw2d_fb.Buffer.Data = R_WM_WindowNewDrawBufGet(LOC_WM_UNIT, &tcluster_draw2d_window);
    R_DRW2D_FramebufferSet(tclusterDrw2dDev, &tcluster_draw2d_fb);
    R_DRW2D_CtxImgQuality(tclusterDrw2dDev,R_DRW2D_IMGQUALITY_HIGH);
    W_Cluster_Draw2d_Clear(0x00000000);

    W_Cluster_Padel();
    W_Cluster_Shifter();
    W_Cluster_Car_Ready();
    W_Cluster_Car_Mode();
    W_Cluster_Sport_Speed();
    W_Cluster_Disp_Turnlight();
    W_Cluster_Icon_Process();
    if((gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.u8door_close_status_byte && 0x1F) < 0x01)
    {
        W_Cluster_Trip_Process();
    }
    else
    {
        W_Cluster_Door_Process();
    }

    /* demo purpose TODO: use real menu keys */
    if((gtmars_info.tcar_info.tcan_id287_info.u8menu_key_status > 0)) 
    {
        W_Cluster_Menu_Button();
        W_Cluster_Menu_BG();
        W_Cluster_Battery_Process();
    }
//    W_Cluster_Demo_Chart();
    W_Cluster_Chart_Porcess();
    //W_Cluster_Eric_Test(); //for align test
    W_Cluster_Debug_OSD();
    u32_FPS_Count++;
}
