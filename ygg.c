#include <stdio.h>
#include <types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[]) {
    int tcp_sock;
    struct sockaddr_in addr;
    printf("Welcome to Yggdrasil!\n");
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        perror("socket");
    }
    addr.sin_port = htons(7777);
    addr.sin_addr.s_addr = htons(7777);
}
