CC = gcc
CFLAGS = -Wall -Wextra -g

server: server.c
	$(CC) $(CFLAGS) $^ -o $@

run: server
	./$^

clean:
	rm server