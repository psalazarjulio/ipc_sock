CC= clang
CFLAGS= -Wall

# Directories
#
DIR_INCLUDES=./include
DIR_SRC= ./src

# Files
HEADERS=$(DIR_INCLUDES)/*.h
SRC=$(DIR_SRC)/*.c

EXECUTABLES= node

all: node

node: $(COMMONS_H)
	$(CC) $(CFLAGS) $(SRC) -I $(DIR_INCLUDES) -o $@


.PHONY: clean

clean:
	rm -f $(EXECUTABLES)
