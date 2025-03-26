#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUF_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    if (argc < 4) {
        fprintf(stderr, "Usage: %s [server_address] [port_number] [file_name.txt]\n", argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Set up server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
        error("ERROR invalid server address");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // Send the file name request to the server
    if (write(sockfd, argv[3], strlen(argv[3])) < 0)
        error("ERROR writing to socket");

    // Create the file "receive.txt" to store the received data
    int fd = creat("receive.txt", 0666);
    if (fd < 0)
        error("ERROR creating file receive.txt");

    // Read data from the server and write it to the file
    int n;
    while ((n = read(sockfd, buffer, BUF_SIZE)) > 0) {
        if (write(fd, buffer, n) != n)
            error("ERROR writing to file");
    }
    if (n < 0)
        error("ERROR reading from socket");

    // Close file and socket
    close(fd);
    close(sockfd);
    return 0;
}
