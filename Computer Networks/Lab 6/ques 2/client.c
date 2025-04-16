// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host_name> <port_number> <file_name.txt>\n", argv[0]);
        exit(1);
    }

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *file_name = argv[3];

    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "No such host\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    write(sockfd, file_name, strlen(file_name));

    int fd = creat("receive.txt", 0666);
    if (fd < 0) {
        perror("Could not create receive.txt");
        close(sockfd);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(sockfd, buffer, BUFFER_SIZE)) > 0) {
        write(fd, buffer, bytes_read);
    }

    printf("File received and saved as receive.txt\n");

    close(fd);
    close(sockfd);
    return 0;
}