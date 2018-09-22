#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// for htons and friends
#include <arpa/inet.h>

#include "protocol.h"

int main(int argc, char* argv[]) {
    int tcp_sock;
    struct sockaddr_in addr;
    printf("Welcome to Yggdrasil!\n");

    // create socket
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        perror("socket");
        exit(1);
    }

    // configure server address information
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);
    if (1 != inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr)) {
        perror("Invalid IP");
        exit(1);
    }

    // connect to server
    if (0 != connect(tcp_sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in))) {
        perror("Failed to connect to server");
        exit(1);
    }
    printf("Connection successful\n");

    // communicate
    size_t num_read;
    FILE *sockfile = fdopen(tcp_sock, "rw");
    char *data = read_line(sockfile, &num_read);
    printf("%s\n", data);
    arbor_msg_t message;
    if (parse_arbor_message(data, &message)) {
        printf("Type: %d\n", message.type);
    } else {
        printf("Message failed to parse\n");
    }
}
