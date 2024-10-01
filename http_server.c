#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "gethandler.h"
#include "posthandler.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 0
#define MAX_FILENAME_LENGTH 256
#define MAX_REQUEST_SIZE 1024 

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
	char buff[30];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        return 1;
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
	}
	if (getsockname(server_socket, (struct sockaddr *)&address, &addrlen) == SOCKET_ERROR){		
		printf("Port = ?");
	}
	printf("http://localhost:%d\n", ntohs(address.sin_port));
	sprintf(buff,"start http://localhost:%d", ntohs(address.sin_port));
	system(buff);
	while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            return 1;
        }
        char request[MAX_REQUEST_SIZE+1];
        int request_length = recv(client_socket, request, MAX_REQUEST_SIZE, 0);
        request[request_length] = '\0';

        if (strncmp(request, "GET ", 4) == 0) {
            handle_get(client_socket, request);
        } else if (strncmp(request, "POST ", 5) == 0) {
            handle_post(client_socket, request);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
