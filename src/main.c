#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#define SERV_PORT 8888

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

int main (void) {
    int server_fd, client_fd;

    int error = 0;

    int n = 0;

    int i = 0;

    int pid;

    char buffer[BUFSIZ];

    struct sockaddr_in serv_addr, client_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    error = bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    catchError(error);

    error = listen(server_fd, 3);
    catchError(error);

    while(1) {

        socklen_t client_socket_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_socket_len);

        if (client_fd < 0) {
            perror("socket Error");
        }


        pid = fork();

        if (pid < 0) {
            perror("fork error!");
            exit(1);
        } else if(pid == 0) {
            close(server_fd);
            while (1) {
                n = read(client_fd, buffer, sizeof(buffer));
                if (n == 0) {
                    close(client_fd);
                    return 0;
                } else if(n == -1) {
                    perror("read error");
                    exit(1);
                }  else {
                    for(i = 0; i < n; i++) {
                        buffer[i] = toupper(buffer[i]);
                    }
                    write(client_fd, buffer, n);
                }

            }
        } else {
            close(client_fd);
            signal(SIGCHLD, child_process_exit);
        }
    }
}



