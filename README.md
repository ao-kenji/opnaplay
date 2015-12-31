opnaplay
========

opnaplay - A tiny YM2608(ONPA) FM-synth MML player on OpenBSD/luna88k

This is my experimental program to use the YM2608(OPNA) FM-synth part of
PC-9801-86 sound board on OpenBSD/luna88k.  It also runs with MELCO WSN-A[24]F.

Preparation
-----------
To use this program, make sure that 'pcex0' is recognized in dmesg.
```
cbus0 at mainbus0
pcex0 at cbus0
```

Compile
-------
Build by 'make'.  The executable binary is 'opnaplay'.

Run
---
```
% ./opnaplay [-d] [-t timbre number] 'MML strings'
```

Example
-------
This example plays 'Sakura Sakura' (Japanese song).
```
% ./opnaplay 't120 o4 l4 aab2 aab2 ab>c<b ab8a8f2 ecef ee8c8<b2> ab>c<b ab8a8f2 ecef ee8c8<b2> aab2 aab2 efb8a8f e1'
```

Special Thanks
--------------
This program uses: 

'A tiny MML parser'  
    Copyright (C) 2014-2015 Shinichiro Nakamura (CuBeatSystems)  
    http://www.cubeatsystems.com/firmware/tinymml/index.html

FM sound parameters  
    VALSOUND FM-Library / (C)Takeshi Abo.  
    http://www.valsound.com/

WSN-A[24]F initialization:
    WSN-A2FのPCM音源をFreeBSDで使う
    http://s-sasaji.ddo.jp/pcunix/bxpcm.htm
