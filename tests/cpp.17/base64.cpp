#include <cpp.17/base64.hpp>


template<typename U, typename V>
constexpr bool compare_blobs(U u, V v) {
    if (u.size() != v.size()) return false;
    std::size_t sz = u.size();
    for (std::size_t i = 0; i < sz; ++i) {
        if (u[i] != v[i]) return false;
    }
    return true;
}

constexpr bool compare_for_test(std::basic_string_view<unsigned char> a, std::string_view b) {
    return compare_blobs(a,b);
}

static_assert(compare_for_test(BinaryData("aGVsbG8gd29ybGQ="), std::string_view("hello world")));

constexpr bool test_encode(const char *msg, const char *encode) {
    char buff[50] = {};
    std::string_view subj( msg);
    auto iter = base64.encode(subj.begin(), subj.end(), std::begin(buff));
    std::string_view res(buff, std::distance(std::begin(buff), iter));
    return res == std::string_view(encode);
}

static_assert(test_encode("this is test!","dGhpcyBpcyB0ZXN0IQ=="));


int main() {
    return 0;
}