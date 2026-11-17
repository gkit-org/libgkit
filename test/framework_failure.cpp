#include "test_utils.hpp"
auto main() -> int {
    gkit::test::TestRunner()
        .add_test_func([] {
            return false;
        })
        .run();
    return 0;
}
