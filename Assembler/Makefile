
CC=gcc
CFLAGS= -g -Wall -pedantic -ansi
MAIN = assembler
IDIR=./include
ODIR=./obj

SRCS = main.c firstPass.c secondPass.c line.c utils.c symbol.c output.c constants.c

_DEPS = $(SRCS:.c=.h) error.h assembler.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(SRCS:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(MAIN): $(OBJ)
	$(CC) $(CFLAGS) $< -o $@

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 