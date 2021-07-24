#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int pipefd[2]; // 0 = read, 1 = write
    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("pipe");
        exit(-1);
    }

    pid_t pid = fork();
    if (pid > 0) {
        close(pipefd[1]); // 禁止父进程写管道

        while (1) { // 读子进程信息
            char buf[1024] = {0};
            int count = read(pipefd[0], buf, sizeof(buf) - 1);
            printf("%s", buf);
        }
    } else if (pid == 0) {
        close(pipefd[0]); // 禁止子进程读管道

        int fd = dup2(STDOUT_FILENO, pipefd[1]);
        execl("/usr/bin/ps", "ps", "aux", NULL);
    } else {
        perror("fork");
        exit(-1);
    }

    return 0;
}
