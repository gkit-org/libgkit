#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <memory>

namespace gkit {
    class Processer;
}

namespace gkit::core::scene {
    class Unit;

    template <typename T>
    concept IsUnitExtend = requires (T v) {
        { std::is_base_of_v<Unit, T> } -> std::convertible_to<bool>;
    };

    /**
     * class Unit
     * @brief This is the gkit application unit. It is the basic unit of program composition.
     * @note This class cannot be copied and constructed externally. If you want to create a unit,
     * you should use the function @ref gkit::core::scene::Unit::create<T>().
     *
     * If you want to move an Unit instance, you should move with the std::unique_ptr by std::move().
     */
    class Unit {
    protected:
        Unit() noexcept;
        Unit(std::string name) noexcept;
    public:
        /**
         * @brief Create a instance of the type which is based of class Unit.
         * @tparam T The type of the instance. It must be a class which is based of class Unit.
         * @param name The name of the instance.
         * @return A unique pointer to the instance. If the type can't be created, return nullptr.
         * @note It will throw error if the type is not a class which is based of class Unit.
         */
        template <IsUnitExtend T>
        static auto create(std::string name) noexcept -> std::unique_ptr<T>;
        virtual ~Unit() = default;

    public: // virtual methods
        virtual auto ready()   -> void;
        virtual auto process() -> void;
        virtual auto physics_process() -> void;
        virtual auto exit() -> void;

    protected: // control workflow handler
        friend Processer;
        /**
         * @brief The call the @ref _ready() of the unit and the children.
         * @note It will be called when the unit is ready.
         * It always will be called when the call @ref gkit::application::run
         * and an unit become the child of another Unit instance by calling 
         * @ref add_child if the unit is active.
         * 
         * It will call the @ref _ready() of the children of the unit 
         * in the forward order of valid indexes and call this one.
         */
        auto ready_handler() noexcept -> void;

        /**
         * @brief The call the @ref _process() of the unit and the children.
         * @note It will be called before the frame begin. And call the @ref _process() 
         * of the children of the unit(the called order is same as @ref ready_handler)
         * 
         * Before call the @ref _process(), it will call the @ref update_index_cache()
         * to make sure the index cache is valid. After call @ref _process() for its children 
         * and itself, it will call the @ref drop_children() to drop the children whose
         * @ref ready_to_drop is true.
         */
        auto process_handler() noexcept -> void;

        /**
         * @brief The call the @ref _physics_process() of the unit and the children.
         * @note It will be called according to a fixed time interval, rather than every frame.
         *
         * It is not use in the current version.
         */
        auto physics_process_handler() noexcept -> void;

        /**
         * @brief The call the @ref _exit() of the unit and the children.
         * @note It will be called when the unit is about to be destroyed.
         * The call order is same as @ref ready_handler().
         */
        auto exit_handler() noexcept -> void;

    public: // add and remove
        /**
         * @brief Add a child unit
         * @param child
         * @return void
         * @note the child unit will transfer its lifetime management rights to the unit.
         *
         * The child will be added to the end of the children list immediately. But it will
         * be available after call @ref update_index_cache(), which will be happened when
         * @ref process_handler() is called.
         */
        auto add_child(std::unique_ptr<Unit>&& child_ptr) -> void;

        /**
         * @brief Remove a child unit by index
         * @param index
         * @return void
         * @note It will be marked as deleted, but not deleted immediately.
         * It will be deleted when the parent all the end of @ref process_handler().
         *
         * If the index is out of range, it will do nothing.
         */
        auto remove_child(uint32_t index) noexcept -> void;

        /**
         * @brief Remove a child unit by name.
         * @param child_name The name of the child.
         * @return void
         * @note It will be marked as deleted, but not deleted immediately.
         * It will be deleted when the parent all the end of @ref process_handler().
         *
         * If the name is not found, it will do nothing.
         */
        auto remove_child(const std::string& child_name) noexcept -> void;

