#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "http_handler.h"

void serve_file(int client_fd, const char *path) {
    // Lisätään polun eteen juurihakemisto (esim. ./www) tietoturvan takia
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "./www%s", path);

    // Tarkistetaan tiedoston tiedot (onko olemassa, koko)
    struct stat st;
    if (stat(full_path, &st) < 0 || !S_ISREG(st.st_mode)) {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        send(client_fd, not_found, strlen(not_found), 0);
        return;
    }

    int file_fd = open(full_path, O_RDONLY);
    if (file_fd < 0) {
        // 500 Internal Server Error jos tiedostoa ei saada auki
        return; 
    }

    // Lähetetään HTTP-headerit
    char header[256];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %lld\r\n"
        "Connection: close\r\n\r\n", 
        (long long)st.st_size);
    send(client_fd, header, header_len, 0);

    // Lähetetään tiedoston sisältö puskuroidusti
    char file_buffer[4096];
    ssize_t n;
    while ((n = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
        send(client_fd, file_buffer, n, 0);
    }

    close(file_fd);
}