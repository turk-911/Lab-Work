#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT1 9999
#define PORT2 8081
#define BUFFER_SIZE 1
int main() {

    int sock;
    struct sockaddr_in server_addr1, server_addr2, client1_addr, client2_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr1, 0, sizeof(server_addr1));

    server_addr1.sin_family = AF_INET;
    server_addr1.sin_addr.s_addr = INADDR_ANY;
    server_addr1.sin_port = htons(PORT1);

    memset(&server_addr2, 0, sizeof(server_addr2));

    server_addr2.sin_family = AF_INET;
    server_addr2.sin_addr.s_addr = INADDR_ANY;
    server_addr2.sin_port = htons(PORT2);

    if (bind(sock, (const struct sockaddr*)&server_addr1, sizeof(server_addr1)) < 0) {
        perror("Bind failed for Client1");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Server ready. Waiting for Client1...\n");
    recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client1_addr, &addr_len);

    printf("Received character from Client1: %c\n", buffer[0]);
    buffer[0]--;

    sendto(sock, buffer, BUFFER_SIZE, 0, (const struct sockaddr*)&server_addr2, sizeof(server_addr2));
    printf("Sent modified character to Client2: %c\n", buffer[0]);
    close(sock);

    return 0;
}