#ifndef CONTROLCAN_H
#define CONTROLCAN_H

#include <minwindef.h>

////�ļ��汾��v2.00 20150920
//#include <cvidef.h>	//ʹ��CVIƽ̨��������ʹ�ø���䡣

//�ӿڿ����Ͷ���

#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_USBCAN2A		4

#define VCI_USBCAN_E_U 		20
#define VCI_USBCAN_2E_U 	21



//�������÷���״ֵ̬
#define	STATUS_OK					1
#define STATUS_ERR					0
	
/*------------------------------------------------����ZLG�ĺ�������������------------------------------------------------*/
//1.ZLGCANϵ�нӿڿ���Ϣ���������͡�
typedef  struct  _VCI_BOARD_INFO{
		USHORT	hw_Version;               // Ӳ���汾�ţ���16���Ʊ�ʾ������0x0100��ʾV1.00��
		USHORT	fw_Version;               // �̼��汾�ţ� ��16���Ʊ�ʾ������0x0100��ʾV1.00
		USHORT	dr_Version;               // ��������汾�ţ� ��16���Ʊ�ʾ������0x0100��ʾV1.00��
		USHORT	in_Version;               // �ӿڿ�汾�ţ� ��16���Ʊ�ʾ������0x0100��ʾV1.00
		USHORT	irq_Num;                  // ��������
		BYTE	can_Num;                  // ��ʾ�м�·CANͨ��
		CHAR	str_Serial_Num[20];       // �˰忨�����к�
		CHAR	str_hw_Type[40];          // Ӳ�����ͣ����硰USBCAN V1.00����ע�⣺�����ַ�����������\0����
		USHORT	Reserved[4];              // ϵͳ����
} VCI_BOARD_INFO,*PVCI_BOARD_INFO; 

//2.����CAN��Ϣ֡���������͡�
typedef  struct  _VCI_CAN_OBJ{
	UINT	ID;                          // ֡ID�� 32λ���������ݸ�ʽΪ���Ҷ��롣 
	UINT	TimeStamp;                   // �豸���յ�ĳһ֡��ʱ���ʶ�� ʱ���ʾ��CAN���ϵ翪ʼ��ʱ����ʱ��λΪ0.1ms��
	BYTE	TimeFlag;                    // �Ƿ�ʹ��ʱ���ʶ��Ϊ1ʱTimeStamp��Ч�� TimeFlag��TimeStampֻ�ڴ�֡Ϊ����֡ʱ�����塣
	BYTE	SendType;                    // ����֡���͡�
									    	     // = 0ʱΪ�������ͣ�����ʧ�ܻ��Զ��ط����ط��ʱ��Ϊ1.5 - 3�룩��
									    	     // = 1ʱΪ���η��ͣ�ֻ����һ�Σ����Զ��ط�����
										         // ����ֵ��Ч��
										         // �����ο���������SendType = 1����߷��͵���Ӧ�ٶȣ�
	BYTE	RemoteFlag;                  // �Ƿ���Զ��֡�� =0ʱΪΪ����֡�� =1ʱΪԶ��֡�����ݶοգ�
	BYTE	ExternFlag;                  // �Ƿ�����չ֡�� =0ʱΪ��׼֡��11λID���� =1ʱΪ��չ֡��29λID��
	BYTE	DataLen;                     // ���ݳ��� DLC (<=8)����CAN֡Data�м����ֽڡ�Լ���˺���Data[8]�е���Ч�ֽڡ�
	BYTE	Data[8];                     // CAN֡�����ݡ�����CAN�涨�������8���ֽڣ���������Ԥ����8���ֽڵĿռ䣬�� DataLenԼ������DataLen����Ϊ3����Data[0]�� Data[1]�� Data[2]����Ч�ġ�
	BYTE	Reserved[3];                 // ϵͳ����
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.�����ʼ��CAN����������VCI_INIT_CONFIG���ṹ�嶨���˳�ʼ��CAN�����á��ṹ�彫��VCI_InitCan�����б���䣬 ����ʼ��֮ǰ��Ҫ���������ṹ�������
typedef struct _VCI_INIT_CONFIG{
	DWORD	AccCode;                     // �����롣 SJA1000��֡���������롣�Ծ������������Ϊ���й�λ������ƥ�䣬ȫ��ƥ��ɹ��󣬴�֡���Ա����ա����򲻽��ա�
	DWORD	AccMask;                     // �����롣 SJA1000��֡���������롣�Խ��յ�CAN֡ID���й��ˣ���ӦλΪ0���ǡ��й�λ������ӦλΪ1���ǡ��޹�λ�����������Ƽ�����Ϊ0xFFFFFFFF����ȫ������
	DWORD	Reserved;                    // ����
	UCHAR	Filter;                      // �˲���ʽ����������Ϊ0-3
	UCHAR	Timing0;                     // �����ʶ�ʱ�� 0��BTR0��	
	UCHAR	Timing1;                     // �����ʶ�ʱ�� 1��BTR1��	
	UCHAR	Mode;                        // ģʽ�� =0��ʾ����ģʽ���൱�������ڵ㣩�� =1��ʾֻ��ģʽ��ֻ���գ���Ӱ�����ߣ��� = 2��ʾ�Է�����ģʽ������ģʽ��
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

///////// new add struct for filter /////////
typedef struct _VCI_FILTER_RECORD{
	DWORD ExtFrame;	//�Ƿ�Ϊ��չ֡
	DWORD Start;
	DWORD End;
}VCI_FILTER_RECORD,*PVCI_FILTER_RECORD;
 
#define EXTERNC		extern "C"


EXTERNC DWORD __stdcall VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);                             // ���豸
EXTERNC DWORD __stdcall VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd);                                           // �ر��豸
EXTERNC DWORD __stdcall VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);  // ��ʼ��CANͨ��

EXTERNC DWORD __stdcall VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);    // ��ȡ�豸��Ϣ

EXTERNC DWORD __stdcall VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);

EXTERNC ULONG __stdcall VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC DWORD __stdcall VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
EXTERNC DWORD __stdcall VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

EXTERNC ULONG __stdcall VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,ULONG Len);                    // ���ͺ���
EXTERNC ULONG __stdcall VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,ULONG Len,INT WaitTime);     // ���պ���


/*------------------------------------------------�������亯�������ݽṹ����------------------------------------------------*/

//USB-CAN�����������忨��Ϣ����������1��������ΪVCI_FindUsbDevice�����ķ��ز�����
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

//USB-CAN�����������忨��Ϣ����������2��������ΪVCI_FindUsbDevice�����ķ��ز�����Ϊ��չ������豸
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
