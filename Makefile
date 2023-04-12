include	Makefile.config

PROJECT=jagroid
SRCC=main.c border.c image.c draw_map.c map.c
SRCS=data.s
SRCH=
OBJS=$(SRCC:.c=.o) $(SRCS:.s=.o) 
OTHEROBJS=
RMVLIBS=display.o interrupt.o paula.o protracker.o gpudriver.o lz77.o rmvlib.a

include	Makefile.example
