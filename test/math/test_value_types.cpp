#include "gkit/math/matrix3.hpp"
#include "gkit/math/matrix4.hpp"
#include "gkit/math/vector2.hpp"
#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"
#include "test_utils.hpp"

#include <type_traits>

static_assert(std::is_standard_layout_v<gkit::math::Vector2>);
static_assert(std::is_trivially_copyable_v<gkit::math::Vector3>);
static_assert(std::is_trivially_copyable_v<gkit::math::Vector4>);
static_assert(sizeof(gkit::math::Vector4) == 4 * sizeof(float));

auto test_values() -> bool {
    using namespace gkit::math;
    const Vector2 a(1, 2);
    const Vector3 b(1, 2, 3);
    const Vector4 c(1, 2, 3, 4);
    gkit::test::assert_if((a * 2).x == 2 && (b / 2).z == 1.5f && (-c).w == -4, "const arithmetic");
    gkit::test::assert_if(Vector2{}.normalize().length_sq() == 0, "normalize zero remains zero");
    gkit::test::assert_if(!Matrix3::inverse(Matrix3::zero()), "singular matrix rejected");
    return true;
}


auto main() -> int {
    gkit::test::TestRunner().add_test_func(test_values).run();
}
