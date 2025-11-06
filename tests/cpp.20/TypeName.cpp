#include <cpp.20/TypeName.hpp>

template<typename T>
static constexpr auto type_name = GenerateTypeName<T>::get_type_name();
template<typename T>
static constexpr auto type_name_hash = GenerateTypeName<T>::get_hash();

struct TestClass {};

static_assert(type_name<int> == "int");
static_assert(type_name<std::source_location> == "std::source_location");
static_assert(type_name<TestClass> == "TestClass");



int main() {
    return 0;
}