#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define ETX 0x03  // End-of-text marker

void send_request(int sock, unsigned char request_type, const char *query)
{
    char buffer[BUFFER_SIZE] = {0};
    buffer[0] = request_type;
    strcpy(buffer + 1, query);
    buffer[strlen(query) + 1] = ETX;

    if (write(sock, buffer, strlen(buffer)) <= 0)
    {
        perror("Write failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Receive response
    char response[BUFFER_SIZE] = {0};
    if (read(sock, response, BUFFER_SIZE) <= 0)
    {
        perror("Read failed");
    }
    else
    {
        // Ensure response ends with ETX
        size_t len = strlen(response);
        if (response[len - 1] == ETX)
        {
            response[len - 1] = '\0';  // Remove ETX
        }
        printf("Response: %s\n", response);
    }
}

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char input[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter lookup type (1 for address, 9 for name): ");
    int choice;
    scanf("%d", &choice);
    getchar();  // Consume newline

    if (choice == 1)
    {
        printf("Enter address: ");
        scanf("%s", input);
        send_request(sock, 0x01, input);
    }
    else if (choice == 9)
    {
        printf("Enter name: ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline
        send_request(sock, 0x09, input);
    }
    else
    {
        printf("Invalid choice.\n");
    }

    close(sock);
    return 0;
}