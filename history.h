#ifndef history_h_INCLUDED
#define history_h_INCLUDED

#include "c_hashmap/hashmap.h"
#include "protocol.h"

struct history {
    size_t capacity;
    size_t last_element;
    arbor_msg_t ** elements;
    map_t id2index;
};

typedef struct history history_t;

#define DEF_HIST_SIZE 2

history_t *make_history();
size_t history_add(history_t *h, arbor_msg_t *element);
arbor_msg_t *history_get_idx(history_t *h, size_t index);
long history_get_id(history_t *h, char *uuid);
void free_history(history_t *h);

#endif // history_h_INCLUDED

