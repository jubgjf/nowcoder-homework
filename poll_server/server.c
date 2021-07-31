#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // 创建 socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    int on = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) <
        0) {
        perror("setsockopt");
        close(lfd);
        exit(-1);
    }

    if (ioctl(lfd, FIONBIO, (char *)&on) < 0) {
        perror("ioctl");
        close(lfd);
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

    struct pollfd fds[200];
    memset(fds, 0, sizeof(fds));
    fds[0].fd     = lfd;
    fds[0].events = POLLIN;

    int timeout = 3 * 60 * 1000; // 超时时间为 3 min

    int end_server = 0;
    int nfds       = 1;

    while (!end_server) {
        printf("waiting for connect\n");

        int ret = poll(fds, nfds, timeout);
        if (ret < 0) {
            perror("poll failed");
            break;
        } else if (ret == 0) {
            printf("poll timed out\n");
            break;
        }

        int compress_array = 0;
        int current_size   = nfds;
        for (int i = 0; i < current_size; i++) {
            if (fds[i].revents == 0) {
                continue;
            } else if (fds[i].revents != POLLIN) {
                printf("ERROR: revents = %d\n", fds[i].revents);
                end_server = 1;
                break;
            }

            if (fds[i].fd == lfd) {
                while (1) {
                    int new_sd = accept(lfd, NULL, NULL);
                    if (new_sd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("accept");
                            end_server = 1;
                        }
                        break;
                    }

                    printf("new connection found\n");
                    fds[nfds].fd     = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                    if (new_sd == -1) {
                        break;
                    }
                }
            } else {
                int  close_conn = 0;
                char buffer[80];

                while (1) {
                    ret = recv(fds[i].fd, buffer, sizeof(buffer), 0);

                    if (ret < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("recv");
                            close_conn = 1;
                        }
                        break;
                    } else if (ret == 0) {
                        printf("connection close\n");
                        close_conn = 1;
                        break;
                    }

                    int len = ret;
                    printf("receive: %s", buffer);

                    if (send(fds[i].fd, buffer, len, 0) < 0) {
                        perror("send");
                        close_conn = 1;
                        break;
                    }

                    memset(buffer, 0, sizeof(buffer));
                }

                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd      = -1;
                    compress_array = 1;
                }
            }
        }

        if (compress_array) {
            compress_array = 0;
            for (int i = 0; i < nfds; i++) {
                if (fds[i].fd == -1) {
                    for (int j = i; j < nfds; j++) {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }
    }

    for (int i = 0; i < nfds; i++) {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }
}
