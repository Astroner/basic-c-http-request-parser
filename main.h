#if !defined(MAIN_H)
#define MAIN_H

#include <stdlib.h>

typedef enum RequestMethod {
    RequestMethodGet,
    RequestMethodPost,
    RequestMethodPut,
    RequestMethodPatch,
    RequestMethodDelete,
} RequestMethod;

typedef struct Request {
    struct {
        size_t start;
        size_t end;
    } pathIndexes;
    RequestMethod method;
    char* buffer;
    size_t bufferSize;
    size_t dataLength;
} Request;

int Request_parseSocket(int socket, char* buffer, size_t bufferSize, Request* result);

char* Request_getPath(Request* request);

#endif // MAIN_H
