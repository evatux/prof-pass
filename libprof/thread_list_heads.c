#include <pthread.h>

#include "internal.h"

static volatile thread_list_heads_t *thread_list_heads;
pthread_mutex_t lock;

thread_list_heads_t *thread_list_heads_get() {
    return thread_list_heads;
}

void thread_list_heads_register(pthread_t tid, list_t *head) {
    thread_list_heads_t *tail = (thread_list_heads_t *)malloc(sizeof(thread_list_heads_t));
    tail->tid = tid;
    tail->head = head;
    tail->next = NULL;

    pthread_mutex_lock(&lock);

    thread_list_heads_t *node = thread_list_heads_get();
    if (!node) {
        thread_list_heads = tail;
    } else {
        while (node->next) node = node->next;
        node->next = tail;
    }

    pthread_mutex_unlock(&lock);
}
