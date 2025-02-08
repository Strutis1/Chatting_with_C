#include <winsock2.h> 
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "Client.h"

#pragma comment(lib, "ws2_32.lib") 

void initialize_winsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Winsock initialization failed! Error Code: %d\n", WSAGetLastError());
        exit(1);
    }
}


DWORD WINAPI receiveMessages(void* socket) {
    int sock = *(int*)socket;
    char buffer[255];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        printf("[Server]: %s\n", buffer);
    }

    printf("Server disconnected.\n");
    closesocket(sock);
    return 0;
}


int main(int argc, char const* argv[]) 
{ 
    if (argc != 3){
        printf("Usage: %s <Port>, <server_ip>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);  
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Please enter a port between 1 and 65535.\n");
        return 1;
    }


    initialize_winsock();
	int sockD = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockD == INVALID_SOCKET) {
        printf("Socket creation failed! Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

	struct sockaddr_in servAddr; 

	servAddr.sin_family = AF_INET; 
	servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(argv[2]);

	int connectStatus = connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr)); 

	if (connectStatus == -1) { 
		printf("Error...\n"); 
	} 

	else { 
		char strData[255]; 

		recv(sockD, strData, sizeof(strData), 0); 

		printf("Message: %s\n", strData); 
	} 

   HANDLE thread = CreateThread(NULL, 0, receiveMessages, &sockD, 0, NULL);

    char buffer[255];
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        send(sockD, buffer, strlen(buffer), 0);
    }

    closesocket(sockD);
    WSACleanup();

	return 0; 
}
