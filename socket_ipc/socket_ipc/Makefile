CC= clang
CFLAGS= -Wall

INCLUDES=./include
SRC= ./src
COMMONS=$(INCLUDES)/general.h

EXECUTABLES= srv cli

all: srv cli

srv: $(INCLUDES)/server.h $(COMMONS)
	$(CC) $(CFLAGS) $(SRC)/server.c -I $(INCLUDES) -o $@

cli: $(INCLUDES)/client.h $(COMMONS)
	$(CC) $(CFLAGS) $(SRC)/client.c -I $(INCLUDES) -o $@

.PHONY: clean

clean:
	rm -f $(EXECUTABLES)
