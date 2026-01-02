#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H
#include <stdbool.h>

void handle_connection(void *arg);
bool is_path_safe(const char *path);

#endif