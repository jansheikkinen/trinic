CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -g
INCLUDE = -Iinclude

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ = $(SRC:.c=.o)
BUILD = build

all: dirs uc

dirs:
	mkdir -p $(BUILD) src tests

run: uc
	$(BUILD)/uc build/test.uc

uc: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILD)/$@ $?

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	find build/ -type f ! -name '*.uc' -delete
	rm -Rf $(OBJ)
