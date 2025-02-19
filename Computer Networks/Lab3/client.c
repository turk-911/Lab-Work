#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server! Type messages and press Enter to send.\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds); 
        FD_SET(STDIN_FILENO, &readfds);  
        int max_sd = sockfd;

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (bytes_received == 0) {
                printf("Disconnected from server.\n");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            printf("\n%s\n", buffer);
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0; 
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }

    return 0;
}