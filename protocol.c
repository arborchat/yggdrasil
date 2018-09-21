// for malloc
#include <stdlib.h>

// for read
#include <unistd.h>

// for memset and strlen
#include <string.h>

// for _Bool
#include <stdbool.h>

#include "protocol.h"

#define READ_BUF_SIZE 1024

// read_line reads a newline-delimited string of text from the given file descriptor
// It returns a pointer to the null-terminated string containing the text, and sets the
// value of bytes_read to indicate the length of the string that it returned (like
// `strlen()`, this does not count the terminating null byte).
// The returned string is dynamically allocated and must be `free()`ed.
// If the file descriptor returns EOF before a newline, this function will discard
// all bytes before the newline and return "". `bytes_read` will be 0.
// 
// This function is not thread-safe. When it reads data past a newline, it buffers
// it for the next invocation.
char *read_line(int fd, int *bytes_read) {
    static char *excess = NULL;
    static int excess_size = 0;
    char * read_buf = (char *) malloc(READ_BUF_SIZE * sizeof(char));
    memset(read_buf, 0, READ_BUF_SIZE);
    int index = 0;
    int num_read = read(fd, read_buf, READ_BUF_SIZE);
    _Bool newline_found = false;
    for (; index <= num_read; index++) {
	if (read_buf[index] == '\n') {
            read_buf[index] = '\0';
            newline_found = true;
            break;
	}
    }
    if (newline_found) {
        excess_size = num_read - (index+1);
        excess = (char *) malloc(excess_size * sizeof(char));
        memcpy(excess, read_buf + index + 1, excess_size);
        *bytes_read = strlen(read_buf);
        return read_buf;
    }
    *bytes_read = 0;
    return "";
}

