#include "gkit/core/unique_object.hpp"
#include "gkit/core/value.hpp"
#include "test_utils.hpp"

#include <atomic>
#include <thread>
#include <type_traits>

namespace {
    struct Tracked : gkit::core::Object {
        static inline std::atomic<int> destroyed = 0;
        int value                                = 42;
        ~Tracked() override { ++destroyed; }
    };
    auto test_values() -> bool {
        using namespace gkit::core;
        static_assert(!std::is_constructible_v<Value, UniqueObject>);
        auto owner = UniqueObject::create<Tracked>();
        auto id    = owner.get_id();
        Value original(Map{{"object", Value(id)}, {"array", Value(Array{Value(42)})}});
        auto copy        = original;
        copy["array"][0] = 7;
        gkit::test::assert_if(original["array"][0].as_int64() == 42, "nested values must copy independently");
        gkit::test::assert_if(copy["object"].as_object().with<Tracked>([](auto& obj) {
            gkit::test::assert_if(obj.value == 42, "copied object value");
        }),
                              "object ID copy");
        bool caught = false;
        try {
            (void)copy.as_bool();
        } catch (const std::bad_variant_access&) {
            caught = true;
        }
        gkit::test::assert_if(caught, "wrong type must throw rather than terminate");
        return true;
    }
    auto test_borrow() -> bool {
        using namespace gkit::core;
        Tracked::destroyed = 0;
        std::optional<UniqueObject> owned(UniqueObject::create<Tracked>());
        auto id = owned->get_id();
        id.with<Tracked>([&](Tracked& borrowed) {
            auto moved = std::move(*owned);
            gkit::test::assert_if(owned->get() == nullptr, "moved owner must be empty");
            owned.emplace(std::move(moved));
            owned.reset();
            gkit::test::assert_if(!id.with([](Object&) {}) && Tracked::destroyed == 0, "retired object stays pinned");
            gkit::test::assert_if(borrowed.value == 42, "callback borrow remains usable");
        });
        gkit::test::assert_if(Tracked::destroyed == 1, "destroy exactly once after final borrow");
        for (int i = 0; i < 100; ++i) {
            auto owner = UniqueObject::create<Tracked>();
            auto live  = owner.get_id();
            std::thread reader([live] {
                for (int n = 0; n < 100; ++n) {
                    live.with<Tracked>([](auto& obj) {
                        gkit::test::assert_if(obj.value == 42, "concurrent borrow");
                    });
                }
            });
            {
                auto released = std::move(owner);
            }
            reader.join();
            gkit::test::assert_if(!live.with([](Object&) {}), "released ID must stay expired");
        }
        return true;
    }
    struct Throws : gkit::core::Object {
        Throws() { throw std::runtime_error("constructor failure"); }
    };
    struct Reentrant : gkit::core::Object {
        ~Reentrant() override { auto other = gkit::core::UniqueObject::create<Tracked>(); }
    };
    auto test_failure_and_reentry() -> bool {
        bool caught = false;
        try {
            auto object = gkit::core::UniqueObject::create<Throws>();
        } catch (const std::runtime_error&) {
            caught = true;
        }
        gkit::test::assert_if(caught, "constructor failures must propagate");
        auto object = gkit::core::UniqueObject::create<Reentrant>();
        return object.get() != nullptr;
    }
    auto test_ids() -> bool {
        struct Tag {};
        using Id        = gkit::core::templates::GenId<Tag>;
        auto& allocator = Id::IdAllocator::instance();
        auto a          = allocator.new_one();
        allocator.drop(a);
        allocator.drop(a);
        auto b = allocator.new_one();
        allocator.drop(a);
        allocator.drop(Id{});
        auto c = allocator.new_one();
        gkit::test::assert_if(b != c && a != b, "stale and duplicate drops cannot alias live IDs");
        gkit::test::assert_if(b.get_id() != c.get_id(), "a live slot cannot be allocated twice");
        allocator.drop(b);
        allocator.drop(c);
        return true;
    }
} // namespace
auto main() -> int {
    gkit::test::TestRunner()
        .add_test_func(test_values)
        .add_test_func(test_borrow)
        .add_test_func(test_ids)
        .add_test_func(test_failure_and_reentry)
        .run();
}
