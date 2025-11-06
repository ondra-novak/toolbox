#include <cpp.20/AnyRef.hpp>
#include "../common/check.hpp"

int main() {

    int v1 = 42;
    AnyRef rv1(v1);

    CHECK(holds_alternative<int>(rv1));
    CHECK_EQUAL(get<int>(rv1),42);
    int &v2 = get<int>(rv1);

    AnyRefConst rv2(v2);
    CHECK(holds_alternative<int>(rv2));
    CHECK_EQUAL(get<int>(rv2),42);
    const int &v3 = get<int>(rv2);
    //int &v4 = get<int>(rv2); //can't compile

    CHECK_EQUAL(v1,v3);
    CHECK(&v1 == &v3);

    CHECK(!holds_alternative<bool>(rv1));
    CHECK(!holds_alternative<unsigned int>(rv2));

}
