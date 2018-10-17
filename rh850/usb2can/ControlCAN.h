#ifndef CONTROLCAN_H
#define CONTROLCAN_H

#include <minwindef.h>

////文件版本：v2.00 20150920
//#include <cvidef.h>	//使用CVI平台开发，请使用该语句。

//接口卡类型定义

#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_USBCAN2A		4

#define VCI_USBCAN_E_U 		20
#define VCI_USBCAN_2E_U 	21



//函数调用返回状态值
#define	STATUS_OK					1
#define STATUS_ERR					0
	
/*------------------------------------------------兼容ZLG的函数及数据类型------------------------------------------------*/
//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO{
		USHORT	hw_Version;               // 硬件版本号，用16进制表示。比如0x0100表示V1.00。
		USHORT	fw_Version;               // 固件版本号， 用16进制表示。比如0x0100表示V1.00
		USHORT	dr_Version;               // 驱动程序版本号， 用16进制表示。比如0x0100表示V1.00。
		USHORT	in_Version;               // 接口库版本号， 用16进制表示。比如0x0100表示V1.00
		USHORT	irq_Num;                  // 保留参数
		BYTE	can_Num;                  // 表示有几路CAN通道
		CHAR	str_Serial_Num[20];       // 此板卡的序列号
		CHAR	str_hw_Type[40];          // 硬件类型，比如“USBCAN V1.00”（注意：包括字符串结束符’\0’）
		USHORT	Reserved[4];              // 系统保留
} VCI_BOARD_INFO,*PVCI_BOARD_INFO; 

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ{
	UINT	ID;                          // 帧ID。 32位变量，数据格式为靠右对齐。 
	UINT	TimeStamp;                   // 设备接收到某一帧的时间标识。 时间标示从CAN卡上电开始计时，计时单位为0.1ms。
	BYTE	TimeFlag;                    // 是否使用时间标识，为1时TimeStamp有效， TimeFlag和TimeStamp只在此帧为接收帧时有意义。
	BYTE	SendType;                    // 发送帧类型。
									    	     // = 0时为正常发送（发送失败会自动重发，重发最长时间为1.5 - 3秒）；
									    	     // = 1时为单次发送（只发送一次，不自动重发）；
										         // 其它值无效。
										         // （二次开发，建议SendType = 1，提高发送的响应速度）
	BYTE	RemoteFlag;                  // 是否是远程帧。 =0时为为数据帧， =1时为远程帧（数据段空）
	BYTE	ExternFlag;                  // 是否是扩展帧。 =0时为标准帧（11位ID）， =1时为扩展帧（29位ID）
	BYTE	DataLen;                     // 数据长度 DLC (<=8)，即CAN帧Data有几个字节。约束了后面Data[8]中的有效字节。
	BYTE	Data[8];                     // CAN帧的数据。由于CAN规定了最大是8个字节，所以这里预留了8个字节的空间，受 DataLen约束。如DataLen定义为3，即Data[0]、 Data[1]、 Data[2]是有效的。
	BYTE	Reserved[3];                 // 系统保留
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.定义初始化CAN的数据类型VCI_INIT_CONFIG，结构体定义了初始化CAN的配置。结构体将在VCI_InitCan函数中被填充， 即初始化之前，要先填好这个结构体变量。
typedef struct _VCI_INIT_CONFIG{
	DWORD	AccCode;                     // 验收码。 SJA1000的帧过滤验收码。对经过屏蔽码过滤为“有关位”进行匹配，全部匹配成功后，此帧可以被接收。否则不接收。
	DWORD	AccMask;                     // 屏蔽码。 SJA1000的帧过滤屏蔽码。对接收的CAN帧ID进行过滤，对应位为0的是“有关位”，对应位为1的是“无关位”。屏蔽码推荐设置为0xFFFFFFFF，即全部接收
	DWORD	Reserved;                    // 保留
	UCHAR	Filter;                      // 滤波方式，允许设置为0-3
	UCHAR	Timing0;                     // 波特率定时器 0（BTR0）	
	UCHAR	Timing1;                     // 波特率定时器 1（BTR1）	
	UCHAR	Mode;                        // 模式。 =0表示正常模式（相当于正常节点）， =1表示只听模式（只接收，不影响总线）， = 2表示自发自收模式（环回模式）
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

///////// new add struct for filter /////////
typedef struct _VCI_FILTER_RECORD{
	DWORD ExtFrame;	//是否为扩展帧
	DWORD Start;
	DWORD End;
}VCI_FILTER_RECORD,*PVCI_FILTER_RECORD;
 
#define EXTERNC		extern "C"


EXTERNC DWORD __stdcall VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);                             // 打开设备
EXTERNC DWORD __stdcall VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd);                                           // 关闭设备
EXTERNC DWORD __stdcall VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);  // 初始化CAN通道

EXTERNC DWORD __stdcall VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);    // 获取设备信息

EXTERNC DWORD __stdcall VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);

EXTERNC ULONG __stdcall VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC DWORD __stdcall VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC ULONG __stdcall VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,ULONG Len);                    // 发送函数
EXTERNC ULONG __stdcall VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,ULONG Len,INT WaitTime);     // 接收函数


/*------------------------------------------------其他补充函数及数据结构描述------------------------------------------------*/

//USB-CAN总线适配器板卡信息的数据类型1，该类型为VCI_FindUsbDevice函数的返回参数。
typedef  struct  _VCI_BOARD_INFO1{
	USHORT	hw_Version;
	USHORT	fw_Version;
	USHORT	dr_Version;
	USHORT	in_Version;
	USHORT	irq_Num;
	BYTE	can_Num;
	BYTE	Reserved;
	CHAR	str_Serial_Num[8];
	CHAR	str_hw_Type[16];
	CHAR	str_Usb_Serial[4][4];
} VCI_BOARD_INFO1,*PVCI_BOARD_INFO1;

//USB-CAN总线适配器板卡信息的数据类型2，该类型为VCI_FindUsbDevice函数的返回参数。为扩展更多的设备
typedef  struct  _VCI_BOARD_INFO2{
	USHORT	hw_Version;
	USHORT	fw_Version;
	USHORT	dr_Version;
	USHORT	in_Version;
	USHORT	irq_Num;
	BYTE	can_Num;
	BYTE	Reserved;
	CHAR	str_Serial_Num[8];
	CHAR	str_hw_Type[16];
	CHAR	str_Usb_Serial[10][4];
} VCI_BOARD_INFO2,*PVCI_BOARD_INFO2;


#define EXTERNC		extern "C"

EXTERNC DWORD __stdcall VCI_GetReference2(DWORD DevType,DWORD DevIndex,DWORD CANIndex,DWORD Reserved,BYTE *pData);
EXTERNC DWORD __stdcall VCI_SetReference2(DWORD DevType,DWORD DevIndex,DWORD CANIndex,DWORD RefType,BYTE *pData);


EXTERNC DWORD __stdcall VCI_ConnectDevice(DWORD DevType,DWORD DevIndex);
EXTERNC DWORD __stdcall VCI_UsbDeviceReset(DWORD DevType,DWORD DevIndex,DWORD Reserved);
EXTERNC DWORD __stdcall VCI_FindUsbDevice(PVCI_BOARD_INFO1 pInfo);
EXTERNC DWORD __stdcall VCI_FindUsbDevice2(PVCI_BOARD_INFO2 pInfo);



#endif
