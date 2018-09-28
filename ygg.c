#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// for STDIN_FILENO
#include <unistd.h>

// for poll
#include <poll.h>

// for strlen
#include <string.h>

// for time
#include <time.h>

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

    // set up file monitoring
    struct pollfd fds[2];
    memset(&fds, 0, sizeof(struct pollfd)*2);
    #define INPUT_FD_INDEX 0
    #define SOCKET_FD_INDEX 1
    fds[INPUT_FD_INDEX].fd = STDIN_FILENO;
    fds[INPUT_FD_INDEX].events = POLLIN;
    fds[SOCKET_FD_INDEX].fd = tcp_sock;
    fds[SOCKET_FD_INDEX].events = POLLIN;

    // communicate
    FILE *sockfile = fdopen(tcp_sock, "rw");
    arbor_msg_t message;
    char * last_id = NULL;
    while (poll(fds, 2, -1)) {
        if (fds[SOCKET_FD_INDEX].revents & POLLIN) {
            read_arbor_message(sockfile, &message);
            if (message.type == ARBOR_WELCOME) {
                printf("Type: %d Major: %d Minor: %d Root: %s Recent_Len: %d\n", message.type, message.major, message.minor, message.root, (int) message.recent_len);
                last_id = strdup(message.root);
            } else if (message.type == ARBOR_NEW) {
                printf("[%s]@%d %s: %s", message.uuid, message.timestamp, message.username, message.content);
                if (message.content[strlen(message.content) -1] != '\n') {
                    printf("\n");
                }
                if (last_id != NULL) {
                    free(last_id);
                }
                last_id = strdup(message.uuid);
            }
        }
        if (fds[INPUT_FD_INDEX].revents & POLLIN) {
            size_t bytes = 0;
            char *input = read_line(stdin, &bytes);
            char *output = NULL;
            message.parent = last_id;
            message.timestamp = time(NULL);
            message.content = input;
            message.username = "Yggdrasil";
            message.type = ARBOR_NEW;
            bytes = 0;
            output = marshal_message(&message, &bytes);
            if (bytes >= 0) {
                write(tcp_sock, output, bytes-1); // don't write the null byte
                write(tcp_sock, "\n", 1);
            }
        }
    }
    printf("Message failed to parse\n");
}
