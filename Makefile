CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -O0
INCLUDE = -Iinclude

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BUILD = build

all: dirs uc

dirs:
	mkdir -p $(BUILD) src include tests

run: uc
	$(BUILD)/uc

uc: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILD)/$@ $?

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -Rf $(BUILD)/* $(OBJ)
