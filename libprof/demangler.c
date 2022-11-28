#include <dlfcn.h>
#include <stdio.h>

#include "internal.h"

#define CXA_DEMANGLE "__cxa_demangle"
typedef char* (*abi_demangle_f)(const char *mangled_name, char *output_buffer, size_t *length, int *status);

static abi_demangle_f abi_demangle;
void demangler_init() {
    void *h = dlopen("libstdc++.so.6", RTLD_LOCAL | RTLD_LAZY);
    printf("@@@ h : %p\n", h);
    if (!h) return;
    abi_demangle = (abi_demangle_f)dlsym(h, CXA_DEMANGLE);
    if (!abi_demangle) return;
    fprintf(stderr, "@@@ demangler initialized\n");
}

char *demangler(const char *abi) {
    if (!abi_demangle) return NULL;
    int status;
    char *ret = abi_demangle(abi, 0, 0, &status);
    if (status) return NULL;
    return ret;
}
