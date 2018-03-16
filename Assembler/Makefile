
CC=gcc
CFLAGS= -Wall -pedantic -ansi -g -o0
MAIN = assembler
IDIR=./include
ODIR=./obj

SRCS = main.c firstPass.c secondPass.c line.c utils.c symbol.c output.c constants.c instruction.c external.c

_OBJ = $(SRCS:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = main.h assembler.h firstPass.h secondPass.h instruction.h utils.h errors.h line.h output.h symbol.h external.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

MKDIR_P = mkdir -p

.PHONY: directories

all: directories $(MAIN)

directories:
	${MKDIR_P} $(ODIR)

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@	

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -I"$(DEPS)"

	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
	rmdir $(ODIR)
