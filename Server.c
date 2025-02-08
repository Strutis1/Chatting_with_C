#include <winsock2.h> 
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "Server.h"


#pragma comment(lib, "ws2_32.lib") 


void initialize_winsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Winsock initialization failed! Error Code: %d\n", WSAGetLastError());
        exit(1);
    }
}

DWORD WINAPI receiveMessages(void* socket) {
    int clientSocket = *(int*)socket;
    char buffer[255];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        printf("[Client]: %s\n", buffer);
    }

    printf("Client disconnected.\n");
    closesocket(clientSocket);
    return 0;
}

int main(int argc, char const* argv[]) 
{ 
     if (argc != 2){
        printf("Usage: %s <Port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);  
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please enter a port between 1 and 65535.\n");
        return 1;
    }
    initialize_winsock();
    int servSockD = socket(AF_INET, SOCK_STREAM, 0); 
  
    char serMsg[255] = "Message from the server to the client \'Hello Client\' "; 

  
    struct sockaddr_in servAddr; 
  
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(port); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("Bind failed! Error: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

  
    if (listen(servSockD, 1) == SOCKET_ERROR) {
        printf("Listen failed! Error: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    printf("Waiting for a client to connect on port %d...\n", port);

  
    int clientSocket = accept(servSockD, NULL, NULL); 
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed! Error: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    printf("Client connected! Sending message...\n");
  
    send(clientSocket, serMsg, strlen(serMsg) + 1, 0);
    char receivedMes[255];
    HANDLE thread = CreateThread(NULL, 0, receiveMessages, &clientSocket, 0, NULL);

    char buffer[255];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; 
        send(clientSocket, buffer, strlen(buffer), 0);
    }
  
    closesocket(clientSocket);
    closesocket(servSockD);
    WSACleanup();


    return 0; 
}