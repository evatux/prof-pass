#include <stdlib.h>
#include <unistd.h>
#include <immintrin.h>

#include "internal.h"

time_stamp_t time_stamp_start;

void time_init() {
    time_stamp_start = __rdtsc();
}

time_stamp_t time_get() {
    return __rdtsc() - time_stamp_start;
}

double time_to_us(time_stamp_t time_stamp) {
    static double time_to_us_factor;
    if (time_to_us_factor == 0) {
        const unsigned calibration_time = 50 * 1000; // 50 ms
        time_stamp_t start = time_get();
        usleep(calibration_time);
        time_stamp_t dt = time_get() - start;
        time_to_us_factor = dt / calibration_time;
    }
    return time_stamp / time_to_us_factor;
}
