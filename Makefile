include	Makefile.config

PROJECT=jagroid
SRCC=main.c border.c draw_map.c map.c
SRCS=data.s
SRCH=
OBJS=$(SRCC:.c=.o) $(SRCS:.s=.o) 
OTHEROBJS=
RMVLIBS=display.o collision.o interrupt.o paula.o protracker.o rmvlib.a

include	Makefile.example
