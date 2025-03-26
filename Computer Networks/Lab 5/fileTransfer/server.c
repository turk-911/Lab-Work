#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[BUF_SIZE];

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Initialize server address structure
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the given port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // Listen for incoming connections
    listen(sockfd, 5);
    printf("Server listening on port %d...\n", portno);

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // Read the requested file name from the client
    memset(buffer, 0, BUF_SIZE);
    if (read(newsockfd, buffer, BUF_SIZE-1) < 0)
        error("ERROR reading from socket");
    printf("Client requested file: %s\n", buffer);

    // Open the requested file in read-only mode
    int fd = open(buffer, O_RDONLY);
    if (fd < 0) {
        // If file cannot be opened, send an error message back to the client
        char *err_msg = "Error: File not found or cannot be opened.\n";
        write(newsockfd, err_msg, strlen(err_msg));
        close(newsockfd);
        close(sockfd);
        exit(1);
    }

    // Read from file and send its content to the client
    int n;
    while ((n = read(fd, buffer, BUF_SIZE)) > 0) {
        if (write(newsockfd, buffer, n) != n)
            error("ERROR writing to socket");
    }
    if (n < 0)
        error("ERROR reading from file");

    // Close file descriptors and sockets
    close(fd);
    close(newsockfd);
    close(sockfd);
    return 0;
}
