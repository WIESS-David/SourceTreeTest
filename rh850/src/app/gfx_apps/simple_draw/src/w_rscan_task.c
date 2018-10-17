
/*============================================================================
 Includes
============================================================================*/
#include "r_dev_api.h"
#include "w_rscan_task.h"
#include "w_sys_pwm.h"
#include "r_bsp_stdio_api.h"
#include "w_system.h"
#include "obd2_pids.h"
#include "nios.h"

extern mars_info_def gtmars_info;
extern queue_task_def gtqueue_manager;
extern task_recorder_def gttask_recorder[_TASK_NUM];
 
//============================================================================================================
// Table defines
//============================================================================================================
void (*const RSCAN_State_Machine[_MAX_RSCAN_ST_NO])(void) = 
{W_RSCAN_Task_Init , W_RSCAN_Timimg , W_RSCAN_Error}; 
/*============================================================================
 Variables
============================================================================*/
unsigned long ul_interrupt_counter;     /* interrupt occurences counter */

/* Create Can_FrameType for send and receive data */
Can_FrameType tcan_cluster =
{
    //CiTBpA
    0x429,
    0,
    0,
    0,

    //CiTBpB
    0x0000,
    0x000,
    0x8,

    {
        0x00,                            //DB0
        0x00,                            //DB1
        0x00,                            //DB2
        0x00,                            //DB3
        //CiTBpD
        0x00,                            //DB4
        0x00,                            //DB5
        0x00,                            //DB6
        0x00                             //DB7
    }
};

/* Create Can_FrameType for send and receive data */
/*
Can_FrameType tcan_trans_rpm =
{
    //CiTBpA
    0x7DF,
    0,
    0,
    0,

    //CiTBpB
    0x0000,
    0x000,
    0x8,

    {
        0x02,                            //DB0
        0x01,                            //DB1
        0x0C,                            //DB2
        0x00,                            //DB3
        //CiTBpD
        0x00,                            //DB4
        0x00,                            //DB5
        0x00,                            //DB6
        0x00                             //DB7
    }
};
*/
/* Create Can_FrameType for send and receive data */
/*
Can_FrameType tcan_trans_tmp =
{
    //CiTBpA
    0x7DF,
    0,
    0,
    0,

    //CiTBpB
    0x0000,
    0x000,
    0x8,

    {
        0x02,                            //DB0
        0x01,                            //DB1
        0x05,                            //DB2
        0x00,                            //DB3
        //CiTBpD
        0x00,                            //DB4
        0x00,                            //DB5
        0x00,                            //DB6
        0x00                             //DB7
    }
};
*/

Can_FrameType tcan_Reci_buffer;

void W_RSCAN_Short_Delay(uint16_t u16delay)
{
    uint16_t u16counter1 = 0;
    uint16_t u16counter2 = 0;
    for(u16counter1 = 0; u16counter1 < u16delay; u16counter1++)
        for(u16counter2 = 0; u16counter2 < 10000; u16counter2++);
}

void W_RSCAN_ChangeState(uint8_t u8canstate)
{    
    if(u8canstate ==_STATE_RSCAN_TIMING)
    {
//        W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_TIMING, RSCAN_State_Machine[_STATE_RSCAN_TIMING]);
        W_Task_ChangeEvent(_TYPE_RSCAN, _LEVEL4, _EVENT_LOOP);
    }
    else
        W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_ERROR, RSCAN_State_Machine[_STATE_RSCAN_ERROR]);
    Task_Done(gtqueue_manager.u8current_point);    
}

void W_RSCAN_Buffer_Init(void)
{
    uint8_t u8i;
    for(u8i = 0;u8i < _FILTER_SIZE; u8i++)
    {
    }
}

void W_RSCAN_Clear_MailBox(const uint8_t qID) {
    /* don't wait */
    CLR_BIT(RSCAN0RMND0, qID);	//To clear a flag to 0, the program must write 0 to the flag               
}

