#include <sys/socket.h>

void main(){
    int sock_ipv4 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ipv4 < 0) { perror("IPv4 socket fail"); return 1; }

    
    setsockopt(sock_ipv4, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int));
    
    int sock_ipv6 = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock_ipv6 < 0) { perror("IPv6 socket fail"); return 1; }

    setsockopt(sock_ipv6, SOL_SOCKET, SO_REUSEADDR, 1, sizeof(int));
}