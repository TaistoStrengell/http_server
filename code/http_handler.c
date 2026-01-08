#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <string.h>

#include "http_handler.h"
#include "parser.h"

const char *ALLOWED_FILES[] = {
    "/test_file",
    NULL
};

void handle_http_request(void *arg) {
    int *socket_ptr = (int *)arg;
    int client_fd = *socket_ptr;
    
    char buffer[2048];
    HttpRequest req;

    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        close(client_fd);
        return;
    }
    buffer[bytes_read] = '\0';

    if (parse_http_request(buffer, &req) != 0) {
        close(client_fd);
        return;
    }

    int allowed = 0;
    for (int i = 0; ALLOWED_FILES[i] != NULL; i++) {
        if (strcmp(req.path, ALLOWED_FILES[i]) == 0) {
            allowed = 1;
            break;
        }
    }

    if (!allowed) {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        send(client_fd, not_found, strlen(not_found), 0);
        close(client_fd);
        return;
    }

    const char *file_to_open = req.path + 1; 
    struct stat st;

    if (stat(file_to_open, &st) != 0) {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        send(client_fd, not_found, strlen(not_found), 0);
        close(client_fd);
        return;
    }

    int fd = open(file_to_open, O_RDONLY);
    if (fd < 0) {
        close(client_fd);
        return;
    }

    char http_header[512];
    int header_length = snprintf(http_header, sizeof(http_header),
                                 "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: application/octet-stream\r\n"
                                 "Content-Length: %ld\r\n"
                                 "Connection: close\r\n"
                                 "\r\n", (long)st.st_size);

    if (header_length > 0 && (size_t)header_length < sizeof(http_header)) {
        send(client_fd, http_header, header_length, 0);
        sendfile(client_fd, fd, NULL, st.st_size);
    }

    close(fd);
    close(client_fd);
}