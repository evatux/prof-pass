#include <stdio.h>
#include <unistd.h>

#include <string>
#include <map>

template <typename T>
struct my_class_t {
    std::map<T, int> data_;
    void add(T t) {
        auto counter = [&]() {
           static int count{};
           ++count;
           unsigned ms = (unsigned)count * 50 * 1000;
           usleep(ms);
           return count;
        };
        data_[t] = counter();
    }
};

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
    my_class_t<std::string> cache;
    cache.add("a");
    cache.add("b");
    return 0;
}
