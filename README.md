opnaplay
========

opnaplay - tweaking PC-9801-86 FM-synth part on OpenBSD/luna88k

This is my experimental program to use the FM-synth part of PC-9801-86 sound
board on OpenBSD/luna88k.

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
% ./opnaplay
```
