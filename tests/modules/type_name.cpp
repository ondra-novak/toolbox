
import ondra.toolbox.type_name;
import <string_view>;
import <source_location>;



struct TestClass {};

static_assert(type_name<int> == "int");
static_assert(type_name<std::source_location> == "std::source_location");
static_assert(type_name<TestClass> == "TestClass");



int main() {
    return 0;
}