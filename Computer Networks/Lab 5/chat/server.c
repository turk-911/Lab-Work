#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345
#define BUF_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    char buffer[BUF_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Initialize socket structure
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Start listening for the clients
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", PORT);
    
    clilen = sizeof(cli_addr);
    // Accept actual connection from the client
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    printf("Connected to client.\n");

    // Fork to create two processes: one for sending and one for receiving
    pid_t pid = fork();
    if (pid < 0) {
        error("ERROR on fork");
    }
    else if (pid == 0) {
        // Child process: Responsible for sending messages
        while (1) {
            memset(buffer, 0, BUF_SIZE);
            // Read message from standard input
            if (fgets(buffer, BUF_SIZE - 1, stdin) == NULL) {
                break;
            }
            // Send message to the client
            if (write(newsockfd, buffer, strlen(buffer)) < 0) {
                error("ERROR writing to socket");
            }
        }
    }
    else {
        // Parent process: Responsible for receiving messages
        while (1) {
            memset(buffer, 0, BUF_SIZE);
            int n = read(newsockfd, buffer, BUF_SIZE - 1);
            if (n < 0) {
                error("ERROR reading from socket");
            }
            if (n == 0) {
                printf("Client disconnected.\n");
                break;
            }
            printf("Client: %s", buffer);
        }
    }
    
    close(newsockfd);
    close(sockfd);
    return 0;
}
