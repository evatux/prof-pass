#include <pthread.h>

#include "prof.h"
#include "internal.h"

const char *prof_fname = "prof.json";

int skip_level(int call_level) {
    static int max_level = -1;
    if (max_level == -1) {
        const char *env = getenv("PROF_MAX_LEVEL");
        max_level = env ? atoi(env) : 3;
    }
    return call_level > max_level;
}

static pthread_once_t init_once_flag = PTHREAD_ONCE_INIT;
void init_once() {
    demangler_init();
    time_init();
}

void prof_register_foo(const char *name) {
    pthread_once(&init_once_flag, init_once);
    list_local_grow(name, DIR_IN);
}

void prof_unregister_foo(const char *name) {
    list_local_grow(name, DIR_OUT);
}

void prof_exit() __attribute__((destructor));
void prof_exit() {
    FILE *f = fopen(prof_fname, "w+");
    if (!f) FATAL("cannot open file '%s' for write\n", prof_fname);

    fprintf(f, "[\n");
    int first_entry_added = 0;

    thread_list_heads_t *tl_heads = thread_list_heads_get();
    while(tl_heads) {
        pthread_t tid = tl_heads->tid;
        const list_t *cur = tl_heads->head;
        int call_level = 0;
        size_t i = 0;
        while (1) {
            if (i < cur->size) {
                const entry_t *entry = &cur->entries[i];
                call_level += entry->dir == DIR_IN ? 1 : 0;
                if (skip_level(call_level)) goto skip;

                if (first_entry_added) fprintf(f, ",\n");
                char *cxx_name = demangler(entry->name);
                const char *name = cxx_name ? cxx_name : entry->name;
                fprintf(f, "{"
                        "  \"name\" : \"%s\""
                        ", \"cat\" : \"category0\""
                        ", \"ph\" : \"%c\""
                        ", \"ts\" : %.6f"
                        ", \"pid\" : 1"
                        ", \"tid\" : %llu"
                        " }",
                        name,
                        entry->dir == DIR_IN ? 'B' : 'E',
                        time_to_us(entry->time_stamp),
                        (unsigned long long)tid);
                // printf("call '%s'\n", cur->entries[i].name);
                free(cxx_name);
                if (!first_entry_added) first_entry_added = 1;
                skip:
                call_level += entry->dir == DIR_IN ? 0 : -1;
                ++i;
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
        tl_heads = tl_heads->next;
    }

    fprintf(f, "\n]\n");
}
