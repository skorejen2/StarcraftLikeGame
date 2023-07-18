CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =

EXECUTABLE = playerOne

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): playerOne.c
    $(CC) $(CFLAGS) playerOne.c -o $(EXECUTABLE)

clean:
    $(RM) $(EXECUTABLE)