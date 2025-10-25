COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror 
CCC = $(COMPILER) $(CFLAGS) -c
OCC = $(COMPILER) $(CFLAGS) -o
FILENAME1 = cae-xor
FILENAME2 = mystat

all: $(FILENAME1) $(FILENAME2)

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


$(FILENAME2): mystat.o 
	$(OCC) $(FILENAME2) mystat.o 

$(FILENAME1): cae-xor.o generalFuncs.o
	$(OCC) $(FILENAME1) cae-xor.o generalFuncs.o

cae-xor.o: cae-xor.c 
	$(CCC) cae-xor.c 

mystat.o: mystat.c 
	$(CCC) mystat.c 

generalFuncs.o: generalFuncs.c generalFuncs.h
	$(CCC) generalFuncs.c generalFuncs.h


clean cls: 
	rm -f *.o cae-xor mystat *~ \#*
#what