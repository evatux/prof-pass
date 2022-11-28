#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define FATAL(...) do { fprintf(stderr, "[libprof]" __VA_ARGS__); exit(2); } while (0)

// time
typedef uint64_t time_stamp_t;
void time_init();
time_stamp_t time_get();

// demangler
void demangler_init();
char *demangler(const char *abi);

// entries
typedef enum { DIR_IN, DIR_OUT } dir_t;
typedef struct {
    const char *name;
    time_stamp_t time_stamp;
    dir_t dir;
} entry_t;

// list
enum { list_num_max_entries = 1024 };
typedef struct list_s {
    size_t size; // <= list_num_max_entries
    entry_t entries[list_num_max_entries];
    struct list_s *next;
} list_t;

list_t **list_local_pcur();
void list_local_grow(const char *name, dir_t dir);

// list thread
typedef struct thread_list_heads_s {
    pthread_t tid;
    list_t *head;
    struct thread_list_heads_s *next;
} thread_list_heads_t;
void thread_list_heads_register(pthread_t tid, list_t *head);
thread_list_heads_t *thread_list_heads_get();

#endif
