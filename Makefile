CC      := /usr/bin/gcc
ARMCC   := /home/drspaceman/sandy/buildroot-2013.05/output/host/usr/bin/arm-linux-gcc
SRCDIR  := src
OBJDIR  := obj
SRC     := $(SRCDIR)/main.c \
           $(SRCDIR)/uart.c

OBJ     := $(patsubst %.c,$(OBJDIR)/%.c.o,$(notdir $(SRC)))
INCLUDE := -Iinclude
CFLAGS  := 
LDFLAGS := 

DEBUG   := 

BINDIR  := bin
EXE     := $(BINDIR)/uart


$(EXE): makedirectory $(SRC) $(OBJ)
	$(CC) $(LIB) $(LDFLAGS) $(OBJ) -o $@
$(OBJDIR)/%.c.o: $(SRCDIR)/%.c
	$(VERBOSE) $(CC) $(DEBUG) $(CFLAGS) $(INCLUDE) -c $< -o  $@

makedirectory:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	$(VERBOSE) rm -rf $(OBJDIR) $(BINDIR)
