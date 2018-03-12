
CC=gcc
CFLAGS= -Wall -pedantic -ansi -g -o0
MAIN = assembler
IDIR=./include
ODIR=./obj

SRCS = main.c firstPass.c secondPass.c line.c utils.c symbol.c output.c constants.c instruction.c

_OBJ = $(SRCS:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS = main.h assembler.h firstPass.h secondPass.h instruction.h utils.h errors.h line.h output.h symbol.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
	
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -I"$(DEPS)"
	
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 