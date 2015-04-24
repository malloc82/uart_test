CC      := /usr/bin/gcc
# CC      := /home/drspaceman/sandy/buildroot-2013.05/output/host/usr/bin/arm-linux-gcc
SRCDIR  := src
OBJDIR  := obj

SRC     := $(SRCDIR)/main.c \
           $(SRCDIR)/uart.c

GHD_SRC := $(SRCDIR)/GHD.c \
           $(SRCDIR)/uart.c

GSD_SRC := $(SRCDIR)/GSD.c \
           $(SRCDIR)/uart.c

TX_SRC  := $(SRCDIR)/tx_test.c \
           $(SRCDIR)/uart.c

RX_SRC  := $(SRCDIR)/rx_test.c \
           $(SRCDIR)/uart.c

OBJ     := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(SRC)))
GHD_OBJ := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(GHD_SRC)))
GSD_OBJ := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(GSD_SRC)))
TX_OBJ  := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(TX_SRC)))
RX_OBJ  := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(RX_SRC)))

INCLUDE := -Iinclude
CFLAGS  := -Wall
LDFLAGS := 

DEBUG   :=  -g

BINDIR  := bin
UART_BIN := $(BINDIR)/uart
GHD_BIN  := $(BINDIR)/GHD
GSD_BIN  := $(BINDIR)/GSD
TX_TEST_BIN := $(BINDIR)/tx_test
RX_TEST_BIN := $(BINDIR)/rx_test

$(UART_BIN): makedirectory $(SRC) $(OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(OBJ) -o $@

GHD: $(GHD_BIN)

GSD: $(GSD_BIN)

TX_TEST: $(TX_TEST_BIN)

RX_TEST: $(RX_TEST_BIN)

$(GHD_BIN): makedirectory $(GHD_SRC) $(GHD_OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(GHD_OBJ) -o $@

$(GSD_BIN): makedirectory $(GSD_SRC) $(GSD_OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(GSD_OBJ) -o $@

$(TX_TEST_BIN): makedirectory $(TX_SRC) $(TX_OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(TX_OBJ) -o $@

$(RX_TEST_BIN): makedirectory $(RX_SRC) $(RX_OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(RX_OBJ) -o $@

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(VERBOSE) $(CC) $(DEBUG) $(CFLAGS) $(INCLUDE) -c $< -o $@

makedirectory:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	$(VERBOSE) rm -rf $(OBJDIR) $(BINDIR)
