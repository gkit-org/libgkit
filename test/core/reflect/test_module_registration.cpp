#include "gkit/gkit.hpp"
#include "test_utils.hpp"

#include <array>
#include <barrier>
#include <string_view>
#include <thread>

namespace {
    using gkit::core::Object;
    using gkit::core::reflect::ClassDB;
    using gkit::core::reflect::regist_types;
    using gkit::core::reflect::Resistible;
    using gkit::test::assert_if;

    struct Parent : Object {
        static auto _regist_method(ClassDB& db) -> void { db.regist<Parent>("TestParent"); }
    };

    struct Child : Parent {
        static auto _regist_method(ClassDB& db) -> void { db.regist<Child>("TestChild", "TestParent"); }
    };

    class PrivateRegistration {
        static auto _regist_method(ClassDB&) -> void;
    };

    struct NonStaticRegistration {
        auto _regist_method(ClassDB&) -> void;
    };

    struct WrongParameter {
        static auto _regist_method(const ClassDB&) -> void;
    };

    struct WrongReturn {
        static auto _regist_method(ClassDB&) -> int;
    };

    static_assert(Resistible<Parent> && Resistible<Child>);
    static_assert(!Resistible<PrivateRegistration> && !Resistible<NonStaticRegistration>);
    static_assert(!Resistible<WrongParameter> && !Resistible<WrongReturn>);


    auto test_concurrent_first_access() -> bool {
        std::array<ClassDB*, 4> registries{};
        std::barrier start(static_cast<std::ptrdiff_t>(registries.size()));
        {
            std::vector<std::jthread> threads;
            threads.reserve(registries.size());
            for (auto& result : registries) {
                threads.emplace_back([&start, &result]() -> void {
                    start.arrive_and_wait();
                    result = &ClassDB::instance();
                    assert_if(result->find("Window") != nullptr, "first access observes the complete registry");
                });
            }
        }
        for (const auto* registry : registries) {
            assert_if(registry == &ClassDB::instance(), "all callers receive the same registry");
        }
        return true;
    }


    template<class T>
    auto check_type(const ClassDB& db, const char* name, std::string_view parent_name) -> void {
        const auto info = db.find(name);
        assert_if(info && info == db.find_with_raw(typeid(T).name()), name);
        const auto* parent = info->parent();
        assert_if(parent_name.empty() ? parent == nullptr : parent && parent->class_name == parent_name, name);
        const auto owner = db.create(name);
        assert_if(owner && owner->get() && owner->get()->class_name() == name, name);
    }


    auto test_module_lists() -> bool {
        const auto& db = ClassDB::instance();
        check_type<Object>(db, "Object", "");
        check_type<gkit::core::Node>(db, "Node", "");
        check_type<gkit::scene::Scene>(db, "Scene", "Node");
        check_type<gkit::scene::Object2D>(db, "Object2D", "Node");
        check_type<gkit::scene::Sprite2D>(db, "Sprite2D", "Object2D");
        check_type<gkit::graphic::Viewport>(db, "Viewport", "Node");
        check_type<gkit::graphic::Window>(db, "Window", "Viewport");

        const auto info = db.find("Scene");
        auto owner      = db.create("Scene");
        assert_if(owner.has_value(), "Scene factory");
        auto* scene = dynamic_cast<gkit::scene::Scene*>(owner->get());
        assert_if(scene != nullptr, "Scene factory produces a Scene");
        assert_if(info->set_field(scene, "name", gkit::core::Value("scene")), "inherited Node setter");
        assert_if(info->set_field(scene, "active", gkit::core::Value(false)), "Scene active setter");
        assert_if(info->set_field(scene, "paused", gkit::core::Value(true)), "Scene paused setter");
        assert_if(scene->get_name() == "scene" && !scene->is_active() && scene->is_paused(),
                  "reflected setters update the native properties");
        scene->set_name("renamed");
        scene->activate();
        scene->resume();
        const auto name   = info->get_field(scene, "name");
        const auto active = info->get_field(scene, "active");
        const auto paused = info->get_field(scene, "paused");
        assert_if(name && name->as_string() == "renamed" && active && active->as_bool() && paused && !paused->as_bool(),
                  "reflected getters observe native edits");

        gkit::Processer first;
        gkit::Processer second;
        assert_if(first.root().class_name() == "Node" && second.root().class_name() == "Node" &&
                      info == db.find("Scene"),
                  "creating processers reuses built-in metadata");
        return true;
    }


    auto test_registration_order() -> bool {
        auto& db = ClassDB::instance();
        try {
            regist_types<Child, Parent>(db);
            return false;
        } catch (const std::invalid_argument& error) {
            const std::string_view message(error.what());
            assert_if(message.find("TestParent") != message.npos && message.find("TestChild") != message.npos,
                      "registration errors identify both types");
        }
        assert_if(!db.find("TestChild") && !db.find("TestParent"), "a failing first entry stops the list");
        regist_types<Parent, Child>(db);
        check_type<Child>(db, "TestChild", "TestParent");
        return true;
    }
} // namespace


auto main() -> int {
    gkit::test::TestRunner()
        .add_test_func(test_concurrent_first_access)
        .add_test_func(test_module_lists)
        .add_test_func(test_registration_order)
        .run();
}
