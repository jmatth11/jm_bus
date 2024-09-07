CC=gcc
CFLAGS=-Wall -std=c11
PFLAGS=-DDEBUG=1
LIBS=-lm
INCLUDES=-I../deps/ -I.
OBJ=obj
BIN=bin
CLIENT_SOURCES=handlers/messages.c helpers/bytes.c helpers/log.c helpers/socket.c handlers/client.c
CLIENT_OBJS=$(addprefix $(OBJ)/,$(CLIENT_SOURCES:%.c=%.o))
SOURCES=$(shell find . -name '*.c' -not -path './plugins/*' -not -path './deps/*')
OBJECTS=$(addprefix $(OBJ)/,$(SOURCES:%.c=%.o))
DEBUG_OBJECTS=$(patsubst %.c, $(OBJ)/%-debug.o, $(SOURCES))
DEPS=$(shell find . -name Makefile -printf '%h\n' | grep -v 'unittest' | grep -v '^.$$')
ARCHIVE_NAME=libjm_bus
ARCHIVE_DEBUG_NAME=libjm_bus-debug
ARCHIVE_CLIENT=libjm_bus_client
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
	$(CC) -ggdb -fPIC -c -o $@ $< $(PFLAGS) $(CFLAGS) $(INCLUDES)

.PHONY: archive
archive: $(OBJECTS)
	@mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $((RCHIVE_DIR)/$(ARCHIVE_NAME).so $^ $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_NAME).a $^

.PHONY: archive-debug
archive-debug: $(OBJECTS)
	@mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $(ARCHIVE_DIR)/$(ARCHIVE_DEBUG_NAME).so $^ $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_DEBUG_NAME).a $^

.PHONY: archive-client
archive-client: $(CLIENT_OBJS)
	@mkdir -p $(ARCHIVE_DIR)
	gcc -shared -fPIC -o $(ARCHIVE_DIR)/$(ARCHIVE_CLIENT).so $^ $(LIBS)
	ar -r $(ARCHIVE_DIR)/$(ARCHIVE_CLIENT).a $^

.PHONY: clean
clean:
	@rm -rf $(OBJ)/* 2> /dev/null
	@rm -f $(ARCHIVE_DIR)/* 2> /dev/null

