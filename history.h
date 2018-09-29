#ifndef history_h_INCLUDED
#define history_h_INCLUDED

#include "protocol.h"

struct history {
    size_t capacity;
    size_t last_element;
    arbor_msg_t ** elements;
};

typedef struct history history_t;

#define DEF_HIST_SIZE 2

history_t *make_history();
size_t history_add(history_t *h, arbor_msg_t *element);
arbor_msg_t *history_get(history_t *h, size_t index);
void free_history(history_t *h);

#endif // history_h_INCLUDED