/* rx mailbox 0 */
void W_RSCAN_ID287(const uint8_t qID)
{
    uint16_t ts = RSCAN0GTSCL;
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);
        pFrame = (Can_FrameType*) &(RSCAN0RMID0);
        R_BSP_STDIO_Printf("287: id:%d ts:%d %d %d %d %d %d %d %d %d \n",  pFrame->ID,  pFrame->TS,  pFrame->DB[0],  \
            pFrame->DB[1],  pFrame->DB[2],  pFrame->DB[3],  pFrame->DB[4],  pFrame->DB[5],  pFrame->DB[6],  pFrame->DB[7]);

        if(pFrame->DB[0] != 87) return; /* MAGIC BYTE */

        switch(pFrame->DB[1])
        {
        /* TODO: switch mode, profiling */
            case 11:
                //R_BSP_STDIO_Printf("W_RSCAN_Data_Process count: %d\n", data_process_count);
                break;
            case 12:
                R_BSP_STDIO_Printf("ts: %d\n", ts);
                break;
        }
        switch(pFrame->DB[6])
        {
            default:
                gtmars_info.tcar_info.tcan_id287_info.u8menu_key_status = pFrame->DB[6];
        }
        switch(pFrame->DB[7])
        {
            case 1:
                //W_BUZZER_SetTone(BUZZER_TONE1);
                break;
            case 0:
                //W_BUZZER_SetTone(BUZZER_TONE2);
                break;
        }
    }
}

/* rx mailbox 1 */
void W_RSCAN_ID133(const uint8_t qID)
{
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);
        pFrame = (Can_FrameType*) &(RSCAN0RMID1);
        gtmars_info.tcar_info.tcan_id133_info.u8vehicle_speed = pFrame->DB[0];
        gtmars_info.tcar_info.tcan_id133_info.u8cruise_speed = pFrame->DB[1];
        gtmars_info.tcar_info.tcan_id133_info.u8accel_Pedal_map = pFrame->DB[2];
        gtmars_info.tcar_info.tcan_id133_info.u8regeneration_map = pFrame->DB[3];
        gtmars_info.tcar_info.tcan_id133_info.u16power_usage_power = pFrame->DB[4] << 8 | pFrame->DB[5];
        gtmars_info.tcar_info.tcan_id133_info.tfault_code1_info.u8fault_code1_byte = pFrame->DB[6];
        gtmars_info.tcar_info.tcan_id133_info.u8main_checksum1 = pFrame->DB[7];
    }
}

/* rx mailbox 2 */
void W_RSCAN_ID134(const uint8_t qID)
{
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);
        pFrame = (Can_FrameType*) &(RSCAN0RMID2);
        gtmars_info.tcar_info.tcan_id134_info.tvehicle_info.u8vehicle_info_byte = pFrame->DB[0]; 
        gtmars_info.tcar_info.tcan_id134_info.tshifter_and_mode_info.u8shifter_mode_byte = pFrame->DB[1];
        gtmars_info.tcar_info.tcan_id134_info.tpowertrain_status_info.u8powertrain_status_byte = pFrame->DB[2];
        gtmars_info.tcar_info.tcan_id134_info.u8coolant_temp_1 = pFrame->DB[3];
        gtmars_info.tcar_info.tcan_id134_info.u8coolant_temp_2 = pFrame->DB[4];
        gtmars_info.tcar_info.tcan_id134_info.u8coolant_temp_3 = pFrame->DB[5];
        gtmars_info.tcar_info.tcan_id134_info.tfault_code2_info.u8fault_code2_byte = pFrame->DB[6];
        gtmars_info.tcar_info.tcan_id134_info.u8main_checksum2 = pFrame->DB[7];
    }
}

/* rx mailbox 3 */
void W_RSCAN_ID53E(const uint8_t qID)
{
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);

        pFrame = (Can_FrameType*) &(RSCAN0RMID3);
        gtmars_info.tcar_info.tcan_id53e_info.u16remained_distance = pFrame->DB[0] << 8 | pFrame->DB[1]; 
        gtmars_info.tcar_info.tcan_id53e_info.u16total_distance = pFrame->DB[2] << 8 | pFrame->DB[3]; 
        gtmars_info.tcar_info.tcan_id53e_info.u32mileague = pFrame->DB[4] << 16 | pFrame->DB[5] << 8 | pFrame->DB[6];
        gtmars_info.tcar_info.tcan_id53e_info.tsys_consisterncy_check1.u8sys_consistency_check1_byte= pFrame->DB[7];
    }
}

/* rx mailbox 4 */
void W_RSCAN_ID53F(const uint8_t qID)
{
    Can_FrameType* pBase;
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);
        pBase = (Can_FrameType*) &(RSCAN0RMID4);
        gtmars_info.tcar_info.tcan_id53f_info.u16battery_remain_capacity = pFrame->DB[0] << 8 | pFrame->DB[1]; 
        gtmars_info.tcar_info.tcan_id53f_info.u16battery_design_capacity = pFrame->DB[2] << 8 | pFrame->DB[3]; 
        gtmars_info.tcar_info.tcan_id53f_info.u8hv_battery_soc = pFrame->DB[4];
        gtmars_info.tcar_info.tcan_id53f_info.u8hv_battery_soh = pFrame->DB[5];
        gtmars_info.tcar_info.tcan_id53f_info.tsys_consistency_check3_info.u8sys_consistency_check3_byte = pFrame->DB[7];
    }
}

