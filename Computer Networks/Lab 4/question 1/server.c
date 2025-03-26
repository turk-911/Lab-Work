#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to lookup name by address
char *lookup_name(int address)
{
    static char line[BUFFER_SIZE];
    FILE *file = fopen("directory.txt", "r");
    if (!file)
    {
        perror("Error opening file");
        return strdup("Error: Unable to open directory file");
    }

    int addr;
    char name[BUFFER_SIZE];
    while (fscanf(file, "%d %[^\n]", &addr, name) != EOF)
    {
        if (addr == address)
        {
            fclose(file);
            snprintf(line, BUFFER_SIZE, "%s", name);
            return strdup(line);  // Allocate memory for the response
        }
    }
    fclose(file);
    return strdup("Address not found");  // Ensure memory allocation
}

void handle_client(int client_socket)
{
    int address;
    if (read(client_socket, &address, sizeof(address)) <= 0)
    {
        perror("Read failed");
        close(client_socket);
        return;
    }

    address = ntohl(address);  // Convert from network to host byte order
    printf("Received request for address: %d\n", address);

    char *response = lookup_name(address);
    if (write(client_socket, response, strlen(response) + 1) <= 0)
    {
        perror("Write failed");
    }

    free(response);  // Free allocated memory
    close(client_socket);
}

int main()
{
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);
    while (1)
    {
        client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }
        handle_client(client_socket);
    }

    close(server_fd);
    return 0;
}