"""Validate the desktop SD application against its linked raw binary."""
import hashlib
from pathlib import Path
import struct
import sys
img=Path(sys.argv[1])
data=img.read_bytes()
magic,length,check=struct.unpack_from('<8sIB',data)
payload=data[16:]
assert magic==b'LXRV2IMG' and data[13:16]==bytes(3)
assert length==len(payload) and length<=992*1024 and check==255-(sum(payload)&255)
sp,pc=struct.unpack_from('<II',payload)
assert 0x20000000<sp<=0x20080000 and sp%8==0
assert pc&1 and 0x08008000<=pc-1<0x08008000+length
assert b'1.8.01' in payload
binary,=img.parent.glob('*.bin')
assert binary.read_bytes()==payload
print('STM32F765VG desktop application. HARDWARE-UNVERIFIED.')
print('Image',img.name,'payload',length,'initial SP',hex(sp),'reset PC',hex(pc))
print('SHA256',hashlib.sha256(data).hexdigest())
