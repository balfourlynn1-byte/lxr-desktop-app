#!/usr/bin/env python3
"""Independently validate the desktop update container and Cortex-M vector addresses."""
import argparse,hashlib,struct
from pathlib import Path
p=argparse.ArgumentParser(); p.add_argument('image',type=Path); p.add_argument('--binary',type=Path); args=p.parse_args()
b=args.image.read_bytes(); assert len(b)>24,'Truncated image'
magic,size,checksum=struct.unpack('<8sIB3x',b[:16]); payload=b[16:]
assert magic==b'LXRV2IMG','Wrong device magic'
assert size==len(payload) and 0<size<0xF7FFF,'Invalid payload size'
assert b[13:16]==bytes(3),'Nonzero reserved header bytes'
assert checksum==0xFF-(sum(payload)&255),'Checksum mismatch'
sp,pc=struct.unpack('<II',payload[:8])
assert 0x20000000<=sp<=0x20080000 and sp%8==0,'Invalid initial stack'
assert pc&1 and 0x08008000<=(pc&~1)<0x08008000+size,'Invalid reset vector'
if args.binary: assert args.binary.read_bytes()==payload,'Container differs from built binary'
print(f'PASS {args.image.name}: {len(b)} bytes; payload {size}; reset {pc:#010x}; stack {sp:#010x}')
print('SHA256 '+hashlib.sha256(b).hexdigest())
