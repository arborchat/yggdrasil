// for malloc
#include <stdlib.h>

// for read
#include <unistd.h>

// for memset
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// for htons and friends
#include <arpa/inet.h>

#define READ_BUF_SIZE 1024

// read_line reads a newline-delimited string of text from the given file descriptor
// It returns a pointer to the null-terminated string containing the text, and sets the
// value of bytes_read to indicate the length of the string that it returned.
// The returned string is dynamically allocated and must be `free()`ed.
char *read_line(int fd, int *bytes_read) {
    char * read_buf = (char *) malloc(READ_BUF_SIZE * sizeof(char));
    memset(read_buf, 0, READ_BUF_SIZE);
    int index = 0;
    int num_read = read(fd, read_buf, READ_BUF_SIZE);
    for (; index <= num_read; index++) {
	if (read_buf[index] == '\n') {
    		read_buf[index] = '\0';
    		*bytes_read = index + 1;
    		return read_buf;
	}
    }
    return "";
}

int main(int argc, char* argv[]) {
    int tcp_sock;
    struct sockaddr_in addr;
    printf("Welcome to Yggdrasil!\n");

    // create socket
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        perror("socket");
    }

    // configure server address information
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7777);
    if (1 != inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr)) {
        perror("Invalid IP");
    }

    // connect to server
    if (0 != connect(tcp_sock, (struct sockaddr*) &addr, sizeof(struct sockaddr_in))) {
        perror("Failed to connect to server");
    }
    printf("Connection successful\n");

    // communicate
    int num_read;
    printf("%s\n", read_line(tcp_sock, &num_read));
}
