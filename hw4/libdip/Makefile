# Usage:
# make all
################################ COMPILER ######################################
LINKER=gcc
LDFLAGS =-L$(BINDIR)
LDLIBS =-ldip -lpng -lz -lm -fopenmp
CC =gcc
CFLAGS = -U__STRICT_ANSI__ -fopenmp -I$(LIBDIR) -I$(SRCDIR)
CFLAGS += -DDEBUG -g -Wall
################################ DIRECTORIES ###################################
LIBDIP_SRCDIR =src
LIBDIP_OBJDIR =obj
################################ FILES #########################################
LIBDIP_SOURCES  := $(wildcard $(LIBDIP_SRCDIR)/*.c)
LIBDIP_INCLUDES := $(wildcard $(LIBDIP_SRCDIR)/*.h)
LIBDIP_OBJECTS  := $(LIBDIP_SOURCES:$(LIBDIP_SRCDIR)/%.c=$(LIBDIP_OBJDIR)/%.o)
############################DEFAULT DIRECTIVE ##################################
all: create_dirs libdip
################################ LIBDIP ########################################

.PHONY: libdip
libdip: libdip.a

libdip.a: $(LIBDIP_OBJECTS)
	ar rc libdip.a $(LIBDIP_OBJECTS)

$(LIBDIP_OBJECTS): $(LIBDIP_OBJDIR)/%.o : $(LIBDIP_SRCDIR)/%.c
	$(CC) -I$(LIBDIP_SRCDIR) -c $< -o $@

.PHONY: create_dirs
create_dirs:
	mkdir -p $(LIBDIP_OBJDIR)

.PHONY: clean
clean:
	rm -rf $(LIBDIP_OBJDIR) libdip.a
