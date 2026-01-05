#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/socket.h>
#include <sys/sendfile.h> 
#include <errno.h>

#include "http_handler.h"

void handle_http_request(void *arg) {
    conn_info_t *conn = (conn_info_t *)arg;
    int client_fd = conn->fd;
    const char *filename = "test_file";
    struct stat st;

    if (stat(filename, &st) != 0) {
        perror("Failed to get file status");
        close(client_fd);
        return;
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        close(client_fd);
        return;
    }

    char http_header[256];
    int header_length = snprintf(http_header, sizeof(http_header),
                                 "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: application/octet-stream\r\n"
                                 "Content-Length: %ld\r\n"
                                 "Connection: close\r\n"
                                 "\r\n", (long)st.st_size);

    if (header_length < 0 || (size_t)header_length >= sizeof(http_header)) {
        fprintf(stderr, "Header too large or error\n");
        close(fd);
        close(client_fd);
        return;
    }

    if (send(client_fd, http_header, header_length, 0) < 0) {
        perror("Failed to send HTTP header");
        close(fd);
        close(client_fd);
        return;
    }

    off_t offset = 0;
    ssize_t sent_bytes = sendfile(client_fd, fd, &offset, st.st_size);
    
    if (sent_bytes < 0) {
        perror("sendfile failed");
    }

    close(fd);
    close(client_fd);
}