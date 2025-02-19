#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER_PORT 9999
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1
int main() {

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    printf("Enter a character to send to the server: ");
    scanf(" %c", &buffer[0]);

    sendto(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Sent character to server: %c\n", buffer[0]);

    close(sock);
    return 0;
}