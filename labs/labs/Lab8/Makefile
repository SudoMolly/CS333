# R Jesse Chaney
# rchaney@pdx.edu

CC = gcc
DEBUG = -g3 -O0
#DEFINES += -DDEBUG

# some of this is redundant. the --coverage does the other 2
GCOV_FLAGS = -fprofile-arcs -ftest-coverage --coverage
WERROR = 
WERROR = -Werror
STD = -std=c11
CFLAGS = $(DEBUG) $(DEFINES) \
		-Wall -Wextra -Wshadow -Wunreachable-code \
		-Wredundant-decls -Wmissing-declarations \
		-Wold-style-definition -Wmissing-prototypes \
		-Wdeclaration-after-statement -Wno-return-local-addr \
		-Wunsafe-loop-optimizations -Wuninitialized \
		-Wno-unused-parameter $(WERROR) $(GCOV_FLAGS) $(STD)
LDFLAGS = $(GCOV_FLAGS)

PROG1 = example1
PROG2 =
PROG3 = 

PROGS = $(PROG1) $(PROG2) $(PROG3)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(LDFLAGS) -o $@ $^

$(PROG1).o: $(PROG1).c
	rm -f *.gcov *.gcda *.gcno
	$(CC) $(CFLAGS) -c $<

cov coverage gcov: all
	gcov -c example1

# clean up the compiled files and editor chaff
clean cls:
	rm -f $(PROGS) *.o *~ \#* *.gcov *.gcda *.gcno

ci:
	if [ ! -d RCS ] ; then mkdir RCS; fi
	ci -t-none -m"${LOGNAME} lazy-coverage" -l *.[ch] ?akefile *.bash

# https://www.youtube.com/watch?v=4m48GqaOz90
gotta git gat:
	if [ ! -d .git ] ; then git init; fi
	git add *.[ch] ?akefile *.bash
	git commit -m "The Black Eyed Peas - check in"

tar:
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[ch] [Mm]akefile
	tar tvaf $(TAR_FILE)
