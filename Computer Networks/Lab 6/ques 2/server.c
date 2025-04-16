// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char filename[100];
    int n = read(client_socket, filename, sizeof(filename));
    if (n <= 0) {
        perror("Failed to read filename");
        close(client_socket);
        exit(1);
    }

    filename[n] = '\0';

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("File open failed");
        write(client_socket, "ERROR", 5);
        close(client_socket);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        write(client_socket, buffer, bytes_read);
    }

    close(fd);
    close(client_socket);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    listen(server_socket, 5);
    printf("Server listening on port %d...\n", port);

    while (1) {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
        } else {
            close(client_socket);
        }
    }

    return 0;
}