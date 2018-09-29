#ifndef history_h_INCLUDED
#define history_h_INCLUDED

#include "protocol.h"

struct history {
    size_t capacity;
    size_t last_element;
    arbor_msg_t ** elements;
};

typedef struct history history_t;

#define DEF_HIST_SIZE 100

history_t *make_history();
void free_history(history_t *h);

#endif // history_h_INCLUDED

