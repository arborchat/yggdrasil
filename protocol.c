// for malloc
#include <stdlib.h>

// for read
#include <unistd.h>

// for memset
#include <string.h>

#include "protocol.h"

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

