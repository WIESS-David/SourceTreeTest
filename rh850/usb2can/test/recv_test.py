from ctypes import *
import sys
import time
 
VCI_USBCAN2A = 4
VCI_USBCAN_2E_U = 21
DEVICE_ID = VCI_USBCAN_2E_U
STATUS_OK = 1
class VCI_INIT_CONFIG(Structure):  
    _fields_ = [("AccCode", c_ulong),
                ("AccMask", c_ulong),
                ("Reserved", c_ulong),
                ("Filter", c_ubyte),
                ("Timing0", c_ubyte),
                ("Timing1", c_ubyte),
                ("Mode", c_ubyte)
                ]  
        
class VCI_CAN_OBJ(Structure):  
    _fields_ = [("ID", c_uint),
                ("TimeStamp", c_uint),
                ("TimeFlag", c_ubyte),
                ("SendType", c_ubyte),
                ("RemoteFlag", c_ubyte),
                ("ExternFlag", c_ubyte),
                ("DataLen", c_ubyte),
                ("Data", c_ubyte*8),
                ("Reserved", c_ubyte*3)
                ] 
 
CanDLLName = 'ControlCAN.dll' #DLL是32位的，必须使用32位的PYTHON
canDLL = windll.LoadLibrary(CanDLLName)
print(CanDLLName)
 
ret = canDLL.VCI_OpenDevice(DEVICE_ID, 0, 0)
print(ret)
if ret != STATUS_OK:
    print('VCI_OpenDevice ERROR\r\n')


baudrate  = c_int32(0x060007)
ret = canDLL.VCI_SetReference(DEVICE_ID, 0, 0, 0, byref(baudrate))
if ret != STATUS_OK:
    print('VCI_SetReference ERROR\r\n')
    
#初始0通道
vci_initconfig = VCI_INIT_CONFIG(0x80000008, 0xFFFFFFFF, 0,
                                 2, 0x00, 0x1C, 0)
ret = canDLL.VCI_InitCAN(DEVICE_ID, 0, 0, byref(vci_initconfig))
if ret != STATUS_OK:
    print('VCI_InitCAN ERROR\r\n')
 
ret = canDLL.VCI_StartCAN(DEVICE_ID, 0, 0)
if ret != STATUS_OK:
    print('VCI_StartCAN ERROR\r\n')
 
#初始1通道 send
#ret = canDLL.VCI_InitCAN(DEVICE_ID, 0, 1, byref(vci_initconfig))
#if ret != STATUS_OK:
#    print('VCI_InitCAN 1 ERROR\r\n')
 
#ret = canDLL.VCI_StartCAN(DEVICE_ID, 0, 1)
#if ret != STATUS_OK:
#    print('VCI_StartCAN 1 ERROR\r\n')
 
#通道1发送数据
ubyte_array = c_ubyte*8
sbuf = ubyte_array(0x1A, 0x11, 0x21, 0x08, 0x44, 0, 0x7F, 0xD3)
ubyte_3array = c_ubyte*3
b = ubyte_3array(0, 0 , 0)
vci_can_obj = VCI_CAN_OBJ(0x1c4, 0, 0, 0, 0, 0,  8, sbuf, b)
 
#通道0接收数据
rbuf = ubyte_array(0, 0, 0, 0, 0, 0, 0, 0)
vci_can_obj2 = VCI_CAN_OBJ(0x0, 0, 0, 0, 0, 0,  8, rbuf, b)

for i in range(10000):
    print(i, '#'*10)
    ret = canDLL.VCI_Transmit(DEVICE_ID, 0, 0, byref(vci_can_obj), 1)
    if ret != STATUS_OK:
        print('VCI_Transmit ERROR\r\n')

    ret = canDLL.VCI_Receive(DEVICE_ID, 0, 0, byref(vci_can_obj2), 1, 0)
    if ret > 0:
        print(vci_can_obj.DataLen, list(vci_can_obj.Data))

    time.sleep(0.03)
 
#关闭
canDLL.VCI_CloseDevice(DEVICE_ID, 0) 
