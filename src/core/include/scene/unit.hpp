#pragma once

#include <assert.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <vector>
#include <memory>

namespace gkit {
    class Application;
}

namespace gkit::scene {
    class Unit;

    template <typename T>
    concept IsUnitExtend = requires (T v) {
        std::is_base_of<Unit, T>();
    };

    /**
     * class Unit
     * @brief This is the gkit application unit. It is the basic unit of program composition.
     */
    class Unit {
    protected:
        Unit() noexcept;
        // Unit(Unit&&) noexcept;
        Unit(std::string name) noexcept;
        // Unit(std::string&& name) noexcept;
    public:
        /**
         * @brief Create a instance of the type which is based of class Unit.
         * @tparam T The type of the instance. It must be a class which is based of class Unit.
         * @param name The name of the instance.
         * @return A unique pointer to the instance. If the type can't be created, return nullptr.
         * @note It will throw error if the type is not a class which is based of class Unit.
         */
        template <IsUnitExtend T>
        static auto create(std::string name = "") noexcept -> std::unique_ptr<T>;
        virtual ~Unit() = default;

    public: // virtual methods
        virtual auto _ready()   -> void;
        virtual auto _process() -> void;
        virtual auto _physics_process() -> void;
        virtual auto _exit() -> void;

    protected: // control workflow handler
        friend Application;
        auto ready_handler() noexcept -> void;
        auto process_handler() noexcept -> void;
        auto physics_process_handler() noexcept -> void;
        auto exit_handler() noexcept -> void;

    public: // add and remove
        /**
         * @brief Add a child unit
         * @param child
         * @return void
         * @note the child unit will transfer its lifetime management rights to the unit
         */
        auto add_child(std::unique_ptr<Unit>&& child_ptr) noexcept -> void;
        auto remove_child(uint32_t index) noexcept -> void;
        auto remove_child(std::unique_ptr<gkit::scene::Unit>& child_ptr) noexcept -> void; 

    public: // cross-unit methods
        template<typename Unit_T, typename F, typename... Args>
        auto with_child(uint32_t index, F&& func, Args&&... args)
            -> std::optional<std::invoke_result_t<F, Unit_T&, Args...>>;

        template<IsUnitExtend T> 
        auto get_parent() noexcept -> std::optional<std::reference_wrapper<T>>;
        // template<> auto get_parent<Unit>() noexcept -> std::optional<std::reference_wrapper<Unit>>;

    protected: // propertries
        std::string name;
        Unit* parent = nullptr;
        
        std::atomic<bool> modified = false;
        mutable std::shared_mutex index_cache_rw_mutex;
        std::vector<uint32_t> active_index_cache;
        mutable std::shared_mutex children_rw_mutex;
        std::vector<std::unique_ptr<Unit>> children;

        // when (active_index_cache.size() / children.size() <= overload_factor)
        // children vector will realloc(call method @ref remap_children_and_cache())
        static const constexpr float overload_factor = 0.5f;

        auto get_children_count() const noexcept -> uint32_t;
        auto get_available_child(uint32_t index) noexcept -> std::optional<Unit*>;
        auto update_index_cache() -> void;
        auto remap_children_and_cache() -> void;

        // process setting
        std::atomic<bool> process_enabled = true;

        std::atomic<bool> ready_to_drop = false;
        inline auto drop() -> void {this->ready_to_drop.store(true);};
        auto drop_children() -> void;
    }; // class Unit

    template <IsUnitExtend T>
    auto Unit::create(std::string name) noexcept -> std::unique_ptr<T> {
        static_assert(std::is_base_of_v<Unit, T>, "T is not derived from Unit");
        try {
            auto ptr = std::unique_ptr<T>(new T(name));
            return ptr;
        } catch(...) {
            return nullptr;
        }
    }

    template<typename Unit_T, typename F, typename... Args>
    auto Unit::with_child(uint32_t index, F&& func, Args&&... args) 
    -> std::optional<std::invoke_result_t<F, Unit_T&, Args...>> {
        auto child_opt = get_available_child(index);
        if (!child_opt.has_value()) return std::nullopt;
        auto* child_ptr = *child_opt;
        auto child = dynamic_cast<Unit_T*>(child_ptr);
        if (child == nullptr) return std::nullopt;
        // return func(*child, std::forward<Args>(args)...);
        return std::invoke(func, *child, std::forward<Args>(args)...);
    } // Unit::with_child

    template<IsUnitExtend T>
    auto Unit::get_parent() noexcept -> std::optional<std::reference_wrapper<T>> {
        if (parent == nullptr) return std::nullopt;
        auto cast_parent = dynamic_cast<T*>(parent);
        return cast_parent == nullptr ? std::nullopt : *cast_parent;
    } // Unit::get_parent<T>
} // namespace gkit::scene