#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 9802

void catchError(int error) {
    if (error != 0) {
        perror("error:");
        exit(-1);
    }
}

int main() {
    // 创建结构体
    struct sockaddr_in serverAddress, clientAddress;
    // 用来保存错误信息
    int error = 0;
    // 用来保存读取的字节数
    int n = 0;
    // 保存服务端socket
    int serverSocket = 0;
    // 保存客户端socket
    int clientSocket = 0;

    socklen_t clientAddressLength;

    char buffer[BUFSIZ];

    bzero(&serverAddress, sizeof(serverAddress));
    bzero(&clientAddress, sizeof(clientAddress));
    bzero(buffer, sizeof(buffer));

    // TCP ipv4 socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 1) {
        puts("create socket error");
        exit(-1);
    }


    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_HOST);

    // 绑定IP跟端口
    error = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    catchError(error);

    error = listen(serverSocket, 64);
    catchError(error);

    clientAddressLength = sizeof(clientAddress);

    clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressLength);
    catchError(error);


    while (1) {
        memset(buffer, 0, sizeof(BUFSIZ));
        n = read(clientSocket, &buffer, sizeof(buffer));
        write(clientSocket, buffer, sizeof(buffer));
    }
    close(clientSocket);
    close(serverSocket);


}