        /**
         * It is not use in the current version.
         */
        // auto remove_child(std::unique_ptr<gkit::core::scene::Unit>& child_ptr) noexcept -> void; 

        /**
         * @brief Get the number of children
         * @return uint32_t
         * @note The result just mean the number of current frame.
         */
        auto get_children_count() const noexcept -> uint32_t;

        /**
         * @brief Drop the unit(by set @ref ready_to_drop to true)
         * @return void
         * @note The unit will be dropped after the end of @ref process_handler().
         */
        inline auto drop() -> void {this->ready_to_drop.store(true);};

    public: // cross-unit methods
        /**
         * @brief Do something by a callable method.
         * @tparam Unit_T The type derived from unit.
         * @tparam F The callable method.
         * @tparam Args The arguments of the callable method.
         * @param index The index of the child.
         * @param func The callable method, which return type is not void.
         * @param args The arguments of the callable method.
         * @return the callable method's return value. 
         * If the index is out of range, return std::nullopt.
         */
        template<IsUnitExtend Unit_T, typename F, typename... Args>
        auto with_child(uint32_t index, F&& func, Args&&... args)
            -> std::invoke_result_t<F, Unit_T&, Args...>;

        /**
         * @brief Do something by a callable method.
         * @tparam Unit_T The type derived from unit.
         * @tparam F The callable method.
         * @tparam Args The arguments of the callable method.
         * @param child_name The name of the child.
         * @param func The callable method, which return type is not void.
         * @param args The arguments of the callable method.
         * @return the callable method's return value.
         * @throws std::out_of_range If child name is not found.
         * @throws std::invalid_argument If child type cast fails.
         */
        template<IsUnitExtend Unit_T, typename F, typename... Args>
        auto with_child(const std::string& child_name, F&& func, Args&&... args)
            -> std::invoke_result_t<F, Unit_T&, Args...>;

        /**
         * @brief Get the unit parent refernce.
         * @tparam T The type derived from unit.
         * @return std::optional<std::reference_wrapper<T>>
         * If the parent is not the target type, return std::nullopt.
         */
        template<IsUnitExtend T> 
        auto get_parent() noexcept -> std::optional<std::reference_wrapper<T>>;

    protected: // propertries
        std::string name;
        Unit* parent = nullptr;
        
    private: // children management
        std::atomic<bool> modified = false;

        std::unordered_map<std::string, Unit*> name_map_cache;
        mutable std::shared_mutex name_map_cache_rw_mutex;

        std::vector<uint32_t> active_index_cache;
        mutable std::shared_mutex index_cache_rw_mutex;
        
        std::vector<std::unique_ptr<Unit>> children;
        mutable std::shared_mutex children_rw_mutex;

        // when (active_index_cache.size() / children.size() <= overload_factor)
        // children vector will realloc(call method @ref remap_children_and_cache())
        static const constexpr float overload_factor = 0.5f;

        /**
         * @brief Get the available child pointer.
         * @param index The index of the child.
         * @return Unit* 
         * If the index is valid, return the pointer to the child, otherwise return nullptr.
         */
        auto get_available_child(uint32_t index) noexcept -> Unit*;

        /**
         * @brief Get child pointer by name.
         * @param child_name The name of the child.
         * @return Unit*
         * If the child exists in name cache, return the pointer; otherwise return nullptr.
         */
        auto get_child(const std::string& child_name) noexcept -> Unit*;

        /**
         * @brief Update the index cache. It will be call substantively when @ref modified is true.
         * @return void
         */
        auto update_index_cache() -> void;

        /**
         * @brief Move away the dead child ptr in @ref children and update index cache.
         * @return void
         */
        auto remap_children_and_cache() -> void;

        /**
         * @brief Drop all children which are marked ready to drop.
         * @return void
         */
        auto drop_children() -> void;

    private:
        std::atomic<bool> process_enabled = true; // process enabled flag
        std::atomic<bool> ready_to_drop = false;  // drop flag(mark as dead)

