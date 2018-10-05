#include <stdlib.h>
#include <string.h>

#include "history.h"

// make_history allocates a new history.
history_t *make_history() {
    history_t *h = malloc(sizeof(history_t));
    memset(h, 0, sizeof(history_t));
    h->elements = calloc(DEF_HIST_SIZE, sizeof(arbor_msg_t*));
    h->capacity = DEF_HIST_SIZE;
    h->id2index = hashmap_new();
    return h;
}

// enlarge doubles the capacity of the history.
static void enlarge(history_t *h) {
    size_t target_capacity = h->capacity * 2;
    arbor_msg_t **new_h = calloc(target_capacity, sizeof(arbor_msg_t*));
    memcpy(new_h, h->elements, (h->last_element +1)*sizeof(arbor_msg_t*));
    free(h->elements);
    h->elements = new_h;
    h->capacity = target_capacity;
}

// history_add inserts the given message pointer into the history
// and returns the index at which it is stored.
size_t history_add(history_t *h, arbor_msg_t *element) {
    if (h->capacity == h->last_element +1) {
	enlarge(h);
    }
    h->elements[h->last_element++] = element;
    hashmap_put(h->id2index, strdup(element->uuid), (void *) (h->last_element-1));
    return h->last_element-1;
}

// history_get_idx returns the element within the history at the given
// index, or NULL if the index is out of bounds.
arbor_msg_t *history_get_idx(history_t *h, size_t index) {
    if (index <= h->last_element) {
        return h->elements[index];
    }
    return NULL;
}

// history_get_id returns the element within the history with the given
// message ID, if there is one. If no message with the ID is found, it
// returns NULL.
long history_get_id(history_t *h, char *uuid) {
    size_t index;
    if (hashmap_get(h->id2index, uuid, (any_t*) &index) != MAP_OK) {
        return -1;
    }
    return index;
}

// free_history releases the memory stored by the history itself,
// but not the messages stored within the history.
void free_history(history_t *h) {
    if (h != NULL) {
        if (h->elements != NULL) {
            free(h->elements);
        }
        free(h);
    }
}
