#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 9801

void catchError(int error) {
    if (error != 0) {
        perror("error:");
        exit(-1);
    }
}

int main () {
    // 用来保存错误信息
    int error = 0;

    // 创建基于ipv4的TCPsocket;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 1) {
        puts("create socket error");
        exit(-1);
    }

    struct sockaddr_in serverAddress, clientAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_HOST);


}