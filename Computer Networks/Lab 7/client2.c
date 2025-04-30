#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE], recvBuffer[BUFFER_SIZE * 4];
    int server_len = sizeof(server);

    if (argc < 3) {
        printf("Usage: %s <server_ip> <command>\n", argv[0]);
        return 1;
    }

    // Combine command line args into one string (supporting spaces in commands)
    strcpy(buffer, argv[2]);
    for (int i = 3; i < argc; i++) {
        strcat(buffer, " ");
        strcat(buffer, argv[i]);
    }

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    // Send command to server
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server, server_len);

    // Receive response from server
    memset(recvBuffer, 0, sizeof(recvBuffer));
    recvfrom(sockfd, recvBuffer, sizeof(recvBuffer), 0, (struct sockaddr *)&server, &server_len);

    printf("Command output from server:\n%s\n", recvBuffer);

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
