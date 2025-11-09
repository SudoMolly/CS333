COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror 
CCC = $(COMPILER) $(CFLAGS) -c
OCC = $(COMPILER) $(CFLAGS) -o
FILENAME = arvik-md4

default: all

debug: CFLAGS += -ggdb -gdwarf-2 -g3
debug: all

$(FILENAME): arvik-md4.o
	$(OCC) $(FILENAME) arvik-md4.o

arvik-md4.o: arvik-md4.c arvik.h
	$(CCC) -lmd arvik-md4.c

clean cls:
	rm -f *.o arvik-md4 *~ \#*

all: clean
all: $(FILENAME)