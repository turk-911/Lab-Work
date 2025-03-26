#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    int address;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Connect to localhost (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter address to lookup: ");
    scanf("%d", &address);
    address = htonl(address);

    if (write(sock, &address, sizeof(address)) <= 0)
    {
        perror("Write failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (read(sock, buffer, BUFFER_SIZE) <= 0)
    {
        perror("Read failed");
    }
    else
    {
        printf("Response: %s\n", buffer);
    }

    close(sock);
    return 0;
}