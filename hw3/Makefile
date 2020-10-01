PROGNAME = hw3

LINKER = gcc
LDFLAGS =  L$(PROVIDED_LIBDIR) -L$(LIBDIR)
LDLIBS = -ldip -lpng -lz -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin

LIBDIR = /usr/lib
PROVIDED_LIBDIR = ./provided_libs

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

CC = gcc
CFLAGS = -g -Wall -I$(PROVIDED_LIBDIR)

all:$(BINDIR)/$(PROGNAME)

$(BINDIR)/$(PROGNAME): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -i $@


$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@


hw3:$(BUILD)/libdip.a

$(BUILD)/libdip.a:$(BUILD)$(LIBOBJECTS)
	ar rc $(BUILD)/libdip.a $(LIBOBJECTS)

.PHONY: clean
clean:
	@ rm -f $(OBJECTS) $(BINDIR)



