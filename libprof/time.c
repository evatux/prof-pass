#include <stdlib.h>
#include <immintrin.h>

#include "internal.h"

time_stamp_t time_stamp_start;

void time_init() {
    time_stamp_start = __rdtsc();
}

time_stamp_t time_get() {
    return __rdtsc() - time_stamp_start;
}
