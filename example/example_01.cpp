#include <stdio.h>

static constexpr int value = 10;

int bar(); // unused declaration

inline void baz() {}

int foo() {
    baz();
    return value;
}

int main() {
    int x = foo();
    printf("foo():%d expected:%d\n", x, value);
    return 0;
}
