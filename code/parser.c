#include "parser.h"
#include <string.h>

int parse_http_request(char *buffer, HttpRequest *req) {
    char *line_saveptr;
    char *first_line = strtok_r(buffer, "\r\n", &line_saveptr);
    
    if (!first_line) return -1;

    char *word_saveptr;
    req->method = strtok_r(first_line, " ", &word_saveptr);
    req->path = strtok_r(NULL, " ", &word_saveptr);
    req->version = strtok_r(NULL, " ", &word_saveptr);

    if (!req->method || !req->path || !req->version) {
        return -1;
    }

    return 0;
}