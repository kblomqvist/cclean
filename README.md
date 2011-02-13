# CCLEAN

Clean C source FILE(s) from comments and empty lines.
The cleaned file is written into a new file postfixed by ".clean".

#### QUICK START

Use eather make or make all to compile and link the program.

        make
        ./cclean -h

#### MAKE TARGETS

clean - cleans the project folder from object files, librarys and the program exec file.

dist  - makes a tar ball from the cleaned project folder

debug - compiles the program with -g and -DDEBUG compiler options
