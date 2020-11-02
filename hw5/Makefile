# Usage:
# make run          <- run program
# make debug        <- debug program
# make submission   <- auto generate submission folder

################################ VARIABLES #####################################
PROGNAME = hw5
INPUT_FILE = input/address.png
OUTPUT_FILE = out/address_threshold.png

################################ COMPILER ######################################
LINKER=gcc
LDFLAGS =-Llibdip
LDLIBS =-ldip -lpng -lz -lm -fopenmp
CC =gcc
CFLAGS = -U__STRICT_ANSI__ -fopenmp -I$(LIBDIR) -I$(SRCDIR)
CFLAGS += -DDEBUG -g -Wall
################################ DIRECTORIES ###################################
SUBMISSIONDIR =submission
OUTDIR =out
SRCDIR =src
OBJDIR =obj
BINDIR =bin
LIBDIR =/usr/lib
################################ FILES #########################################
SOURCES := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

############################DEFAULT DIRECTIVE ##################################
all: create_dirs libdip $(BINDIR)/$(PROGNAME)

$(BINDIR)/$(PROGNAME): $(OBJECTS) libdip
	$(LINKER) -o $@ $(OBJECTS) $(LDFLAGS) $(LDLIBS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(SRCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@
################################ TARGETS #######################################

.PHONY: libdip
libdip:
	$(MAKE) -C libdip

.PHONY: run
run: all
	./$(BINDIR)/$(PROGNAME) -v -i input/keys.png -o out/keys_hw5.png
	./$(BINDIR)/$(PROGNAME) -v -c 0 -m 0 -i input/book.png -o out/book_hw5.png

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
submission:
	mkdir -p $(SUBMISSIONDIR)
	rm -rf $(SUBMISSIONDIR)/*
	rsync -av ./* $(SUBMISSIONDIR) --exclude=$(SUBMISSIONDIR)
	zip -r hw4.zip $(SUBMISSIONDIR)
