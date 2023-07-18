CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: playerOne

playerOne: playerOne.c
	$(CC) $(CFLAGS) -o playerOne playerOne.c

clean:
	rm -f playerOne