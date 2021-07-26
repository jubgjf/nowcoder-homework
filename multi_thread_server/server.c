#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Server 为每一个连接的 Client 启动的线程
 *
 * @param cfd 为客户端开启的文件描述符
 */
void threaded_server(void *cfd);

int main() {
    // 创建 socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 绑定
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;   // 0.0.0.0
    saddr.sin_port        = htons(10000); // port = 10000
    if (bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        perror("bind");
        exit(-1);
    }

    // 监听
    if (listen(lfd, 8) == -1) {
        perror("listen");
        exit(-1);
    }

    struct sockaddr_in clientaddr;
    int                len;
    int                cfd;
    while (1) {
        printf("waiting for connection\n");
        pthread_t tid;
        len = sizeof(clientaddr);

        cfd =
            accept(lfd, (struct sockaddr *)(&clientaddr), (socklen_t *)(&len));
        if (cfd == -1) {
            perror("accept");
            continue; // 不影响其他客户端的连接
        }

        printf("A new connection occurs!\n");
        if (pthread_create(&tid, NULL, (void *)(&threaded_server),
                           (void *)(&cfd)) == -1) {
            perror("pthread");
            break;
        }
    }

    // 关闭文件描述符
    shutdown(lfd, SHUT_WR);

    return 0;
}

void threaded_server(void *cfd) {
    int         fd              = *((int *)cfd);
    const int   max_len         = 1024;
    const char *server_response = "your message is received\n";
    char        recv_buf[max_len];

    while (1) {
        printf("waiting for client message\n");
        memset(recv_buf, 0, max_len);

        int num = read(fd, recv_buf, max_len);
        if (num == 0) {
            printf("client may closed\n");
            break;
        } else if (num == -1) {
            perror("read");
            break;
        }
        printf("read from client : %s\n", recv_buf);

        // 给客户端发送数据
        if (write(fd, server_response, strlen(server_response)) == -1) {
            break;
        }
    }

    close(fd);
}
