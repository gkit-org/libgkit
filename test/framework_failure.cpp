#include "test_utils.hpp"
auto main() -> int {
    gkit::test::TestRunner()
        .add_test_func([]() -> bool {
            return false;
        })
        .run();
    return 0;
}
