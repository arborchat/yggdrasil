#ifndef protocol_h_INCLUDED
#define protocol_h_INCLUDED

#include <sys/types.h>
#include <stdio.h>

struct arbor_message {
    int type;
    int major;
    int minor;
    int timestamp;
    char *root;
    char *uuid;
    char *parent;
    char *content;
    char *username;
    size_t recent_len;
    char **recent;
}; 

typedef struct arbor_message arbor_msg_t;

#define ARBOR_WELCOME 0
#define ARBOR_QUERY 1
#define ARBOR_NEW 2

char *read_line(FILE* input, size_t *bytes_read);
_Bool parse_arbor_message(const char *text, arbor_msg_t* msg);
_Bool read_arbor_message(FILE *input, arbor_msg_t* msg);
char *marshal_message(arbor_msg_t *msg, size_t *byte_written);

#endif // protocol_h_INCLUDED

