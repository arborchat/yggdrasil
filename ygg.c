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
#include "history.h"

#define INPUT_FD_INDEX 0
#define SOCKET_FD_INDEX 1
struct client {
    struct pollfd fds[2];
    history_t *history;
};

typedef struct client client_t;

client_t *make_client(int stdin_fd, int sock_fd) {
    client_t *client = malloc(sizeof(client_t));
    memset(client->fds, 0, sizeof(struct pollfd) *2);
    client->fds[INPUT_FD_INDEX].fd = stdin_fd;
    client->fds[INPUT_FD_INDEX].events = POLLIN;
    client->fds[SOCKET_FD_INDEX].fd = sock_fd;
    client->fds[SOCKET_FD_INDEX].events = POLLIN;
    client->history = make_history();
    return client;
}

void handle_messages(client_t *client) {
    FILE *sockfile = fdopen(client->fds[SOCKET_FD_INDEX].fd, "rw");
    arbor_msg_t stack_msg;
    char * last_id = NULL;
    while (poll(client->fds, 2, -1)) {
        if (client->fds[SOCKET_FD_INDEX].revents & POLLIN) {
            read_arbor_message(sockfile, &stack_msg);
            if (stack_msg.type == ARBOR_WELCOME) {
                printf("Type: %d Major: %d Minor: %d Root: %s Recent_Len: %d\n", stack_msg.type, stack_msg.major, stack_msg.minor, stack_msg.root, (int) stack_msg.recent_len);
                last_id = strdup(stack_msg.root);
            } else if (stack_msg.type == ARBOR_NEW) {
                arbor_msg_t *received = malloc(sizeof(arbor_msg_t));
                memcpy(received, &stack_msg, sizeof(arbor_msg_t));
                size_t index = history_add(client->history, received);
                printf("[%x]@%d %s: %s", (unsigned int) index, stack_msg.timestamp, stack_msg.username, stack_msg.content);
                if (stack_msg.content[strlen(stack_msg.content) -1] != '\n') {
                    printf("\n");
                }
                if (last_id != NULL) {
                    free(last_id);
                }
                last_id = strdup(stack_msg.uuid);
            }
        }
        if (client->fds[INPUT_FD_INDEX].revents & POLLIN) {
            size_t bytes = 0;
            char *input = read_line(stdin, &bytes);
            char *output = NULL;
            memset(&stack_msg, 0, sizeof(arbor_msg_t));
            stack_msg.parent = last_id;
            stack_msg.content = input;
            if (strncmp(input, "re:", 3) == 0) {
		long int reply_to_index = strtol(input+4, NULL, 16);
		arbor_msg_t *parent = history_get(client->history, reply_to_index);
		if (parent != NULL) {
                    stack_msg.parent = parent->uuid;
		}
		// ensure that the stack_msg contents do not contain the reply
		// prefix by incrementing the string pointer past it.
		stack_msg.content += (reply_to_index / 16) + 5;
            }
            stack_msg.timestamp = time(NULL);
            stack_msg.username = "Yggdrasil";
            stack_msg.type = ARBOR_NEW;
            bytes = 0;
            output = marshal_message(&stack_msg, &bytes);
            if (bytes > 0) {
                write(client->fds[SOCKET_FD_INDEX].fd, output, bytes-1); // don't write the null byte
                write(client->fds[SOCKET_FD_INDEX].fd, "\n", 1);
            }
        }
    }
}

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

    client_t *client = make_client(STDIN_FILENO, tcp_sock);
    // communicate
    handle_messages(client);
}
