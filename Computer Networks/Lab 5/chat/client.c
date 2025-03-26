#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(1);
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Setup server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
        error("ERROR invalid server IP address");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Connected to server %s on port %d.\n", argv[1], PORT);

    // Fork to create two processes: one for sending and one for receiving
    pid_t pid = fork();
    if (pid < 0) {
        error("ERROR on fork");
    }
    else if (pid == 0) {
        // Child process: Sending messages to the server
        while (1) {
            memset(buffer, 0, BUF_SIZE);
            if (fgets(buffer, BUF_SIZE - 1, stdin) == NULL) {
                break;
            }
            if (write(sockfd, buffer, strlen(buffer)) < 0) {
                error("ERROR writing to socket");
            }
        }
    }
    else {
        // Parent process: Receiving messages from the server
        while (1) {
            memset(buffer, 0, BUF_SIZE);
            int n = read(sockfd, buffer, BUF_SIZE - 1);
            if (n < 0) {
                error("ERROR reading from socket");
            }
            if (n == 0) {
                printf("Server disconnected.\n");
                break;
            }
            printf("Server: %s", buffer);
        }
    }

    close(sockfd);
    return 0;
}
