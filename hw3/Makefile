PROGNAME = hw3

LINKER=gcc
LDFLAGS =-L$(BINDIR)
LDLIBS =-ldip -lpng -lz -lm
CC =gcc
CFLAGS =-g -Wall -I$(LIBDIR) -I$(SRCDIR)

SRCDIR =src
OBJDIR =obj
BINDIR =bin
LIBDIR =/usr/lib

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# LIBDIP_OBJECTS groups every source other than hw3.c and hw3.h
LIBDIP_OBJECTS := $(filter-out hw%.c, $(OBJECTS))

.PHONY: all
all: create_dirs $(BINDIR)/$(PROGNAME)
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(BINDIR)/$(PROGNAME): $(OBJECTS) $(BUILD)/libdip.a
	$(LINKER) obj/hw3.o $(LDFLAGS) $(LDLIBS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/libdip.a: $(OBJECTS)
	ar rc $(BINDIR)/libdip.a $(LIBDIP_OBJECTS)

.PHONY: run
run: all
	./bin/hw3

.PHONY: create_dirs
create_dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p out

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR) submission/ out/

.PHONY: submission
submission: run
	mkdir -p submission/
	echo "TODO: implement"
