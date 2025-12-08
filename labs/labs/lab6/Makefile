COMPILER = gcc 
CFLAGS =-Wall -Wextra -Wshadow -Wunreachable-code -Wredundant-decls  -Wmissing-declarations -Wold-style-definition  -Wmissing-prototypes -Wdeclaration-after-statement  -Wno-return-local-addr -Wunsafe-loop-optimizations  -Wuninitialized -Werror -pthread
CCC = $(COMPILER) $(CFLAGS)
CLIENTD= ./server
SERVERD= ./client
SERVER = rockem_server
CLIENT = rockem_client
HEADER = rockem_hdr
MKDIR  = mkdir -p

default: all

debug: CFLAGS += -ggdb -gdwarf-2 -g3 --define-macro=DEBUG
debug: all

$(SERVER): $(SERVER).o $(HEADER).h
	$(CCC) $(SERVER).o -o $(SERVER) 

$(SERVER).o: $(SERVER).c $(HEADER).h
	$(CCC) -c $(SERVER).c -o $(SERVER).o

$(CLIENT): $(CLIENT).o $(HEADER).h
	$(CCC) $(CLIENT).o -o $(CLIENT)

$(CLIENT).o: $(CLIENT).c $(HEADER).h
	$(CCC) -c $(CLIENT).c -o $(CLIENT).o

$(CLIENTD): 
	if [ -d "$(CLIENTD)"]; then $(MDKIR) $(CLIENTD); fi
$(SERVERD):
	if [ -d "$(SERVERD)"]; then $(MKDIR) $(SERVERD); fi

MKDIRS: $(CLIENTD)
MKDIRS: $(SERVERD)


# Then link
#$(FILENAME): $(FILENAME).o
#	$(COMPILER) -o $(FILENAME) $(FILENAME).o -lcrypto
#arvik-md4.o: arvik-md4.c arvik.h
#$(CCC)  arvik-md4.c

clean cls:
	rm -f *.o client/rockem_client.o client/rockem_client server/rockem_server.o server/rockem_server.o *~ \#*

all: clean
all: MKDIRS
all: $(SERVER)
all: $(CLIENT)