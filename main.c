#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "main.h"

#define REQUEST_BUFFER_LENGTH 2048

#define BASIC_RESPONSE "HTTP/1.1 200 OK\nContent-Type: text/plain\n"
#define BASIC_RESPONSE_LENGTH 41

int main(void) {
    int mainSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if(mainSocket < 0) return 1;

    struct sockaddr_in sockAddr = {
        .sin_family = AF_INET,
        .sin_addr = {
            .s_addr = htonl(INADDR_ANY),
        },
        .sin_port = htons(2000),
    };

    if(bind(mainSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) return 2;

    if(listen(mainSocket, 10) < 0) return 3;

    int incomingSocket;
    struct sockaddr_in incomingAddress;
    socklen_t incomingLength;

    Request request;
    char requestBuffer[REQUEST_BUFFER_LENGTH];

    while(1) {
        if((incomingSocket = accept(mainSocket, (struct sockaddr*)&incomingAddress, &incomingLength)) < 0) return 4;
        
        printf("# New Request\n");

        Request_parseSocket(incomingSocket, requestBuffer, REQUEST_BUFFER_LENGTH, &request);

        char* path = Request_getPath(&request);
        printf("## Path: %s\n", path);

        char* methodString = 
            request.method == RequestMethodGet 
            ? "GET"
            : request.method == RequestMethodPost
            ? "POST"
            : request.method == RequestMethodPut
            ? "PUT"
            : request.method == RequestMethodPatch
            ? "PATCH"
            : "DELETE";
        printf("## Method: %s\n", methodString);

        write(incomingSocket, BASIC_RESPONSE, BASIC_RESPONSE_LENGTH);
        write(incomingSocket, "Content-Length: ", 16);
        
        char responseDataLengthBuffer[4];
        snprintf(responseDataLengthBuffer, 4, "%zu", strlen(methodString) + 1 + strlen(path));

        write(incomingSocket, responseDataLengthBuffer, strlen(responseDataLengthBuffer));
        write(incomingSocket, "\n\n", 2);
        write(incomingSocket, methodString, strlen(methodString));
        write(incomingSocket, " ", 1);
        write(incomingSocket, path, strlen(path));

        free(path);
        memset(requestBuffer, 0, REQUEST_BUFFER_LENGTH);
        close(incomingSocket);

        printf("\n");
    }

    return 0;
}