#!/usr/bin/env python3
"""JB-22 ARM link budget gate; checks actual linker memory usage.
ITCM has only 16KiB and the prior JB-20 build consumed 16376/16384 B.
A successful link with 8 bytes spare is not a maintainable release margin.
"""
from pathlib import Path
import re
import sys

p=Path(sys.argv[1]) if len(sys.argv)>1 else Path("build/release/arm-build.log")
s=p.read_text(encoding="utf-8",errors="replace")
m=re.search(r"ITCM_RAM:\s*(\d+)\s*B\s*16\s*KB",s)
if not m:
    sys.exit("FAIL JB-22: could not verify linker ITCM usage from build log")
used=int(m.group(1))
total=16384
remain=total-used
print(f"JB-22 ITCM_RAM: {used}/{total} B; {remain} B headroom")
if used>total:
    sys.exit("FAIL JB-22: ITCM overflow")
if remain<512:
    sys.exit("FAIL JB-22: less than 512 B ITCM headroom; don't approve firmware")
if "region `ITCM_RAM` overflowed" in s:
    sys.exit("FAIL JB-22: linker overflow warning despite reported size")
print("PASS JB-22: at least 512 B ITCM headroom")
