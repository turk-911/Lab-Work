// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    struct servent *service;

    service = getservbyname("filexfer", "tcp");
    if (!service) {
        fprintf(stderr, "ERROR: 'filexfer' service not found in /etc/services\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = service->s_port;

    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);
    printf("📡 Server listening on port %d (filexfer)...\n", ntohs(service->s_port));
    
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);

        // Print hostname of requester
        struct hostent *he = gethostbyaddr(&cli_addr.sin_addr, sizeof(cli_addr.sin_addr), AF_INET);
        if (he)
            printf("📥 Request received from %s\n", he->h_name);
        else
            printf("📥 Request received from %s\n", inet_ntoa(cli_addr.sin_addr));

        char filename[MAX];
        bzero(filename, MAX);
        read(newsockfd, filename, MAX);
        filename[strcspn(filename, "\n")] = '\0';

        int fd = open(filename, O_RDONLY);
        if (fd < 0) {
            write(newsockfd, "File not found", 15);
            close(newsockfd);
            continue;
        }

        char buffer[MAX];
        int n;
        while ((n = read(fd, buffer, MAX)) > 0) {
            write(newsockfd, buffer, n);
        }

        close(fd);
        close(newsockfd);
        printf("📤 File '%s' sent successfully.\n", filename);
    }

    close(sockfd);
    return 0;
}