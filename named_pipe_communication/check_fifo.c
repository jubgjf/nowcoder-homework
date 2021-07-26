#include "header.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int check_fifo() {
    if (access("fifo1", F_OK) == -1) {
        if (mkfifo("fifo1", 0664) == -1) {
            perror("mkfifo");
            return 0;
        }
    }

    if (access("fifo2", F_OK) == -1) {
        if (mkfifo("fifo2", 0664) == -1) {
            perror("mkfifo");
            return 0;
        }
    }

    return 1;
}
