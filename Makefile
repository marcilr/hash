##
# Makefile automatically generated by genmake 1.0, Aug-07-02
# genmake 1.0 by ma_muquit@fccc.edu
##

SUB = ../list

CC= cc
DEFS=
PROGNAME= main
INCLUDES=  -I.
LIBS=

# replace -O with -g in order to debug

DEFINES= $(INCLUDES) $(DEFS) -DSYS_UNIX=1
CFLAGS= -g -O $(DEFINES)

SRCS = hash.c main.c

OBJS = hash.o str.o main.o

.c.o:
	rm -f $@
	$(CC) $(CFLAGS) -c $*.c

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(PROGNAME) core

cycle: clean all

test:
	valgrind --leak-check=yes ./$(PROGNAME)

