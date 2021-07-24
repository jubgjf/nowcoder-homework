CC = gcc
CFLAGS = -Wall -Wextra -g

main: main.c
	$(CC) $(CFLAGS) $^ -o $@

run: main
	./$^

clean:
	rm main