#
# R Jesse Chaney
# 

CC = gcc
DEBUG = -g3 -O0 -pg
DEFINES = 

WERROR = -Werror
CFLAGS = $(DEBUG) --no-warnings -Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls \
        -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
        -Wdeclaration-after-statement -Wno-return-local-addr \
		-Wunsafe-loop-optimizations -Wuninitialized $(WERROR) $(DEFINES)
LDFLAGS =
LDFLAGS += $(DEBUG)
PERMS = a+rx,g-w
PROG1 = test_gprof1
PROG2 = 
PROG3 = 
PROG4 = 
PROGS = $(PROG1) $(PROG2) $(PROG3) $(PROG4)

all: $(PROGS)


$(PROG1): $(PROG1).o test_gprof2.o
	$(CC) -o $@ $^ $(LDFLAGS)
	chmod $(PERMS) $@

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $<

test_gprof2.o: test_gprof2.c
		$(CC) $(CFLAGS) -c $<

collect coll: cls all
	time ./$(PROG1) > $(PROG1).out
	gprof $(PROG1) gmon.out > $(PROG1).gprof

# clean up the compiled files and editor chaff
clean cls:
	rm -f $(PROGS) *.o *~ \#* gmon.out gprof.out $(PROG1).out $(PROG1).gprof

ci:
	if [ ! -d RCS ] ; then mkdir RCS; fi
	ci -t-none -m"lazy-checkin" -l *.[ch] ?akefile

co:
	co -f -l RCS/*.txt,v RCS/*.bin,v
