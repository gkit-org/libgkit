#include "core/reflect/seralize.hpp"
#include "gkit/core/reflect/registry.hpp"
#include "test_utils.hpp"

#include <atomic>
#include <thread>

namespace {
    struct Base : gkit::core::Object {
        int base = 7;
    };
    struct Child : Base {
        int child = 9;
    };
    struct Complete : gkit::core::Object {
        int value = 11;
    };

    auto test_snapshots() -> bool {
        using namespace gkit::core;
        auto& db = reflect::ClassDB::instance();
        db.regist<Base>("ConcurrentBase").add_field("ConcurrentBase", "base", &Base::base);
        db.regist<Child>("ConcurrentChild", "ConcurrentBase").add_field("ConcurrentChild", "child", &Child::child);
        auto info = db.find("ConcurrentChild");
        int count = 0;
        info->for_each_field([&](const auto&, const auto&, const auto*) -> void {
            ++count;
        });
        gkit::test::assert_if(count == 2, "inheritance must work before parent lookup");
        std::string raw(typeid(Base).name());
        gkit::test::assert_if(db.find_with_raw(raw.c_str()) != nullptr, "raw lookup must compare string contents");
        bool duplicate = false;
        try {
            db.add_field("ConcurrentChild", "base", &Child::child);
        } catch (const std::invalid_argument&) {
            duplicate = true;
        }
        gkit::test::assert_if(duplicate, "duplicate inherited field rejected");
        std::atomic<bool> done = false;
        std::thread reader([&]() -> void {
            while (!done.load()) {
                if (auto published = db.find("Complete")) {
                    gkit::test::assert_if(published->fields.size() == 1,
                                          "transaction must publish complete description");
                }
            }
        });
        db.register_class<Complete>("Complete", [](auto& staging) -> void {
            staging.add_field("Complete", "value", &Complete::value);
        });
        done = true;
        reader.join();
        Child child;
        gkit::test::assert_if(info->get_field(&child, "base")->as_int64() == 7, "inherited getter");
        std::weak_ptr<const reflect::ClassInfo> old = info;
        db.add_field("ConcurrentChild", "another", &Child::child);
        gkit::test::assert_if(info->fields.size() == 1, "existing snapshot remains readable");
        info.reset();
        gkit::test::assert_if(old.expired(), "obsolete unreferenced metadata released");
        auto owner = UniqueObject::create<Child>();
        reflect::SerdeStruct serialized(owner.get_id());
        gkit::test::assert_if(serialized.available(), "serialize registered root");
        bool expired = false;
        try {
            serialized.from(ObjectId{});
        } catch (const std::invalid_argument&) {
            expired = true;
        }
        gkit::test::assert_if(expired && !serialized.available(), "failed root clears stale serialization");
        struct Unknown : Object {};
        auto unknown      = UniqueObject::create<Unknown>();
        bool unregistered = false;
        try {
            serialized.from(unknown.get_id());
        } catch (const std::invalid_argument&) {
            unregistered = true;
        }
        gkit::test::assert_if(unregistered, "unregistered type reports an error instead of dereferencing null");
        return true;
    }
} // namespace

auto main() -> int {
    gkit::test::TestRunner().add_test_func(test_snapshots).run();
}
