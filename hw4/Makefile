# Usage:
# make run          <- run program
# make debug        <- debug program
# make submission   <- auto generate submission folder
PROGNAME = thresh
INPUT_FILE = input/address.png
OUTPUT_FILE = out/address_threshold.png

LINKER=gcc
LDFLAGS =-L$(BINDIR)
LDLIBS =-ldip -lpng -lz -lm -fopenmp
CC =gcc
CFLAGS =-fopenmp -I$(LIBDIR) -I$(SRCDIR)
CFLAGS += -DDEBUG -g -Wall

SUBMISSIONDIR =submission
OUTDIR =out
SRCDIR =src
OBJDIR =obj
BINDIR =bin
LIBDIR =/usr/lib

SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# LIBDIP_OBJECTS groups every source other than hw3.c and hw3.h
LIBDIP_OBJECTS := $(filter-out $(PROGNAME)%.c, $(OBJECTS))

.PHONY: all
all: create_dirs $(BINDIR)/$(PROGNAME)
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(BINDIR)/$(PROGNAME): $(OBJECTS) $(BUILD)/libdip.a
	$(LINKER) obj/$(PROGNAME).o $(LDFLAGS) $(LDLIBS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/libdip.a: $(OBJECTS)
	ar rc $(BINDIR)/libdip.a $(LIBDIP_OBJECTS)

.PHONY: run
run: clean all
	./$(BINDIR)/$(PROGNAME) $(INPUT_FILE) $(OUTPUT_FILE)

.PHONY: graybook
graybook: all
	./$(BINDIR)/$(PROGNAME) input/graybook.png $(OUTDIR)/graybook_thresh.png
.PHONY: create_dirs
create_dirs:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)
	mkdir -p out

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR) $(SUBMISSIONDIR) $(OUTDIR)

.PHONY: submission
submission: run
	mkdir -p $(SUBMISSIONDIR)
	echo "TODO: implement"
