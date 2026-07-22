#pragma once

#include "gkit/core/scene/object.hpp"

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace gkit::core {
    /**
     * @brief Forward declaration of the Value type
     */
    class Value;

    /**
     * @brief JSON value types enumeration
     */
    enum class Type : std::uint8_t {
        Null,
        Bool,
        Number,
        String,
        Array,
        Map,
        Object
    };

    /**
     * @brief JSON null type
     */
    struct Null {};

    /**
     * @brief JSON array type - ordered collection of values
     */
    using Array = std::vector<Value>;

    /**
     * @brief JSON number type - holds either integer or floating-point
     */
    using Number = std::variant<std::int64_t, double>;

    using String = std::string;

    /**
     * @brief JSON Map type - string-keyed map of values
     * @note Uses std::map to preserve key ordering for deterministic serialization
     */
    using Map = std::map<std::string, Value>;

    /**
     * @brief Dynamically typed value container capable of holding null, bool,
     *        number (int64/double), string, array, map, or a scene::Object（abd Object extended）.
     */
    class Value final {
    public:
        /**
         * @brief Variant storage backing the Value. Object is held via std::unique_ptr
         *        for exclusive ownership and automatic lifetime management.
         */
        using Storage =
            std::variant<Null, bool, Number, std::string, Array, Map, std::unique_ptr<gkit::core::scene::Object>>;

    public: // Constructors
        Value() noexcept = default;
        /// @brief Copy constructor. Throws std::logic_error if the source holds a non-copyable Object.
        Value(const Value&);
        Value(Value&&) noexcept = default;
        ~Value()                = default;

        explicit Value(Null) noexcept;
        explicit Value(bool value) noexcept;
        explicit Value(std::int64_t value) noexcept;
        explicit Value(double value) noexcept;
        explicit Value(int value) noexcept : Value(static_cast<std::int64_t>(value)) {}
        explicit Value(const char* value);
        explicit Value(std::string value);
        Value(Array value); // NOLINT(google-explicit-constructor)
        Value(Map value); // NOLINT(google-explicit-constructor)

        /** @brief Store an Object by unique_ptr (the Value takes exclusive ownership). */
        explicit Value(std::unique_ptr<gkit::core::scene::Object> value) noexcept;

        /**  @brief Copy-construct a Value from a derived Object type. Allocates a new copy on the heap. */
        template<gkit::core::scene::IsObject T>
        explicit Value(const T& value);

    public: // Assignment operators
        auto operator=(const Value&) -> Value&;
        auto operator=(Value&&) noexcept -> Value& = default;

        auto operator=(Null) noexcept -> Value&;
        auto operator=(bool value) noexcept -> Value&;
        auto operator=(std::int64_t value) noexcept -> Value&;
        auto operator=(int value) noexcept -> Value& { return *this = static_cast<std::int64_t>(value); }
        auto operator=(double value) noexcept -> Value&;
        auto operator=(const char* value) -> Value&;
        auto operator=(std::string value) -> Value&;
        auto operator=(Array value) -> Value&;
        auto operator=(Map value) -> Value&;

        /** @brief Assign a unique_ptr, taking ownership of the Object. */ 
        auto operator=(std::unique_ptr<gkit::core::scene::Object> value) noexcept -> Value&;

    public: // Type checking
        [[nodiscard]] constexpr auto is_null() const noexcept -> bool { return std::holds_alternative<Null>(storage); }
        [[nodiscard]] constexpr auto is_bool() const noexcept -> bool { return std::holds_alternative<bool>(storage); }
        [[nodiscard]] constexpr auto is_number() const noexcept -> bool {
            return std::holds_alternative<Number>(storage);
        }
        [[nodiscard]] constexpr auto is_number_integer() const noexcept -> bool {
            if (!is_number()) return false;
            return std::holds_alternative<std::int64_t>(std::get<Number>(storage));
        }
        [[nodiscard]] constexpr auto is_number_float() const noexcept -> bool {
            if (!is_number()) return false;
            return std::holds_alternative<double>(std::get<Number>(storage));
        }
        [[nodiscard]] constexpr auto is_string() const noexcept -> bool {
            return std::holds_alternative<std::string>(storage);
        }
        [[nodiscard]] constexpr auto is_array() const noexcept -> bool {
            return std::holds_alternative<Array>(storage);
        }
        [[nodiscard]] constexpr auto is_map() const noexcept -> bool { return std::holds_alternative<Map>(storage); }
        [[nodiscard]] constexpr auto is_object() const noexcept -> bool {
            return std::holds_alternative<std::unique_ptr<gkit::core::scene::Object>>(storage);
        }

    public: // Value accessors (unchecked - behavior undefined if wrong type)
        [[nodiscard]] constexpr auto as_bool() const noexcept -> bool { return std::get<bool>(storage); }
        [[nodiscard]] constexpr auto as_int64() const noexcept -> std::int64_t {
            const auto& num = std::get<Number>(storage);
            return std::get<std::int64_t>(num);
        }
        [[nodiscard]] constexpr auto as_double() const noexcept -> double {
            const auto& num = std::get<Number>(storage);
            if (std::holds_alternative<double>(num)) {
                return std::get<double>(num);
            }
            return static_cast<double>(std::get<std::int64_t>(num));
        }
        [[nodiscard]] auto as_string() const noexcept -> const std::string&;
        [[nodiscard]] auto as_array() const noexcept -> const Array&;
        [[nodiscard]] auto as_map() const noexcept -> const Map&;

        /** Mutable accessors **/
        [[nodiscard]] auto as_array() noexcept -> Array&;
        [[nodiscard]] auto as_map() noexcept -> Map&;

    public: // Object accessors
        /**
         * @brief Release the stored Object and transfer ownership to the caller.
         *        The Value becomes Null after this call.
         * @pre is_object() must be true; otherwise throws std::bad_variant_access.
         */
        [[nodiscard]] auto as_object() -> std::unique_ptr<gkit::core::scene::Object>;

        /**
         * @brief Access the stored Object by reference, downcasting to T via dynamic_cast.
         * @throws std::invalid_argument if the stored Object is not of type T.
         */
        template<gkit::core::scene::IsObject T>
        [[nodiscard]] auto as() -> T&;

        /**
         * @brief Const version of as<T>().
         */
        template<gkit::core::scene::IsObject T>
        [[nodiscard]] auto as() const -> const T&;

        /**
         * @brief Try to access the stored Object as type T. Returns nullptr on mismatch
         *        or if the Value does not hold an Object.
         */
        template<gkit::core::scene::IsObject T>
        [[nodiscard]] auto try_as() noexcept -> T*;

        /**
         * @brief Const version of try_as<T>().
         */
        template<gkit::core::scene::IsObject T>
        [[nodiscard]] auto try_as() const noexcept -> const T*;

    public: // Safe value accessors with fallback
        [[nodiscard]] constexpr auto as_bool_or(bool fallback) const noexcept -> bool {
            return is_bool() ? as_bool() : fallback;
        }
        [[nodiscard]] constexpr auto as_int64_or(std::int64_t fallback) const noexcept -> std::int64_t {
            return is_number_integer() ? as_int64() : fallback;
        }
        [[nodiscard]] constexpr auto as_double_or(double fallback) const noexcept -> double {
            return is_number() ? as_double() : fallback;
        }
        [[nodiscard]] auto as_string_or(const std::string& fallback) const noexcept -> const std::string&;

        /**
         * @brief Return the stored Object pointer, or the given fallback if the Value
         *        does not hold an Object. Does not transfer ownership.
         */
        [[nodiscard]] auto as_object_or(gkit::core::scene::Object* fallback) const noexcept
            -> gkit::core::scene::Object*;

    public: // Map helpers
        /**
         * @brief Check if Map contains a key
         * @return false if not an Map or key not found
         */
        [[nodiscard]] auto contains(const std::string& key) const noexcept -> bool;

        /**
         * @brief Access Map member (unchecked)
         * @pre is_Map() must be true
         */
        [[nodiscard]] auto operator[](const std::string& key) -> Value&;
        [[nodiscard]] auto operator[](const std::string& key) const -> const Value&;

        /**
         * @brief Access array element (unchecked)
         * @pre is_array() must be true
         */
        [[nodiscard]] auto operator[](std::size_t index) -> Value&;
        [[nodiscard]] auto operator[](std::size_t index) const -> const Value&;

        /**
         * @brief Get Map member if it exists
         * @return std::nullopt if not an Map or key not found
         */
        [[nodiscard]] auto at(const std::string& key) const noexcept
            -> std::optional<std::reference_wrapper<const Value>>;
        [[nodiscard]] auto at(std::size_t index) const noexcept -> std::optional<std::reference_wrapper<const Value>>;

    public: // Array/Map modification helpers
        auto push_back(Value value) -> void;
        template<typename... Args>
        auto emplace_back(Args&&... args) -> void {
            as_array().emplace_back(std::forward<Args>(args)...);
        }

    public:
        [[nodiscard]] constexpr auto type() const noexcept -> Type {
            return std::visit(
                [](const auto& v) -> Type {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, Null>) return Type::Null;
                    if constexpr (std::is_same_v<T, bool>) return Type::Bool;
                    if constexpr (std::is_same_v<T, Number>) return Type::Number;
                    if constexpr (std::is_same_v<T, std::string>) return Type::String;
                    if constexpr (std::is_same_v<T, Array>) return Type::Array;
                    if constexpr (std::is_same_v<T, Map>) return Type::Map;
                    if constexpr (std::is_same_v<T, std::unique_ptr<gkit::core::scene::Object>>) return Type::Object;
                    return Type::Null; // unreachable
                },
                storage);
        }

        /**
         * @brief Direct access to underlying storage
         * @note For advanced use cases (visitation, etc.)
         */
        [[nodiscard]] constexpr auto raw() noexcept -> Storage& { return storage; }
        [[nodiscard]] constexpr auto raw() const noexcept -> const Storage& { return storage; }

    private:
        Storage storage = Null{};
    };

    template<gkit::core::scene::IsObject T>
    Value::Value(const T& value) {
        this->storage = std::make_unique<T>(value);
    }

    template<gkit::core::scene::IsObject T>
    auto Value::as() -> T& {
        auto* ptr  = std::get<std::unique_ptr<gkit::core::scene::Object>>(this->storage).get();
        auto* cast = dynamic_cast<T*>(ptr);
        if (cast == nullptr) {
            throw std::invalid_argument("Value::as<T>() failed: stored object is not of requested type");
        }
        return *cast;
    }

    template<gkit::core::scene::IsObject T>
    auto Value::as() const -> const T& {
        const auto* ptr  = std::get<std::unique_ptr<gkit::core::scene::Object>>(this->storage).get();
        const auto* cast = dynamic_cast<const T*>(ptr);
        if (cast == nullptr) {
            throw std::invalid_argument("Value::as<T>() const failed: stored object is not of requested type");
        }
        return *cast;
    }

    template<gkit::core::scene::IsObject T>
    auto Value::try_as() noexcept -> T* {
        if (!this->is_object()) return nullptr;
        auto* ptr = std::get<std::unique_ptr<gkit::core::scene::Object>>(this->storage).get();
        return dynamic_cast<T*>(ptr);
    }

    template<gkit::core::scene::IsObject T>
    auto Value::try_as() const noexcept -> const T* {
        if (!this->is_object()) return nullptr;
        const auto* ptr = std::get<std::unique_ptr<gkit::core::scene::Object>>(this->storage).get();
        return dynamic_cast<const T*>(ptr);
    }
} // namespace gkit::core