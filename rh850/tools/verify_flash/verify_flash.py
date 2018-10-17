import random

from hexformat.srecord import SRecord
from hexformat.hexdump import HexDump

srec = SRecord.frombinfile("C:\\git\\rh850\\src\\app\\image_bin\\data_flash_sf.rec") 
print(srec)
print('len:', '%X' % len(srec[:]))
print('startaddress', srec.startaddress)
#print(HexDump(srec))

testdata = bytearray.fromhex("0001 0203 0405 0607 0809 0A0B 0C0D 0000")
srec = SRecord()
srec.startaddress = None