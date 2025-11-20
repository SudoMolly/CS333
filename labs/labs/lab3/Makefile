COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror 
CFLAGS += -Wno-string-compare -Wno-stringop-overflow -Wno-stringop-overread -Wno-stringop-truncation
CCC = $(COMPILER) $(CFLAGS)
FILENAME = thread_hash
OTHER_FILES =

default: all

debug: CFLAGS += -ggdb -gdwarf-2 -g3
debug: all

$(FILENAME).o: $(FILENAME).c 
	$(CCC) -c $(FILENAME).c $(FILENAME).h 

$(FILENAME): $(FILENAME).o
	$(COMPILER) -o $(FILENAME) $(FILENAME).o -lcrypt
#arvik-md4.o: arvik-md4.c arvik.h
#$(CCC)  arvik-md4.c

clean cls:
	rm -f *.o $(FILENAME) *~ \#*

all: clean
all: $(FILENAME)
