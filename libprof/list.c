#include "internal.h"

list_t **list_local_pcur() {
    static __thread list_t *list_cur;
    if (list_cur == NULL) {
        list_t *head = (list_t *)calloc(1, sizeof(list_t));
        thread_list_heads_register(pthread_self(), head);
        list_cur = head;
    }
    return &list_cur;
}

void list_local_grow(const char *name, dir_t dir) {
    list_t **pcur = list_local_pcur(), *cur = *pcur;
    if (cur->size == list_num_max_entries) {
        list_t *next = (list_t *)malloc(sizeof(list_t));
        if (!next) FATAL("fail to allocate list_t::next\n");
        cur->next = next;
        cur = cur->next;
        cur->size = 0;
        *pcur = cur;
    }
    cur->entries[cur->size++] = (entry_t){name, time_get(), dir};
}
