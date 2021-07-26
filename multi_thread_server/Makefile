CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

server: server.c
	$(CC) $(CFLAGS) $^ -o $@

run: server
	./$^

clean:
	rm server
