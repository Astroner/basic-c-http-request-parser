#include "main.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum ParsingStage {
    ParsingStageMethod,
    ParsingStagePath,
} ParsingStage;

#define CHAR_SPACE 32
#define CHAR_NL 10
#define CHAR_G 71
#define CHAR_D 68
#define CHAR_P 80
#define CHAR_O 79
#define CHAR_U 85
#define CHAR_H 72
#define CHAR_T 84
#define CHAR_A 65

/**
 * @brief basic HTTP request parser
 * 
 * @param socket socket_fd
 * @param buffer buffer where the function will write data
 * @param bufferSize 
 * @param result parser result
 * @return int 0 or -1
 * 
 * @details
 * This is simple GET HTTP request: 
 * GET / HTTP/1.1
 * Host: localhost:2000
 * Accept: text/html,application/xhtml+xml,application/xml
 * Accept-Encoding: gzip, deflate, br
 * 
 * The function only reads Method and path(in this case GET and "/")
 */
int Request_parseSocket(int socket, char* buffer, size_t bufferSize, Request* result) {
    size_t dataLength = read(socket, buffer, bufferSize); // read data into buffer

    result->buffer = buffer;
    result->bufferSize = bufferSize;
    result->dataLength = dataLength;

    result->pathIndexes.start = 0;

    ParsingStage stage = ParsingStageMethod;
    
    int gotMethodFromFirstChar = 0;

    for(size_t i = 0; i < dataLength; i++) {
        // Method is at the beginning of the request and we only accept 5 methods: GET, PUT, POST, PATCH, DELETE
        // So we can use tree search to identify the method
        if(i == 0) {// first character of the request
            if(buffer[0] == CHAR_G) { // If "G" then it is GET request
                result->method = RequestMethodGet;
                gotMethodFromFirstChar = 1;
                continue;
            } else if(buffer[0] == CHAR_D) { // If "D" then it is DELETE request
                result->method = RequestMethodDelete;
                gotMethodFromFirstChar = 1;
                continue;
            } else if(buffer[0] == CHAR_P) { // If "P" then it need further processing
                continue;
            } else {
                return -1;
            }
        } else if(i == 1) { // second character
            if(gotMethodFromFirstChar) { // if we got the method from the first character then just skip 
                continue;  
            } else if(buffer[1] == CHAR_O) { // If "O" then it is POST request
                result->method = RequestMethodPost;
                continue;
            } else if(buffer[1] == CHAR_U) { // If "U" then it is PUT request
                result->method = RequestMethodPut;
                continue;
            } else if(buffer[1] == CHAR_A) { // If "O" then it is PATCH request
                result->method = RequestMethodPatch;
                continue;
            } else {
                return -1;
            }
        } else if(buffer[i] == CHAR_SPACE) { // Space divides parts of the first line
            if(stage == ParsingStageMethod) {
                result->pathIndexes.start = i + 1; // Set start index of the path
                stage = ParsingStagePath;
            } else if(stage == ParsingStagePath) {
                result->pathIndexes.end = i - 1; // Set end index of the path
                return 0;
            } else {
                return -1;
            }
        };
    }
    
    return -1;
}

char* Request_getPath(Request* req) {
    char* str = malloc(req->pathIndexes.end - req->pathIndexes.start + 2);

    for(size_t i = req->pathIndexes.start; i <= req->pathIndexes.end; i++) {
        str[i - req->pathIndexes.start] = req->buffer[i];
    }

    str[req->pathIndexes.end + 1] = '\0';

    return str;
}