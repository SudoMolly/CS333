COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror 
CCC = $(COMPILER) $(CFLAGS) -c
OCC = $(COMPILER) $(CFLAGS) -o
CAE = cae-xor
MYSTAT = mystat

all: cls
all: $(CAE)
all: $(MYSTAT)

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

$(CAE): cae-xor.o 
	$(OCC) $(CAE) cae-xor.o

cae-xor.o: cae-xor.c 
	$(CCC) cae-xor.c 

$(MYSTAT): mystat.o
	$(OCC) $(MYSTAT) -lbsd mystat.o -lbsd

mystat.o: mystat.c 
	$(CCC) -lbsd mystat.c -lbsd



clean cls: 
	rm -f *.o mystat cae-xor *~ \#*