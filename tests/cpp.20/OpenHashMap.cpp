#include <cpp.20/OpenHashMap.hpp>



struct PrimHash {
    constexpr std::size_t operator()(int x) const {return static_cast<std::size_t>(x);}
};

class TestClass {
public:
    int *_v;

    constexpr TestClass(int v):_v(new int(v)) {}
    constexpr ~TestClass() {delete _v;}
    constexpr TestClass(TestClass &&other):_v(other._v) {other._v = nullptr;}
    constexpr TestClass &operator=(TestClass &&other) {
        if (this != &other) {
            delete _v;
            _v = other._v;
            other._v = nullptr;
        }
        return *this;
    }
    constexpr int operator *() const {return *_v;}
};


constexpr int test_open_hash() {
    OpenHashMap<int, TestClass, PrimHash> hh;
    for (int i = 0; i < 100; ++i) {
        hh.emplace(i, TestClass(i*2+1));
    }
    for (int i = 0; i < 100;  ++i) {
        auto iter = hh.find(i);
        if (iter == hh.end()) return 1;
        if (*iter->second != i *2 + 1) return 2;
    }
    for (int i = 0; i < 100; i+=2) {
        hh.erase(i);
    }
    for (int i = 0; i < 100; i+=2) {
        auto iter = hh.find(i);
        if (iter != hh.end()) return 3;
    }
    for (int i = 1; i < 200; i+=2) {
        hh.emplace(i, TestClass(i*3+1));
    }
    for (int i = 0; i < 100;  ++i) {
        auto iter = hh.find(i);
        if (i & 1) {
            if (iter == hh.end()) return 4;
            if (*iter->second != i *2 + 1) return 5;
        } else {
            if (iter != hh.end()) return 6;;
        }
    }
    for (int i = 101; i < 200;++i ) {
        auto iter = hh.find(i);
        if (i & 1) {
            if (iter == hh.end()) return 7;
            if (*iter->second != i *3 + 1) return 8;
        } else {
            if (iter != hh.end()) return 9;
        }
    }
    return 0;
}

static_assert(test_open_hash() == 0, "Failed");;

int main() {
    return 0;
}