/* rx mailbox 5 */
void W_RSCAN_ID434(const uint8_t qID)
{
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);

        pFrame = (Can_FrameType*) &(RSCAN0RMID5); 
        gtmars_info.tcar_info.tcan_id434_info.tdoor_status_info.u8door_close_status_byte = pFrame->DB[0];
        gtmars_info.tcar_info.tcan_id434_info.tlamp_status_info.u8lamp_status_byte = pFrame->DB[2];
        gtmars_info.tcar_info.tcan_id434_info.tcabin_status_info.u8cabin_status_byte = pFrame->DB[3];
        gtmars_info.tcar_info.tcan_id434_info.twiper_status_info.u8wiper_status_byte = pFrame->DB[4];
        gtmars_info.tcar_info.tcan_id434_info.tsecurity_status_info.u8security_status_byte = pFrame->DB[5];
        gtmars_info.tcar_info.tcan_id434_info.toperation1_mode_flag_info.u8operation1_mode_flag_byte = pFrame->DB[7];

    }
}

/* rx mailbox 6 */
void W_RSCAN_ID340(const uint8_t qID)
{
    Can_FrameType* pFrame;

   if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
         W_RSCAN_Clear_MailBox(qID);
        pFrame = (Can_FrameType*) &(RSCAN0RMID6);
        gtmars_info.tcar_info.tcan_id340_info.thvac_status_info.u8havc_status_byte = pFrame->DB[0];
        gtmars_info.tcar_info.tcan_id340_info.u8temp_inside = pFrame->DB[1];
        gtmars_info.tcar_info.tcan_id340_info.u8temp_outside = pFrame->DB[2];
        gtmars_info.tcar_info.tcan_id340_info.toperation2_mode_flag_info.u8operation2_mode_flag_byte = pFrame->DB[7];
    }
}

/* rx mailbox 7 */
void W_RSCAN_ID56B(const uint8_t qID)
{   
    Can_FrameType* pFrame;

    if(GET_BIT(RSCAN0RMND0, qID) == CAN_SET) {
        W_RSCAN_Clear_MailBox(qID);
        pFrame = (Can_FrameType*) &(RSCAN0RMID7);
        gtmars_info.tcar_info.tcan_id56b_info.u32time_in_seconds = pFrame->DB[0] << 16| pFrame->DB[1] << 8 | pFrame->DB[2];
        gtmars_info.tcar_info.tcan_id56b_info.u8date = pFrame->DB[3];
        gtmars_info.tcar_info.tcan_id56b_info.u8Month = pFrame->DB[4];
        gtmars_info.tcar_info.tcan_id56b_info.u16Year = pFrame->DB[5] << 8 | pFrame->DB[6];
        gtmars_info.tcar_info.tcan_id56b_info.topertion3_mode_flag_info.u8opertion3_mode_flag_byte = pFrame->DB[7];
    }
}


//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_Data_Process(void)
{
    uint8_t queueID = 0;

    W_RSCAN_ID287(queueID++); /* wiess control message */
    W_RSCAN_ID133(queueID++);
    W_RSCAN_ID134(queueID++);
    W_RSCAN_ID53E(queueID++);
    W_RSCAN_ID53F(queueID++);
    W_RSCAN_ID434(queueID++);
    W_RSCAN_ID340(queueID++);
    W_RSCAN_ID56B(queueID++);
}

void W_RSCAN_Read_FIFO(void)
{
    uint8_t ret;
	Can_FrameType* pRxBuffer;
	
	// check receive FIFO Buffer Empty Status Flag
	if ((RSCAN0.RFSTS0.UINT16[L] & 0x0001U) == 0x0001U)		// RFEMP
	{
		ret = CAN_RTN_BUFFER_EMPTY;
	}
	else
	{
		// Read out message from Rx FIFO buffer 0
		pRxBuffer = (Can_FrameType*) &(RSCAN0.RFID0.UINT32);
		//*CAN_RX_Frame = pRxBuffer[0];
		
		// clear interrupt flag RFIF
		RSCAN0.RFSTS0.UINT16[L] &= 0xF7U;
		
		// read pointer moves to the next unread message in the receive FIFO buffer.
		RSCAN0.RFPCTR0.UINT8[LL] = 0xFFU;
		
		ret = CAN_SET;
	}
	
	//return ret;
}