    public:
        // iterator
        class iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = Unit;
            using difference_type = std::ptrdiff_t;
            using pointer = Unit*;
            using reference = Unit&;

        public:
            // Constructor and operators
            iterator(Unit* owner, size_t pos);
            auto operator*() const -> reference;
            auto operator->() const -> pointer;
            auto operator++() -> iterator&;
            auto operator++(int) -> iterator;
            auto operator--() -> iterator&;
            auto operator--(int) -> iterator;
            auto operator==(const iterator& other) const -> bool;
            auto operator!=(const iterator& other) const -> bool;

        private:
            Unit* m_owner;
            size_t m_pos;
            friend class Unit;
        };


        auto begin() -> iterator;
        auto end() -> iterator;

    public:
        // Next, we are going to write the const implementation of the iterator.
        class const_iterator {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = const Unit;
            using difference_type = std::ptrdiff_t;
            using pointer = const Unit*;
            using reference = const Unit&;

        public:
            // Constructor and operators
            const_iterator(const Unit* owner, size_t pos);
            auto operator*() const -> reference;
            auto operator->() const -> pointer;
            auto operator++() -> const_iterator&;
            auto operator++(int) -> const_iterator;
            auto operator--() -> const_iterator&;
            auto operator--(int) -> const_iterator;
            auto operator==(const const_iterator& other) const -> bool;
            auto operator!=(const const_iterator& other) const -> bool;

        private:
            const Unit* m_owner;
            size_t m_pos;
        };

        auto begin() const -> const_iterator;
        auto end() const -> const_iterator;

        auto cbegin() const -> const_iterator;
        auto cend() const -> const_iterator;

    public:
        // This is a reverse iterator, implemented using std::reverse_iterator.
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        auto rbegin() -> reverse_iterator;
        auto rend() -> reverse_iterator;

        auto rbegin() const -> const_reverse_iterator;
        auto rend() const -> const_reverse_iterator;

        auto crbegin() const -> const_reverse_iterator;
        auto crend() const -> const_reverse_iterator;

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

    template<IsUnitExtend Unit_T, typename F, typename... Args>
    auto Unit::with_child(uint32_t index, F&& func, Args&&... args) 
    -> std::invoke_result_t<F, Unit_T&, Args...> {
        auto* child_ptr = get_available_child(index);
        if (child_ptr == nullptr) {
            throw std::out_of_range("Child index is out of range");
        }

        auto target_child_ptr = dynamic_cast<Unit_T*>(child_ptr);
        if (target_child_ptr == nullptr) {
            throw std::invalid_argument("Failed to cast child to requested type");
        }
        return std::invoke(func, *target_child_ptr, std::forward<Args>(args)...);
    } // Unit::with_child

    template<IsUnitExtend Unit_T, typename F, typename... Args>
    auto Unit::with_child(const std::string& child_name, F&& func, Args&&... args)
    -> std::invoke_result_t<F, Unit_T&, Args...> {
        auto child_ptr = get_child(child_name);
        if (child_ptr == nullptr) {
            throw std::out_of_range("Child name is not found");
        }

        auto* child = dynamic_cast<Unit_T*>(child_ptr);
        if (child == nullptr) {
            throw std::invalid_argument("Failed to cast child to requested type");
        }
        return std::invoke(func, *child, std::forward<Args>(args)...);
    } // Unit::with_child(name)

    template<IsUnitExtend T>
    auto Unit::get_parent() noexcept -> std::optional<std::reference_wrapper<T>> {
        static_assert(std::is_base_of_v<Unit, T>, "T is not derived from Unit");
        if (parent == nullptr) return std::nullopt;
        auto cast_parent = dynamic_cast<T*>(parent);
        return cast_parent == nullptr ? std::nullopt : *cast_parent;
    } // Unit::get_parent<T>
} // namespace gkit::core::scene
