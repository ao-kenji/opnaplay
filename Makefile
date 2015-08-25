# Makefile

PROG = opnaplay
SRCS = opnaplay.c opna.c soundlib.c
#LDFLAGS += -lm
#BINDIR = /usr/local/bin
NOMAN = 1

.include <bsd.prog.mk>
