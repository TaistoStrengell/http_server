#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

typedef struct {
    char *method;
    char *path;
    char *version;
} HttpRequest;

int parse_http_request(char *buffer, HttpRequest *req);

#endif