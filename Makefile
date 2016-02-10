#Makefile for mmoscoso-dis.c
CC=gcc
CFLAGS=-lm
mips-disassemblymake:
	$(CC) -o mips-disassembly mips-disassembly.c $(CFLAGS)

