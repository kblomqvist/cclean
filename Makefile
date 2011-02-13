includes = headers

VPATH = src:headers
CC = gcc
CFLAGS = -I$(includes) -Wall -pedantic -O2

objects = main.o functions.o
progs = libfuncs.a cclean

## Default target, build all progs
all : $(progs)

# Make predefined pattern rule, which compiles .c source files to
# .o object files. Repeated here for convenience.
%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

## Program executives
cclean : main.o libfuncs.a
	$(CC) -o $@ $^

## Libraries
libfuncs.a : functions.o
	ar rv $@ $^
	ranlib $@

## Header file dependencies
main.o : functions.h
functions.o : functions.h

## Phonies
.PHONY : clean debug dist
clean :
	-rm -f $(progs) $(objects) *.clean

debug : clean all
debug : CFLAGS += -g -DDEBUG

dist : clean
	cd .. && tar czvf cclean.tar.gz cclean/
