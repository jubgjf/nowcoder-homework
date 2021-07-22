CC = gcc
CFLAGS = -Wall -Wextra -g

ls: ls.c
	$(CC) $(CFLAGS) $^ -o $@

run: ls
	./$^

clean:
	rm ls