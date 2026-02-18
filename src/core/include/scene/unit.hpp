#pragma once

#include <assert.h>
#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
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
    concept IsUnitExtend = std::is_base_of_v<Unit, T>;

    /**
     * class Unit
     * @brief This is the gkit application unit. It is the basic unit of program composition.
     * @note This class cannot be copied and constructed externally. If you want to create a unit,
     * you should use the function @ref gkit::scene::Unit::create<T>().
     *
     * If you want to move an Unit instance, you should move with the std::unique_ptr by std::move().
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
        auto add_child(std::unique_ptr<Unit>&& child_ptr) noexcept -> void;

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
         * It is not use in the current version.
         */
        // auto remove_child(std::unique_ptr<gkit::scene::Unit>& child_ptr) noexcept -> void; 

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
        template<typename Unit_T, typename F, typename... Args>
        auto with_child(uint32_t index, F&& func, Args&&... args)
            -> std::optional<std::invoke_result_t<F, Unit_T&, Args...>>;

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
        mutable std::shared_mutex index_cache_rw_mutex;
        std::vector<uint32_t> active_index_cache;
        mutable std::shared_mutex children_rw_mutex;
        std::vector<std::unique_ptr<Unit>> children;

        // when (active_index_cache.size() / children.size() <= overload_factor)
        // children vector will realloc(call method @ref remap_children_and_cache())
        static const constexpr float overload_factor = 0.5f;

        /**
         * @brief Get the available child pointer.
         * @param index The index of the child.
         * @return std::optional<Unit*> 
         * If the index is valid, return the pointer to the child, otherwise return std::nullopt.
         */
        auto get_available_child(uint32_t index) noexcept -> std::optional<Unit*>;

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
        std::atomic<bool> ready_to_drop  = false; // drop flag(mark as dead)
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
        static_assert(std::is_base_of_v<Unit, Unit_T>, "T is not derived from Unit");
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
        static_assert(std::is_base_of_v<Unit, T>, "T is not derived from Unit");
        if (parent == nullptr) return std::nullopt;
        auto cast_parent = dynamic_cast<T*>(parent);
        return cast_parent == nullptr ? std::nullopt : *cast_parent;
    } // Unit::get_parent<T>

    /**
     * @class UnitIterator
     * @brief Read-write iterator for traversing active children of a Unit
     * 
     * This iterator provides both read and write access to Unit children.
     * It uses only public Unit methods (get_available_child) to access children.
     * Automatically skips deleted children.
     * 
     * Usage:
     * @code
     * auto view = iterate_children(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     it->name = "new_name";  // Write access
     *     std::cout << it->name;   // Read access
     * }
     * @endcode
     */
    class UnitIterator {
    private:
        Unit* parent;
        size_t current_index;
        std::optional<Unit*> cached_child;

        /**
         * @brief Get child at current index using public Unit method
         */
        void cache_current_child() {
            if (!parent) {
                cached_child = std::nullopt;
                return;
            }
            // Use with_child to access the child through public interface
            auto result = parent->with_child<Unit>(
                static_cast<uint32_t>(current_index),
                [](Unit& unit) -> Unit* { return &unit; }
            );
            cached_child = result;
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Unit;
        using difference_type = std::ptrdiff_t;
        using pointer = Unit*;
        using reference = Unit&;

        /**
         * @brief Construct an iterator
         * @param parent Pointer to the parent Unit
         * @param index Starting index
         */
        UnitIterator(Unit* parent, size_t index = 0) 
            : parent(parent), current_index(index) {
            cache_current_child();
        }

        /**
         * @brief Dereference operator - provides read-write access
         * @return Reference to the current Unit
         * @throws std::runtime_error if iterator is out of range or invalid
         */
        reference operator*() const {
            if (!cached_child.has_value() || !cached_child.value()) {
                throw std::runtime_error("UnitIterator out of range or pointing to null");
            }
            return *(cached_child.value());
        }

        /**
         * @brief Prefix increment operator
         */
        UnitIterator& operator++() {
            ++current_index;
            cache_current_child();
            return *this;
        }

        /**
         * @brief Postfix increment operator
         */
        UnitIterator operator++(int) {
            UnitIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * @brief Equality comparison
         *
         * Iterators are equal if:
         * - Both have nullptr parent (both are end iterators)
         * - Both have no cached child (both are end iterators)
         * - Both have the same parent and index
         */
        friend bool operator==(const UnitIterator& a, const UnitIterator& b) {
            // Both are end iterators (nullptr parent or no cached child)
            bool a_is_end = (!a.parent) || (!a.cached_child.has_value());
            bool b_is_end = (!b.parent) || (!b.cached_child.has_value());

            if (a_is_end && b_is_end) {
                return true;
            }
            // Only one is end iterator
            if (a_is_end || b_is_end) {
                return false;
            }
            // Same parent and index
            return a.parent == b.parent && a.current_index == b.current_index;
        }

        /**
         * @brief Inequality comparison
         */
        friend bool operator!=(const UnitIterator& a, const UnitIterator& b) {
            return !(a == b);
        }
    };

    /**
     * @class UnitConstIterator
     * @brief Read-only iterator for traversing active children of a Unit
     *
     * This iterator provides read-only access to Unit children.
     * It uses only public Unit methods (get_available_child) to access children.
     * Automatically skips deleted children.
     *
     * Usage:
     * @code
     * auto view = iterate_children_const(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     std::cout << it->name;  // Read access only
     *     // it->name = "new";  // Compilation error - read-only
     * }
     * @endcode
     */
    class UnitConstIterator {
    private:
        const Unit* parent;
        size_t current_index;
        std::optional<const Unit*> cached_child;

        /**
         * @brief Get child at current index using public Unit method
         */
        void cache_current_child() {
            if (!parent) {
                cached_child = std::nullopt;
                return;
            }
            // Use with_child to access the child through public interface
            auto result = const_cast<Unit*>(parent)->with_child<Unit>(
                static_cast<uint32_t>(current_index),
                [](Unit& unit) -> const Unit* { return &unit; }
            );
            cached_child = result;
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Unit;
        using difference_type = std::ptrdiff_t;
        using pointer = const Unit*;
        using reference = const Unit&;

        /**
         * @brief Construct an iterator
         * @param parent Pointer to the parent Unit
         * @param index Starting index
         */
        UnitConstIterator(const Unit* parent, size_t index = 0)
            : parent(parent), current_index(index) {
            cache_current_child();
        }

        /**
         * @brief Dereference operator - provides read-only access
         * @return Reference to the current Unit
         * @throws std::runtime_error if iterator is out of range or invalid
         */
        reference operator*() const {
            if (!cached_child.has_value() || !cached_child.value()) {
                throw std::runtime_error("UnitConstIterator out of range or pointing to null");
            }
            return *(cached_child.value());
        }

        /**
         * @brief Arrow operator - provides read-only access
         * @return Pointer to the current Unit
         */
        pointer operator->() const {
            return &operator*();
        }

        /**
         * @brief Prefix increment operator
         */
        UnitConstIterator& operator++() {
            ++current_index;
            cache_current_child();
            return *this;
        }

        /**
         * @brief Postfix increment operator
         */
        UnitConstIterator operator++(int) {
            UnitConstIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * @brief Equality comparison
         *
         * Iterators are equal if:
         * - Both have nullptr parent (both are end iterators)
         * - Both have no cached child (both are end iterators)
         * - Both have the same parent and index
         */
        friend bool operator==(const UnitConstIterator& a, const UnitConstIterator& b) {
            // Both are end iterators (nullptr parent or no cached child)
            bool a_is_end = (!a.parent) || (!a.cached_child.has_value());
            bool b_is_end = (!b.parent) || (!b.cached_child.has_value());

            if (a_is_end && b_is_end) {
                return true;
            }
            // Only one is end iterator
            if (a_is_end || b_is_end) {
                return false;
            }
            // Same parent and index
            return a.parent == b.parent && a.current_index == b.current_index;
        }

        /**
         * @brief Inequality comparison
         */
        friend bool operator!=(const UnitConstIterator& a, const UnitConstIterator& b) {
            return !(a == b);
        }
    };

    /**
     * @class ConstUnitIterator
     * @brief Read-only iterator for traversing active children of a Unit
     *
     * This iterator provides only read access to Unit children.
     * It automatically handles the active_index_cache to skip deleted children.
     * Thread-safe by using shared_lock during iteration.
     *
     * Usage:
     * @code
     * auto view = iterate_children(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     std::cout << it->name;   // Read access only
     *     // it->name = "new";    // Compilation error: cannot assign to const reference
     * }
     * @endcode
     */
    class ConstUnitIterator {
    private:
        const Unit* parent;
        size_t current_index;
        std::optional<const Unit*> cached_child;

        /**
         * @brief Get child at current index using public Unit method
         */
        void cache_current_child() {
            if (!parent) {
                cached_child = std::nullopt;
                return;
            }
            // Use with_child to access the child through public interface
            auto result = const_cast<Unit*>(parent)->with_child<Unit>(
                static_cast<uint32_t>(current_index),
                [](Unit& unit) -> const Unit* { return &unit; }
            );
            cached_child = result;
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Unit;
        using difference_type = std::ptrdiff_t;
        using pointer = const Unit*;
        using reference = const Unit&;

        /**
         * @brief Construct an iterator
         * @param parent Pointer to the parent Unit
         * @param index Starting index
         */
        ConstUnitIterator(const Unit* parent, size_t index = 0)
            : parent(parent), current_index(index) {
            cache_current_child();
        }

        /**
         * @brief Dereference operator - provides read-only access
         * @return Const reference to the current Unit
         * @throws std::runtime_error if iterator is out of range or invalid
         */
        reference operator*() const {
            if (!cached_child.has_value() || !cached_child.value()) {
                throw std::runtime_error("ConstUnitIterator out of range or pointing to null");
            }
            return *(cached_child.value());
        }

        /**
         * @brief Arrow operator - provides read-only access
         * @return Const pointer to the current Unit
         */
        pointer operator->() const {
            return &operator*();
        }

        /**
         * @brief Prefix increment operator
         */
        ConstUnitIterator& operator++() {
            ++current_index;
            cache_current_child();
            return *this;
        }

        /**
         * @brief Postfix increment operator
         */
        ConstUnitIterator operator++(int) {
            ConstUnitIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * @brief Equality comparison
         *
         * Iterators are equal if:
         * - Both have nullptr parent (both are end iterators)
         * - Both have no cached child (both are end iterators)
         * - Both have the same parent and index
         */
        friend bool operator==(const ConstUnitIterator& a, const ConstUnitIterator& b) {
            // Both are end iterators (nullptr parent or no cached child)
            bool a_is_end = (!a.parent) || (!a.cached_child.has_value());
            bool b_is_end = (!b.parent) || (!b.cached_child.has_value());

            if (a_is_end && b_is_end) {
                return true;
            }
            // Only one is end iterator
            if (a_is_end || b_is_end) {
                return false;
            }
            // Same parent and index
            return a.parent == b.parent && a.current_index == b.current_index;
        }

        /**
         * @brief Inequality comparison
         */
        friend bool operator!=(const ConstUnitIterator& a, const ConstUnitIterator& b) {
            return !(a == b);
        }
    };

    /**
     * @class UnitChildrenView
     * @brief Helper class to provide read-only iterator-like access to Unit children
     *
     * This class provides begin() and end() methods for range-based iteration.
     * The end iterator is identified by having no cached child (out of range).
     * This is a read-only view that prevents modification of children.
     *
     * Usage:
     * @code
     * auto view = iterate_children(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     std::cout << it->name;   // Read access only
     * }
     * @endcode
     *
     * For read-write access, use UnitIterator directly:
     * @code
     * for (auto it = UnitIterator(my_unit, 0); it != UnitIterator(nullptr, 0); ++it) {
     *     it->name = "new_name";  // Write access
     * }
     * @endcode
     */
    class UnitChildrenView {
    private:
        const Unit* parent;
    public:
        UnitChildrenView(const Unit* p) : parent(p) {}

        /**
         * @brief Get read-only iterator to the first active child
         */
        ConstUnitIterator begin() const {
            return ConstUnitIterator(parent, 0);
        }

        /**
         * @brief Get read-only iterator to one past the last active child
         *
         * This creates an end sentinel iterator with nullptr parent.
         * Comparison logic handles this specially to mark the end.
         */
        ConstUnitIterator end() const {
            return ConstUnitIterator(nullptr, 0);
        }
    };

    /**
     * @class MutableUnitChildrenView
     * @brief Helper class to provide read-write iterator-like access to Unit children
     *
     * This class provides begin() and end() methods for range-based iteration.
     * The end iterator is identified by having no cached child (out of range).
     * This is a mutable view that allows modification of children.
     *
     * Usage:
     * @code
     * auto view = iterate_children_mut(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     it->name = "new_name";  // Write access
     * }
     * @endcode
     *
     * For read-only access, use UnitChildrenView or iterate_children().
     */
    class MutableUnitChildrenView {
    private:
        Unit* parent;
    public:
        MutableUnitChildrenView(Unit* p) : parent(p) {}

        /**
         * @brief Get read-write iterator to the first active child
         */
        UnitIterator begin() {
            return UnitIterator(parent, 0);
        }

        /**
         * @brief Get read-write iterator to one past the last active child
         *
         * This creates an end sentinel iterator with nullptr parent.
         * Comparison logic handles this specially to mark the end.
         */
        UnitIterator end() {
            return UnitIterator(nullptr, 0);
        }
    };

    /**
     * @brief Create a read-only view for iterating over active children
     * @param unit The unit to iterate over
     * @return UnitChildrenView that provides begin/end iterators (read-only)
     *
     * This free function provides a convenient way to access children iterators.
     * The iterator provides only read access to Unit children.
     *
     * Usage example:
     * @code
     * for (auto it = iterate_children(my_unit).begin();
     *      it != iterate_children(my_unit).end(); ++it) {
     *     std::cout << it->name;   // Read access only
     * }
     *
     * // Or with view capture
     * auto view = iterate_children(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     std::cout << it->name;   // Read access only
     * }
     * @endcode
     *
     * For read-write access, use iterate_children_mut() instead.
     */
    inline auto iterate_children(const Unit* unit) -> UnitChildrenView {
        return UnitChildrenView(unit);
    }

    /**
     * @brief Create a read-only view for iterating over active children - reference overload
     * @param unit The unit to iterate over
     * @return UnitChildrenView that provides begin/end iterators (read-only)
     *
     * This overload accepts a reference instead of a pointer for convenience.
     *
     * Usage example:
     * @code
     * for (auto it = iterate_children(my_unit).begin();
     *      it != iterate_children(my_unit).end(); ++it) {
     *     std::cout << it->name;   // Read access only
     * }
     * @endcode
     */
    inline auto iterate_children(const Unit& unit) -> UnitChildrenView {
        return UnitChildrenView(&unit);
    }

    /**
     * @brief Create a mutable view for iterating over active children (read-write)
     * @param unit The unit to iterate over
     * @return MutableUnitChildrenView that provides begin/end iterators (read-write)
     *
     * This free function provides a convenient way to access mutable children iterators.
     * The iterator provides both read and write access to Unit children.
     *
     * Usage example:
     * @code
     * for (auto it = iterate_children_mut(my_unit).begin();
     *      it != iterate_children_mut(my_unit).end(); ++it) {
     *     it->name = "new_name";  // Write access
     *     std::cout << it->name;   // Read access
     * }
     *
     * // Or with view capture
     * auto view = iterate_children_mut(my_unit);
     * for (auto it = view.begin(); it != view.end(); ++it) {
     *     it->drop();  // Write access
     * }
     * @endcode
     */
    inline auto iterate_children_mut(Unit* unit) -> MutableUnitChildrenView {
        return MutableUnitChildrenView(unit);
    }

    /**
     * @brief Create a mutable view for iterating over active children (read-write) - reference overload
     * @param unit The unit to iterate over
     * @return MutableUnitChildrenView that provides begin/end iterators (read-write)
     *
     * This overload accepts a reference instead of a pointer for convenience.
     *
     * Usage example:
     * @code
     * for (auto it = iterate_children_mut(my_unit).begin();
     *      it != iterate_children_mut(my_unit).end(); ++it) {
     *     it->name = "new_name";
     * }
     * @endcode
     */
    inline auto iterate_children_mut(Unit& unit) -> MutableUnitChildrenView {
        return MutableUnitChildrenView(&unit);
    }

} // namespace gkit::scene