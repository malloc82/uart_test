CC  := /usr/bin/gcc
SRC := main.c

CFLAGS  := 
LDFLAGS := 

BINDIR := bin
EXE    := $(BINDIR)/uart

OBJDIR := obj

uart: makedirectory
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(EXE)

makedirectory:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
