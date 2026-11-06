from pathlib import Path
import re,sys
fw,out=map(Path,sys.argv[1:])
s=(fw/'Core/Inc/Soundengine/config.h').read_text()
s=re.sub(r'(#define (?:INDTCMZ?|IN_ITCM_RAM))[^\n]*',r'\1',s)
(out/'host_config.h').write_text(s)
s=(fw/'Core/Src/Soundengine/DSPAudio/modulationNode.c').read_text()
s=s.replace('const Parameter *p=&parameterArray[idx];', 'const Parameter *p=&parameterArray[idx]; if(!p->ptr)return;')
(out/'modulationNode.c').write_text(s)

s=(fw/'Middlewares/Third_Party/FatFs/src/diskio.c').read_text().replace('get_fattime (void)', 'emu_unused_weak_fattime (void)')
(out/'diskio.c').write_text(s)
