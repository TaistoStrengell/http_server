#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#include "threadpool.h"
#include "http_handler.h"

#define PORT 8080

volatile sig_atomic_t server_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    server_running = 0; 
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGPIPE, SIG_IGN);

    int sockfd;
    struct sockaddr_in6 server_addr;
    int off = 0;

    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("creating a socket failed");
        exit(1);
    }

    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&off, sizeof(off)) < 0) {
        perror("setsockopt IPV6_V6ONLY failed");
        close(sockfd);
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(PORT); 
    server_addr.sin6_addr = in6addr_any;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 10) < 0) {
        perror("listen failed");
        close(sockfd);
        exit(1);
    }
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    threadpool_t *pool = threadpool_create(4, 100);

    while (server_running) {
        struct sockaddr_in6 client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
    
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR){
                continue;
            }
            perror("accept failed");
            continue;
        }

        int conn;
        conn = client_fd;

        if (threadpool_add(pool, handle_http_request, &conn, sizeof(int)) != 0) {
            fprintf(stderr, "Failed to add, queue might be full\n");
            close(client_fd);
        }
    }

    threadpool_destroy(pool);
    close(sockfd);
    return 0;
}