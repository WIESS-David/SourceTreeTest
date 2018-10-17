from ctypes import *
import time

# give location of dll
candll = windll.LoadLibrary("ControlCAN.dll")
VCI_USBCAN_2E_U = 21
BYTE = c_ubyte
WORD = c_ushort
DWORD = c_int32
UCHAR = c_uint8
ULONG = DWORD
UINT = c_uint
m_DevType = VCI_USBCAN_2E_U
m_DevIndex = 0
m_nCanIndex = 0
	
class VCI_INIT_CONFIG(Structure):
		_fields_ = [("AccCode", DWORD),
			("AccMask", DWORD),
			("Reserved", DWORD),
			("Filter", UCHAR),
			("Timing0", UCHAR),
			("Timing1", UCHAR),
			("Mode", UCHAR)
			]
			
cfg = VCI_INIT_CONFIG()
cfg.AccCode = 0x00000000
cfg.AccMask = 0xFFFFFFFF
cfg.Reserved = 0
cfg.Filter = 0
cfg.Timing0 = 0x00 
cfg.Timing1 =0x1C
cfg.Mode = 0 
	
def init():
	vci_open_device = candll.VCI_OpenDevice
	vci_open_device.restype = DWORD
	vci_open_device.argtypes = [DWORD, DWORD, DWORD]
	status = vci_open_device(m_DevType, m_DevIndex, 0)
	print('VCI_OpenDevice status', status) #1 is ok

	

	# https://www.cnblogs.com/xinyuyuanm/archive/2013/04/25/3043396.html
	baudrate  = DWORD(0x060007)

	VCI_SetReference = candll.VCI_SetReference
	VCI_SetReference.restype = DWORD
	VCI_SetReference.argtypes = [DWORD, DWORD, DWORD, DWORD, POINTER(DWORD)]
	status = VCI_SetReference(m_DevType, m_DevIndex, m_nCanIndex, 0, byref(baudrate))
	print('VCI_SetReference status', status) #1 is ok


	VCI_InitCAN = candll.VCI_InitCAN
	VCI_InitCAN.restype = DWORD
	VCI_InitCAN.argtypes = [DWORD, DWORD, DWORD, POINTER(VCI_INIT_CONFIG) ]
	#m_DevType, m_DevIndex, m_nCanIndex, InitInfo
	status = VCI_InitCAN(m_DevType, m_DevIndex, m_nCanIndex, byref(cfg))
	print('VCI_InitCAN status', status) #1 is ok


	VCI_StartCAN = candll.VCI_StartCAN
	VCI_StartCAN.restype = DWORD
	VCI_StartCAN.argtypes = [DWORD, DWORD, DWORD ]
	status = VCI_StartCAN(m_DevType, m_DevIndex, m_nCanIndex)
	print('VCI_StartCAN status', status) #1 is ok

init()


class VCI_CAN_OBJ(Structure):
		_fields_ = [("ID", UINT),
			("TimeStamp", UINT),
			("TimeFlag", BYTE),
			("SendType", BYTE),
			("RemoteFlag", BYTE),
			("ExternFlag", BYTE),
			("DataLen", BYTE),
			("Data", BYTE * 8),
			("Reserved", BYTE * 3),
			]

			

data = VCI_CAN_OBJ()
data.ID = 0x1C4
data.SendType = 0 #0:retry
data.RemoteFlag = 0
data.ExternFlag = 0
data.DataLen = 8
data.Data = (BYTE * 8)(*[0x1A, 0x11, 0x21, 0x08, 0x44, 0x00, 0x7F, 0xD3]) #1A 11 21 08 44 00 7F D3
data.Reserved = (BYTE * 3)(*[0, 0, 0])
 
for i in range(30):
	VCI_Transmit = candll.VCI_Transmit
	VCI_Transmit.restype = DWORD
	VCI_Transmit.argtypes = [DWORD, DWORD, DWORD,  POINTER(VCI_CAN_OBJ), DWORD]
	status = VCI_Transmit(m_DevType, m_DevIndex, m_nCanIndex, byref(data), 1)
	print('VCI_Transmit status', status) #1 is ok
      


