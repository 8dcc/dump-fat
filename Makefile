
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Wpedantic -Wshadow# -ggdb3 -fsanitize=address,undefined -fstack-protector-strong
LDLIBS=

SRC=main.c bytearray.c fat12.c print.c
OBJ=$(addprefix obj/, $(addsuffix .o, $(SRC)))

BIN=dump-fat.out

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

#-------------------------------------------------------------------------------

.PHONY: all clean install

all: $(BIN)

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

install: $(BIN)
	install -D -m 755 $^ -t $(DESTDIR)$(BINDIR)

#-------------------------------------------------------------------------------

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<
