#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#define SERV_PORT 888
#define MAXLINE 1024

void child_process_exit(int signo) {
    while (waitpid(0, NULL, WNOHANG) > 0) {
        return;
    }
}

void catchError(int error) {
    if (error != 0) {
        perror("error:");
    }
}
struct s_info {
    struct sockaddr_in client_addr;
    int client_fd;
};

void * swoole_callback(void *arg) {
    int n, i;
    struct s_info *ts = (struct s_info *)arg;

    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];

    while (1) {
        buf[0] = 0;
        n = read(ts->client_fd,buf,MAXLINE);
        if (n == 0) {
            printf("client %d is closed\n", ts->client_fd);
            break;
        }
        write(ts->client_fd, buf, n);
        write(STDOUT_FILENO, buf, sizeof(buf));
    }

    return (void *)0;
}





int main (void) {

    struct s_info *s;

    int server_fd, client_fd;

    int error = 0;

    int n = 0;

    int i = 0;

    int pid;

    char buffer[BUFSIZ];

    char client_ip[BUFSIZ];

    struct sockaddr_in serv_addr, client_addr;

    struct s_info ts[256];

    pthread_t tid;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    error = bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    catchError(error);

    error = listen(server_fd, 256);
    catchError(error);

    while(1) {

        socklen_t client_socket_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_socket_len);
        printf("当前客户端IP:%s, 客户端端口号:%d\n",
               inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)),
               ntohs(client_addr.sin_port));

        if (client_fd < 0) {
            perror("socket Error");
        }

        ts[i].client_fd = client_fd;
        ts[i].client_addr = client_addr;

        error = pthread_create(&tid, NULL, swoole_callback, (void *) &ts[i]);
        catchError(error);
        pthread_detach(tid);
        i++;
    }
}



