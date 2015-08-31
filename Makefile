# Makefile

PROG = opnaplay
SRCS = opnaplay.c opna.c soundlib.c \
	mml.c mml_stack.c ntlibc.c \
	mml_callback.c
NOMAN = 1

.include <bsd.prog.mk>
