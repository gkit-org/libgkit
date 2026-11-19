file(MAKE_DIRECTORY "${WORK_DIR}")
file(WRITE "${WORK_DIR}/main.cpp" [=[
#include <gkit/gkit.hpp>
template<class T>
concept ChainableSettings = requires(T& loop) {
    { loop.set_fixed_delta(0.1) } noexcept -> std::same_as<T&>;
    { loop.set_max_frame_delta(0.5) } noexcept -> std::same_as<T&>;
    { loop.set_max_fixed_steps(1) } noexcept -> std::same_as<T&>;
    { loop.set_initialize_sdl(false) } noexcept -> std::same_as<T&>;
    { loop.set_sdl_flags(std::nullopt) } noexcept -> std::same_as<T&>;
    { loop.set_parallel_update_enabled(true) } noexcept -> std::same_as<T&>;
    { loop.set_worker_count(2) } noexcept -> std::same_as<T&>;
    { loop.set_target_frame_rate(60) } noexcept -> std::same_as<T&>;
};
static_assert(ChainableSettings<gkit::Processer>);

namespace {
    class ConsumerSprite : public gkit::scene::Sprite2D {
    public:
        static auto _regist_method(gkit::core::reflect::ClassDB& db) -> void {
            db.regist<ConsumerSprite>("ConsumerSprite", "Sprite2D");
        }
    };
}

int main() {
    auto& db = gkit::core::reflect::ClassDB::instance();
    gkit::core::reflect::regist_types<ConsumerSprite>(db);
    auto sprite = gkit::core::UniqueObject::create_with_classname("ConsumerSprite");
    auto info = gkit::core::reflect::ClassDB::instance().find("ConsumerSprite");
    if (!sprite || sprite->get()->class_name() != "ConsumerSprite" || !info ||
        !info->set_field(sprite->get(), "name", gkit::core::Value(std::string("sprite")))) return 1;

    gkit::Processer loop;
    loop.set_initialize_sdl(false).set_target_frame_rate(120);
    auto player = gkit::core::UniqueObject::create<gkit::scene::Object2D>();
    auto id = player.get_id();
    loop.root().add_child("player", std::move(player));
    auto found = id.with<gkit::scene::Object2D>([](auto& node) { node.set_rotation(0.5f); });
    const gkit::math::Vector2 a(1,2);
    return found && (a * 2).x == 2 ? 0 : 1;
}
]=])
# This executable has no references to concrete built-in types that could pull extra archive members into the link.
file(WRITE "${WORK_DIR}/names_only.cpp" [=[
#include <gkit/core/reflect/registry.hpp>

const auto WINDOW_INFO = gkit::core::reflect::ClassDB::instance().find("Window");

auto main() -> int {
    if (!WINDOW_INFO) return 1;
    for (const char* name : {"Object", "Node", "Scene", "Object2D", "Sprite2D", "Viewport", "Window"}) {
        auto object = gkit::core::UniqueObject::create_with_classname(name);
        if (!object || object->get()->class_name() != name) return 1;
    }
    return 0;
}
]=])
file(WRITE "${WORK_DIR}/CMakeLists.txt" "cmake_minimum_required(VERSION 3.20)\nproject(consumer LANGUAGES CXX)\n")
file(APPEND "${WORK_DIR}/CMakeLists.txt" "add_library(engine INTERFACE)\n"
    "target_compile_features(engine INTERFACE cxx_std_20)\n"
    "target_include_directories(engine INTERFACE \"${INCLUDE_DIR}\")\n"
    "target_link_libraries(engine INTERFACE \"${LIBRARY}\")\n")
if(STATIC_CONSUMER)
    file(APPEND "${WORK_DIR}/CMakeLists.txt" "find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3-static)\n"
        "target_link_libraries(engine INTERFACE SDL3::SDL3-static)\n")
endif()
file(APPEND "${WORK_DIR}/CMakeLists.txt" [=[
add_executable(consumer main.cpp)
add_executable(names_only names_only.cpp)
target_link_libraries(consumer PRIVATE engine)
target_link_libraries(names_only PRIVATE engine)
]=])
# Match the library configuration, including the Windows debug CRT and STL ABI.
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${WORK_DIR}" -B "${WORK_DIR}/build" -G Ninja
    "-DSDL3_DIR=${SDL3_DIR}" "-DCMAKE_CXX_COMPILER=${CXX}" "-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}" RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "consumer configure failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${WORK_DIR}/build" RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "consumer build failed")
endif()
foreach(program consumer names_only)
    if(WIN32)
        execute_process(COMMAND "${CMAKE_COMMAND}" -E env "PATH=${RUNTIME_DIR};$ENV{PATH}"
            "${WORK_DIR}/build/${program}.exe" RESULT_VARIABLE result)
    else()
        execute_process(COMMAND "${WORK_DIR}/build/${program}" RESULT_VARIABLE result)
    endif()
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${program} execution failed: ${result}")
    endif()
endforeach()
