#include <cpp.20/FunctionView.hpp>
#include "../common/check.hpp"


int called_1 = 0;

struct Callable {
    int operator()(char a, float b) const {++called_1;return static_cast<int>(a + b);}
};

int called_2 = 0;
struct Writter {
    void set_non_const(int i) {called_2 = i;}
};

constexpr Callable c1;
constexpr auto test_create = FunctionView<int(char, float)>(c1);

int  call_site(FunctionView<int(int)> f, int v) {
    return f(v+1);
}

int main() {

    int r = test_create(1,2);
    CHECK_EQUAL(r,3);
    CHECK_EQUAL(called_1, 1);

    int s = call_site([wr = Writter()](int z) mutable {
        wr.set_non_const(1);
        return z+2;
    }, 50);
    CHECK_EQUAL(s, 53);
    CHECK_EQUAL(called_2, 1);

    

    return 0;

}