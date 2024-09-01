CC=gcc
CFLAGS=-Wall -std=c11
LIBS=-lm
INCLUDES=-I../deps/ -I.
OBJ=obj
BIN=bin
SOURCES=$(shell find . -name '*.c' -not -path './plugins/*' -not -path './deps/*')
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
DEPS=$(shell find . -name Makefile -printf '%h\n' | grep -v 'unittest' | grep -v '^.$$')
ARCHIVE_NAME=libjm_bus.a
SHARED_NAME=libjm_bus.so
ARCHIVE_DEBUG_NAME=libjm_bus-debug.a
SHARED_DEBUG_NAME=libjm_bus-debug.so
ARCHIVE_DIR=lib

.PHONY: all
all: archive

$(OBJ)/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -fPIC -c -o $@ $< $(CFLAGS) $(INCLUDES)

$(OBJ)/%-debug.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(BIN)
	$(CC) -ggdb -fPIC -c -o $@ $< $(CFLAGS) $(INCLUDES)

.PHONY: archive
archive: $(OBJECTS)
	@mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $(ARCHIVE_DIR)/$(SHARED_NAME) $^ $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_NAME) $^

.PHONY: archive-debug
archive-debug: $(OBJECTS)
	@mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $(ARCHIVE_DIR)/$(SHARED_DEBUG_NAME) $^ $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_DEBUG_NAME) $^

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(ARCHIVE_DIR)/* 2> /dev/null

