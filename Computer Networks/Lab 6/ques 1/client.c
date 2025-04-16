// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct servent *service;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <filename>\n", argv[0]);
        exit(1);
    }

    // Get server info
    server = gethostbyname(argv[1]);
    if (!server) {
        fprintf(stderr, "ERROR: No such host\n");
        exit(1);
    }

    // Get service info from /etc/services
    service = getservbyname("filetransfer", "tcp");
    if (!service) {
        fprintf(stderr, "ERROR: 'filexfer' service not found in /etc/services\n");
        exit(1);
    }

    char *filename = argv[2];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = service->s_port;  // Automatically from /etc/services

    connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    write(sockfd, filename, strlen(filename));

    int outfd = creat("receive.txt", 0666);
    char buffer[MAX];
    int n;
    while ((n = read(sockfd, buffer, MAX)) > 0) {
        write(outfd, buffer, n);
    }

    printf("✅ File received successfully as 'receive.txt'\n");
    close(outfd);
    close(sockfd);
    return 0;
}