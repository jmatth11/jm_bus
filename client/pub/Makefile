CC=gcc
CFLAGS=-Wall -std=c11
LIBS=-lm -L../../common/lib/ -ljm_bus
LIBS_DEBUG=-lm -L../../common/lib/ -ljm_bus-debug
INCLUDES=-I./ -I../../common/
OBJ=obj
BIN=bin
SOURCES=$(shell find . -name '*.c' -not -path './plugins/*' -not -path './deps/*')
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
DEPS=$(shell find . -name Makefile -printf '%h\n' | grep -v 'unittest' | grep -v '^.$$')
TARGET=jm_bus_client_pub

.PHONY: all
all: src

.PHONY: src
src: $(OBJECTS)
	$(CC) $^ -O2 $(CFLAGS) $(LIBS) -o $(BIN)/$(TARGET)

.PHONY: debug
debug: debugsrc

.PHONY: debugsrc
debugsrc: $(DEBUG_OBJECTS)
	$(CC) $^ -ggdb $(CFLAGS) $(LIBS_DEBUG) -o $(BIN)/$(TARGET)

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

$(OBJ)/%-debug.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -ggdb -c -o $@ $< $(CFLAGS) $(INCLUDES)

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(BIN)/* 2> /dev/null