//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_Task_Init(void)
{
    gtmars_info.tcar_info.u16timer1 = _TIME_DISABLE;
    gtmars_info.tcar_info.u16timer2 = _TIME_DISABLE;
//    gtmars_info.tcar_info.u16error_counter = 0;

    W_RSCAN_Buffer_Init();
    
    if(W_RSCAN_Init(_RSCAN_RETRY_COUNTER) != CAN_RTN_OK)
        W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_ERROR, RSCAN_State_Machine[_STATE_RSCAN_ERROR]);
    else
        W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_TIMING, RSCAN_State_Machine[_STATE_RSCAN_TIMING]);
    
    Task_Done(gtqueue_manager.u8current_point);
}

void W_RSCAN_Send_Data(void)
{
    uint16_t u16retry_temp;

    tcan_cluster.ID = 0x429;
    tcan_cluster.DLC = 8;
    tcan_cluster.DB[0] = gtmars_info.tcar_info.tcar_power_light_info.tcar_light_info.u8car_light_byte;
    //TODO tcan_cluster.DB[1] tcan_cluster.DB[2] average power
    //TODO tcan_cluster.DB[3] tcan_cluster.DB[4] average power
    //TODO tcan_cluster.DB[5] tcan_cluster.DB[6] average power
    tcan_cluster.DB[7] = 0x80; //life flag

    u16retry_temp = _RSCAN_RETRY_COUNTER;
    while( W_RSCAN_SendTxBuffer(0, &tcan_cluster, u16retry_temp) != CAN_RTN_OK)
    {
        u16retry_temp--;
        if(u16retry_temp == 0)
        {
            //gtmars_info.tcar_info.u16timer1 = _TIME_100ms;                   
            //Task_Done(gtqueue_manager.u8current_point);
            return;
        }
    }
}

void W_RSCAN_Timimg(void)
{
    uint16_t u16retry_temp;
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;
    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tcar_info.u16timer1 = _TIME_150ms;
            break;
        case _EVENT_DATA:
            break;
        case _EVENT_TIMER1:
            /*
            u16retry_temp = _RSCAN_RETRY_COUNTER;
            while( W_RSCAN_ReadRxBuffer(&tcan_Reci_buffer) != CAN_RTN_OK)
            {
                u16retry_temp--;
                if(u16retry_temp == 0)
                {
                    //W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_ERROR, RSCAN_State_Machine[_STATE_RSCAN_ERROR]); 
                    gtmars_info.tcar_info.u16timer1 = _TIME_100ms;                   
                    Task_Done(gtqueue_manager.u8current_point);
                    return;
                }
            }
            */
            //W_RSCAN_Read_FIFO();
            //W_RSCAN_Data_Process();
            W_RSCAN_Send_Data();
            gtmars_info.tcar_info.u16timer1 = _TIME_500ms;
            break;
        case _EVENT_TIMER2:
            break;
        case _EVENT_TIMER3:
            break;
        default:
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_Error(void)
{
    uint8_t u8event = gtqueue_manager.ttask[gtqueue_manager.u8current_point].u8event;

    switch(u8event)
    {
        case _EVENT_FIRST:
            gtmars_info.tcar_info.u16timer1 = _TIME_5000ms;
            break;
        case _EVENT_TIMER1:
            R_BSP_STDIO_Printf("W_RSCAN_Error goes to  _STATE_RSCAN_INIT\n");
            W_Task_ChangeState(_TYPE_RSCAN, _LEVEL5, _STATE_RSCAN_INIT, RSCAN_State_Machine[_STATE_RSCAN_INIT]);
            break;
    }
    Task_Done(gtqueue_manager.u8current_point);
}
// Timer
//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_Timer1(void)
{
    if(gtmars_info.tcar_info.u16timer1 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tcar_info.u16timer1 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_RSCAN, _LEVEL3, _EVENT_TIMER1);
        gtmars_info.tcar_info.u16timer1--;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_Timer2(void)
{
    if(gtmars_info.tcar_info.u16timer2 == _TIME_DISABLE)
        return;
    else
    {
        if(gtmars_info.tcar_info.u16timer2 == _TIME_10ms)
            W_Task_ChangeEvent(_TYPE_RSCAN, _LEVEL3, _EVENT_TIMER2);
        gtmars_info.tcar_info.u16timer2--;

    }
}

//----------------------------------------------------------------------------------------------------------------------------
void W_RSCAN_AllTimer(void)
{
    W_RSCAN_Timer1();
    W_RSCAN_Timer2();
}
//----------------------------------------------------------------------------------------------------------------------------

