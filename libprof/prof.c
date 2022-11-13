#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <immintrin.h>

#include "prof.h"

const char *prof_fname = "prof.json";

#define FATAL(...) do { fprintf(stderr, "[libprof]" __VA_ARGS__); exit(2); } while (0)

void prof_exit() __attribute__((destructor));

__thread pthread_t tid_self;

typedef uint64_t time_stamp_t;
time_stamp_t time_stamp_start;
time_stamp_t get_time_stamp() {
    return __rdtsc() - time_stamp_start;
}

typedef enum { DIR_IN, DIR_OUT } dir_t;

typedef struct {
    const char *name;
    pthread_t tid;
    time_stamp_t time_stamp;
    dir_t dir;
} entry_t;

enum { list_num_max_entries = 1024 };
struct list_s;
typedef struct list_s {
    size_t size; // <= list_num_max_entries
    entry_t entries[list_num_max_entries];
    struct list_s *next;
} list_t;

list_t *list_head() {
    static list_t list;
    return &list;
}

list_t **list_pcur() {
    static list_t *list_cur;
    if (list_cur == NULL) list_cur = list_head();
    return &list_cur;
}

void list_grow(const char *name, dir_t dir) {
    list_t **pcur = list_pcur(), *cur = *pcur;
    if (cur->size == list_num_max_entries) {
        list_t *next = (list_t *)malloc(sizeof(list_t));
        if (!next) FATAL("fail to allocate list_t::next\n");
        cur->next = next;
        cur = cur->next;
        cur->size = 0;
        *pcur = cur;
    }
    cur->entries[cur->size++] = (entry_t){name, tid_self, get_time_stamp(), dir};
}

void prof_register_foo(const char *name) {
    // instead of first call
    if (tid_self == 0) {
        tid_self = pthread_self();
        time_stamp_start = get_time_stamp();
    }
    list_grow(name, DIR_IN);
}

void prof_unregister_foo(const char *name) {
    list_grow(name, DIR_OUT);
}

void prof_exit() {
    FILE *f = fopen(prof_fname, "w+");
    if (!f) FATAL("cannot open file '%s' for write\n", prof_fname);

    fprintf(f, "[\n");
    int first_entry_added = 0;

    const list_t *cur = list_head();
    size_t i = 0;
    while (1) {
        if (i < cur->size) {
            if (first_entry_added) fprintf(f, ",\n");
            const entry_t *entry = &cur->entries[i];
            fprintf(f, "{"
                    "  \"name\" : \"%s\""
                    ", \"cat\" : \"category0\""
                    ", \"ph\" : \"%c\""
                    ", \"ts\" : %llu"
                    ", \"pid\" : 1"
                    ", \"tid\" : %llu"
                    " }",
                    entry->name,
                    entry->dir == DIR_IN ? 'B' : 'E',
                    (unsigned long long)entry->time_stamp,
                    (unsigned long long)entry->tid);
            // printf("call '%s'\n", cur->entries[i].name);
            ++i;
            if (!first_entry_added) first_entry_added = 1;
        }
        if (i == cur->size) {
            if (cur->size == list_num_max_entries) {
                cur = cur->next;
                i = 0;
            } else {
                break;
            }
        }
    }

    fprintf(f, "\n]\n");
}
