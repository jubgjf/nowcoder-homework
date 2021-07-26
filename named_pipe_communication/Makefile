CC = gcc
CFLAGS = -Wall -Wextra -g

all: client1 client2

client1: client1.c check_fifo.c header.h
	$(CC) $(CFLAGS) $^ -o $@

client2: client2.c check_fifo.c header.h
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm client1 client2 fifo1 fifo2