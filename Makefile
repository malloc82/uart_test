CC      := /usr/bin/gcc
ARMCC   := /home/drspaceman/sandy/buildroot-2013.05/output/host/usr/bin/arm-linux-gcc
SRC_DIR := src
SRC     := $(SRC_DIR)/main.c

CFLAGS  := 
LDFLAGS := 

BINDIR  := bin
EXE     := $(BINDIR)/uart

OBJDIR  := obj

uart: makedirectory
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(EXE)

makedirectory:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

