#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define ETX 0x03  // End-of-text marker

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
            return strdup(line);
        }
    }
    fclose(file);
    return strdup("Address not found");
}

// Function to lookup address by name
char *lookup_address(const char *name)
{
    static char line[BUFFER_SIZE];
    FILE *file = fopen("directory.txt", "r");
    if (!file)
    {
        perror("Error opening file");
        return strdup("Error: Unable to open directory file");
    }

    int addr;
    char stored_name[BUFFER_SIZE];
    while (fscanf(file, "%d %[^\n]", &addr, stored_name) != EOF)
    {
        if (strcmp(stored_name, name) == 0)
        {
            fclose(file);
            snprintf(line, BUFFER_SIZE, "%d", addr);
            return strdup(line);
        }
    }
    fclose(file);
    return strdup("Name not found");
}

// Handle client request
void handle_client(int client_socket)
{
    unsigned char request_type;
    if (read(client_socket, &request_type, 1) <= 0)
    {
        perror("Read failed");
        close(client_socket);
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    if (read(client_socket, buffer, BUFFER_SIZE) <= 0)
    {
        perror("Read failed");
        close(client_socket);
        return;
    }

    // Ensure request ends with ETX
    if (buffer[strlen(buffer) - 1] != ETX)
    {
        printf("Invalid request format\n");
        close(client_socket);
        return;
    }
    buffer[strlen(buffer) - 1] = '\0';  // Remove ETX

    char *response;
    if (request_type == 0x01)  // Lookup by address
    {
        int address = atoi(buffer);
        response = lookup_name(address);
    }
    else if (request_type == 0x09)  // Lookup by name
    {
        response = lookup_address(buffer);
    }
    else
    {
        response = strdup("Invalid request type");
    }

    // Send response with ETX
    write(client_socket, response, strlen(response));
    write(client_socket, "\x03", 1);

    free(response);
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