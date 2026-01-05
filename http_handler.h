#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <netinet/in.h> // struct sockaddr_in6

typedef struct {
    int fd;
    struct sockaddr_in6 addr;
} conn_info_t;

void handle_http_request(void *arg);

#endif

