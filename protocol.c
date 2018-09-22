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
#define MAX_READ_BUF_SIZE (READ_BUF_SIZE*64)

// read_line reads a newline-delimited string of text from the given stream.
// It returns a pointer to the null-terminated string containing the text, and sets the
// value of bytes_read to indicate the length of the string that it returned (like
// `strlen()`, this does not count the terminating null byte).
// The returned string is dynamically allocated and must be `free()`ed.
// If the file stream returns EOF before a newline, this function will discard
// all bytes before the newline and return "". `bytes_read` will be 0.
char *read_line(FILE *input, size_t *bytes_read) {
    char * read_buf = (char *) malloc(READ_BUF_SIZE * sizeof(char));
    size_t curr_buf_len = READ_BUF_SIZE;
    memset(read_buf, 0, READ_BUF_SIZE);
    unsigned int index = 0;
    _Bool newline_found = false;
    while (!newline_found) {
        // read until either the buffer is full or find a newline
        while (index < curr_buf_len && // must check index before reading
               fread(read_buf+index, sizeof(char), 1, input) > 0) {
    	if (read_buf[index] == '\n') {
                read_buf[index] = '\0';
                newline_found = true;
                break;
    	}
    	index++;
        }
        // since we didn't find a newline, enlarge the buffer and try again
        if (curr_buf_len >= MAX_READ_BUF_SIZE) {
            break;
        }
        char * larger_buf = (char *) malloc(curr_buf_len + READ_BUF_SIZE * sizeof(char));
        memcpy(larger_buf, read_buf, curr_buf_len);
        free(read_buf);
        read_buf = larger_buf;
        curr_buf_len += READ_BUF_SIZE;
    }
    if (newline_found) {
        *bytes_read = strlen(read_buf);
        return read_buf;
    }
    *bytes_read = 0;
    return "";
}

