#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define SERV_PORT 8888
#define MAXLINE 1024
#define FD_SIZE 10


void handleError(int error) {
    if (error != 0) {
        perror("error :");
        exit(-1);
    }
}

int main() {

    // 保存server_fd
    int server_fd, client_fd;
    // 保存错误
    int error;
    // 保存网络地址
    struct sockaddr_in server_addr, client_addr;
    // 保存配置项
    int optval = 1;
    // epoll fd
    int epfd;
    //
    struct epoll_event ev, ep[FD_SIZE];

    char str[1024];

    socklen_t client_addr_len;

    int socketFd;

    int i;


    int n;

    char buffer[BUFSIZ];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        printf("socket create error!\n");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    error = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    handleError(error);
    error = listen(server_fd, 400);
    handleError(error);

    epfd = epoll_create(FD_SIZE);


    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    error = epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    handleError(error);


    while (true) {
        int nReady = epoll_wait(epfd, ep, FD_SIZE, -1);

        for (i = 0; i < nReady; i++) {
            if (!(ep[i].events & EPOLLIN)) {
                continue;
            }
            if (ep[i].data.fd == server_fd) {
                client_addr_len = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
                printf("当前客户端:%s:%d连接:\n", inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
                       ntohs(client_addr.sin_port));
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                break;
            } else {
                socketFd = ep[i].data.fd;
                n = read(socketFd, buffer, sizeof(buffer));
                if (n == 0) {
                    printf("当前客户端断开了连接\n");
                    close(client_fd);
                } else if (n < 0) {

                } else {
                    write(STDOUT_FILENO, buffer, n);
                    char *response = (char *)malloc(BUFSIZ);
//                    sprintf(response, "%s%s","服务端发来消息:\n", buffer);
                    char header[] = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n";
                    char html[] = "<!DOCTYPE html><html><head></head><body><h1>这他妈是老严</h1></body></html>";
                    send(socketFd, header, strlen(header), 0);
                    send(socketFd, html,strlen(html), 0);
                    close(socketFd);
                }

            }
        }
//
    }


}

