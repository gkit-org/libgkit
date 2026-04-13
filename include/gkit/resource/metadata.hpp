#pragma once

#include <cstdint>
#include <exception>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace gkit::resource::metadata {

    /**
     * @brief Forward declaration of the Value type
     */
    class Value;

    /**
     * @brief JSON value types enumeration
     */
    enum class Type : std::uint8_t {
        Null,
        Boolean,
        Number,
        String,
        Array,
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
     * @brief JSON object type - string-keyed map of values
     * @note Uses std::map to preserve key ordering for deterministic serialization
     */
    using Object = std::map<std::string, Value>;

    /**
     * @brief JSON number type - holds either integer or floating-point
     */
    using Number = std::variant<std::int64_t, double>;

    /**
     * @brief The core Value type representing any JSON value
     */
    class Value final {
    public:
        /**
         * @brief Underlying variant type holding all JSON value types
         */
        using Storage = std::variant<Null, bool, Number, std::string, Array, Object>;

        Value() noexcept = default;
        Value(const Value&) = default;
        Value(Value&&) noexcept = default;
        ~Value() = default;

        /** Convenience constructors for each JSON type **/
        Value(Null) noexcept;
        Value(bool value) noexcept;
        Value(std::int64_t value) noexcept;
        Value(double value) noexcept;
        Value(int value) noexcept : Value(static_cast<std::int64_t>(value)) {}
        Value(const char* value);
        Value(std::string value);
        Value(Array value);
        Value(Object value);

        /** Assignment operators **/
        auto operator=(const Value&) -> Value& = default;
        auto operator=(Value&&) noexcept -> Value& = default;

        auto operator=(Null) noexcept -> Value&;
        auto operator=(bool value) noexcept -> Value&;
        auto operator=(std::int64_t value) noexcept -> Value&;
        auto operator=(int value) noexcept -> Value& { return *this = static_cast<std::int64_t>(value); }
        auto operator=(double value) noexcept -> Value&;
        auto operator=(const char* value) -> Value&;
        auto operator=(std::string value) -> Value&;
        auto operator=(Array value) -> Value&;
        auto operator=(Object value) -> Value&;

    public: // Type checking
        [[nodiscard]] constexpr auto is_null() const noexcept -> bool {
            return std::holds_alternative<Null>(storage_);
        }
        [[nodiscard]] constexpr auto is_bool() const noexcept -> bool {
            return std::holds_alternative<bool>(storage_);
        }
        [[nodiscard]] constexpr auto is_number() const noexcept -> bool {
            return std::holds_alternative<Number>(storage_);
        }
        [[nodiscard]] constexpr auto is_number_integer() const noexcept -> bool {
            if (!is_number()) return false;
            return std::holds_alternative<std::int64_t>(std::get<Number>(storage_));
        }
        [[nodiscard]] constexpr auto is_number_float() const noexcept -> bool {
            if (!is_number()) return false;
            return std::holds_alternative<double>(std::get<Number>(storage_));
        }
        [[nodiscard]] constexpr auto is_string() const noexcept -> bool {
            return std::holds_alternative<std::string>(storage_);
        }
        [[nodiscard]] constexpr auto is_array() const noexcept -> bool {
            return std::holds_alternative<Array>(storage_);
        }
        [[nodiscard]] constexpr auto is_object() const noexcept -> bool {
            return std::holds_alternative<Object>(storage_);
        }

    public: // Value accessors (unchecked - behavior undefined if wrong type)
        [[nodiscard]] constexpr auto as_bool() const noexcept -> bool {
            return std::get<bool>(storage_);
        }
        [[nodiscard]] constexpr auto as_int64() const noexcept -> std::int64_t {
            const auto& num = std::get<Number>(storage_);
            return std::get<std::int64_t>(num);
        }
        [[nodiscard]] constexpr auto as_double() const noexcept -> double {
            const auto& num = std::get<Number>(storage_);
            if (std::holds_alternative<double>(num)) {
                return std::get<double>(num);
            }
            return static_cast<double>(std::get<std::int64_t>(num));
        }
        [[nodiscard]] auto as_string() const noexcept -> const std::string&;
        [[nodiscard]] auto as_array() const noexcept -> const Array&;
        [[nodiscard]] auto as_object() const noexcept -> const Object&;

        /** Mutable accessors **/
        [[nodiscard]] auto as_array() noexcept -> Array&;
        [[nodiscard]] auto as_object() noexcept -> Object&;

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

    public: // Object helpers
        /**
         * @brief Check if object contains a key
         * @return false if not an object or key not found
         */
        [[nodiscard]] auto contains(const std::string& key) const noexcept -> bool;

        /**
         * @brief Access object member (unchecked)
         * @pre is_object() must be true
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
         * @brief Get object member if it exists
         * @return std::nullopt if not an object or key not found
         */
        [[nodiscard]] auto at(const std::string& key) const noexcept -> std::optional<std::reference_wrapper<const Value>>;
        [[nodiscard]] auto at(std::size_t index) const noexcept -> std::optional<std::reference_wrapper<const Value>>;

    public: // Array/Object modification helpers
        auto push_back(Value value) -> void;
        template<typename... Args>
        auto emplace_back(Args&&... args) -> void {
            as_array().emplace_back(std::forward<Args>(args)...);
        }

    public:
        [[nodiscard]] constexpr auto type() const noexcept -> Type {
            return std::visit([](const auto& v) -> Type {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Null>) return Type::Null;
                if constexpr (std::is_same_v<T, bool>) return Type::Boolean;
                if constexpr (std::is_same_v<T, Number>) return Type::Number;
                if constexpr (std::is_same_v<T, std::string>) return Type::String;
                if constexpr (std::is_same_v<T, Array>) return Type::Array;
                if constexpr (std::is_same_v<T, Object>) return Type::Object;
                return Type::Null; // unreachable
            }, storage_);
        }

        /**
         * @brief Direct access to underlying storage
         * @note For advanced use cases (visitation, etc.)
         */
        [[nodiscard]] constexpr auto raw() noexcept -> Storage& {
            return storage_;
        }
        [[nodiscard]] constexpr auto raw() const noexcept -> const Storage& {
            return storage_;
        }

    private:
        Storage storage_ = Null{};
    };

    /**
     * @brief Parse error exception type
     */
    class ParseError : public std::exception {
    public:
        explicit ParseError(const std::string& message, std::size_t line = 0, std::size_t column = 0);
        [[nodiscard]] auto what() const noexcept -> const char* override;
        [[nodiscard]] auto line() const noexcept -> std::size_t;
        [[nodiscard]] auto column() const noexcept -> std::size_t;

    private:
        std::string message_;
        std::size_t line_;
        std::size_t column_;
    };

    /**
     * @brief Serialization format options
     */
    struct FormatOptions {
        bool pretty = false;           ///< Pretty-print with indentation
        std::uint8_t indent_size = 4;  ///< Number of spaces per indent level (when pretty)
        bool escape_unicode = false;   ///< Escape non-ASCII characters as \uXXXX
    };

    /**
     * @brief Deserialize a JSON string into a Value
     * @param json The JSON string to parse
     * @return The parsed Value
     * @throws ParseError if the input is not valid JSON
     */
    [[nodiscard]] auto parse(std::string_view json) -> Value;

    /**
     * @brief Try to deserialize a JSON string (non-throwing)
     * @return std::nullopt on parse failure instead of throwing
     */
    [[nodiscard]] auto try_parse(std::string_view json) noexcept -> std::optional<Value>;

    /**
     * @brief Serialize a Value to a JSON string
     * @param value The value to serialize
     * @param options Formatting options
     * @return The JSON string representation
     */
    [[nodiscard]] auto serialize(const Value& value, const FormatOptions& options = {}) -> std::string;

    /**
     * @brief Serialize a Value to a JSON string with pretty formatting
     * @param value The value to serialize
     * @param indent_size Number of spaces per indent level
     * @return The pretty-printed JSON string
     */
    [[nodiscard]] auto serialize_pretty(const Value& value, std::uint8_t indent_size = 4) -> std::string;

} // namespace gkit::resource::metadata
