// for malloc
#include <stdlib.h>

// for read
#include <unistd.h>

// for memset and strlen
#include <string.h>

// for _Bool
#include <stdbool.h>

// for json_parse and friends
#include "json-parser/json.h"

#include "protocol.h"

#define READ_BUF_SIZE 1024
#define MAX_READ_BUF_SIZE (READ_BUF_SIZE*64)

// read_line reads a newline-delimited string of text from the given stream.
// It returns a pointer to the null-terminated string containing the text, and sets the
// value of bytes_read to indicate the length of the string that it returned (like
// `strlen()`, this does not count the terminating null byte).
// 
// The returned string is dynamically allocated and must be `free()`ed.
// 
// If the file stream returns EOF before a newline, this function will discard
// all bytes before the newline and return "". `bytes_read` will be 0.
// 
// If the file stream contains a line longer than 10^16 bytes, it will be discarded
// and this function will return "" with `bytes_read` as 0.
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
    free(read_buf);
    *bytes_read = 0;
    return "";
}

size_t sizemax(size_t a, size_t b) {
    return a > b ? a : b;
}

_Bool keys_match(char* target, json_object_entry *key) {
    return strncmp(key->name, target, sizemax(strlen(target), key->name_length)) == 0;
}

// extract_welcome searches for the Root, Major, Minor, and Recent fields in the provided JSON
// and extracts them into `msg`. It ignores other fields. It returns true if it found all of the
// fields and false if it either failed to find a field or found a required field that had the wrong
// type.
_Bool extract_welcome(json_value *json, arbor_msg_t *msg) {
    _Bool found_root = false;
    _Bool found_major = false;
    _Bool found_minor = false;
    _Bool found_recent = false;
    for (unsigned int i = 0; i < json->u.object.length; i++) {
        if (keys_match("Root", &json->u.object.values[i])) {
            if (json->u.object.values[i].value->type != json_string) {
                return false;
            }
            msg->root = strndup(json->u.object.values[i].value->u.string.ptr, json->u.object.values[i].value->u.string.length);
            found_root = true;
        } else if (keys_match("Major", &json->u.object.values[i])) {
            if (json->u.object.values[i].value->type != json_integer) {
                return false;
            }
            msg->major = json->u.object.values[i].value->u.integer;
            found_major = true;
        } else if (keys_match("Minor", &json->u.object.values[i])) {
            if (json->u.object.values[i].value->type != json_integer) {
                return false;
            }
            msg->minor = json->u.object.values[i].value->u.integer;
            found_minor = true;
        } else if (keys_match("Recent", &json->u.object.values[i])) {
            if (json->u.object.values[i].value->type != json_array) {
                return false;
            }
            json_value *arr = json->u.object.values[i].value;
            msg->recent_len = arr->u.array.length;
            msg->recent = calloc(sizeof(char *), msg->recent_len);
            // ensure that every element of msg->recent starts out as NULL
            memset(msg->recent, 0, sizeof(char *) * msg->recent_len);
            for (unsigned int k = 0; k < msg->recent_len; k++) {
                if (arr->u.array.values[k]->type != json_string) {
                    // if it's not a string, it's invalid. Before returning, we must carefully
                    // deallocate all of the memory that we might have allocated so far
                    for (unsigned int m = 0; m < msg->recent_len; m++) {
                        if (msg->recent[m] != NULL) {
                            free(msg->recent[m]);
                        }
                    }
                    free(msg->recent);
                    return false;
                }
                // if we get here, the array element is a valid string
                msg->recent[k] = strndup(arr->u.array.values[k]->u.string.ptr, arr->u.array.values[k]->u.string.length);
            }
            found_recent = true;
        }
    }
    return found_major && found_minor && found_root && found_recent;
}

// parse_arbor_message extracts the JSON string in `text` into the struct
// `msg`. It returns true if it succeeded and false if it failed.
_Bool parse_arbor_message(char *text, arbor_msg_t* msg) {
    _Bool result = false;
    json_value *parsed = json_parse(text, strlen(text));
    if (parsed == NULL) {
        return false;
    }
    if (parsed->type != json_object) {
        goto parse_arbor_message_end;
    }
    // find the message type
    for (unsigned int i = 0; i < parsed->u.object.length; i++) {
        if (keys_match("Type", &parsed->u.object.values[i])) {
            if (parsed->u.object.values[i].value->type != json_integer) {
                goto parse_arbor_message_end;
            }
            msg->type = parsed->u.object.values[i].value->u.integer;
            break;
        }
    }
    switch (msg->type) {
        case ARBOR_WELCOME:
            result = extract_welcome(parsed, msg);
            break;
        default:
            goto parse_arbor_message_end;
    }
parse_arbor_message_end:
    json_value_free(parsed);
    return result;
}
// _Bool read_arbor_message(FILE *input, arbor_msg_t* msg) {
// }
