#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Connected to client: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

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
        fprintf(stderr, "Usage: %s <service_name>\n", argv[0]);
        exit(1);
    }

    struct servent *service = getservbyname(argv[1], "tcp");
    if (service == NULL) {
        perror("getservbyname failed");
        exit(1);
    }

    int port = ntohs(service->s_port);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    listen(server_socket, 5);
    printf("Server listening on service '%s' (port %d)...\n", argv[1], port);

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket, client_addr);
        } else {
            close(client_socket);
        }
    }

    return 0;
}