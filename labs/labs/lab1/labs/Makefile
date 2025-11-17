COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror 
CCC = $(COMPILER) $(CFLAGS) -c
OCC = $(COMPILER) $(CFLAGS) -o
FILENAME = lab1

all: $(FILENAME)

#--------------------debug

debug: CFLAGS += -ggdb -gdwarf-2 -g3
debug: all

#DEBUG: cae-xor.o mystat.o generalFuncs.o
#	$(OCC) -o DEBUG -g cae-xor.o mystat.o generalFuncs.o
#cae-xor.o: cae-xor.c 
#	$(CCC) -c -g cae-xor.c 
#mystat.o: mystat.c 
#	$(CCC) -c -g mystat.c 
#generalFuncs.o: generalFuncs.c generalFuncs.h
#	$(CCC) -c -g generalFuncs.c generalFuncs.h

#---------------Normal

$(FILENAME): cae-xor.o mystat.o generalFuncs.o
	$(OCC) $(FILENAME) cae-xor.o mystat.o generalFuncs.o

cae-xor.o: cae-xor.c 
	$(CCC) cae-xor.c 

mystat.o: mystat.c 
	$(CCC) mystat.c 

generalFuncs.o: generalFuncs.c generalFuncs.h
	$(CCC) generalFuncs.c generalFuncs.h


clean cls: 
	rm -f *.o lab1 DEBUG *~ \#*