#include <stdio.h>
#include <unistd.h>

#include <thread>

inline void baz(int delay) {
    usleep(delay * 1000); // ms
}

void foo(int delay) {
    baz(delay);
}

int main() {
    std::thread worker([]() { foo(100); });
    foo(10);
    worker.join();
    return 0;
}
