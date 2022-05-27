# Simple Makefile

CC=gcc
CFLAGS=-Wall
SOURCE=source
INCLUDE=include
BUILD=build
EXEC=rutracker_parser
LIBS=-lsqlite3 -llibz -ljansson

all:
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(SOURCE)/*.* -I$(INCLUDE)/ $(LIBS) -o $(BUILD)/$(EXEC)

clean:
	rm -rf $(BUILD)