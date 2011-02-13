VPATH = ../src
CFLAGS = -Wall -pedantic -I../headers

objects = functions.o test_rm_comments.o
progs = test_rm_comments

test_rm_comments : $(objects)

.PHONY : clean
clean :
	rm -f $(objects) $(progs)
