#include "header.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    if (!check_fifo()) {
        return -1;
    }

    while (1) {
        // client1 -> fifo1 -> client2
        // client1 <- fifo2 <- client2

        int fd1 = open("fifo1", O_RDONLY);
        char buf1[1024] = {0};
        read(fd1, buf1, sizeof(buf1) - 1);
        printf("client2 recv:%s", buf1);
        close(fd1);

        int fd2 = open("fifo2", O_WRONLY);
        char buf2[1024] = {0};
        fgets(buf2, sizeof(buf2) - 1, stdin);
        write(fd2, buf2, strlen(buf2));
        close(fd2);
    }

    return 0;
}
