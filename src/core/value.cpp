#include "gkit/core/value.hpp"

namespace gkit::core {
    // =========================================================================
    // Value - Constructors
    // =========================================================================

    Value::Value(Null) noexcept : storage(Null{}) {}
    Value::Value(bool value) noexcept : storage(value) {}
    Value::Value(std::int64_t value) noexcept : storage(Number{value}) {}
    Value::Value(float value) noexcept : storage(Number{value}) {}
    Value::Value(const char* value) : storage(std::string(value)) {}
    Value::Value(std::string value) : storage(std::move(value)) {}
    Value::Value(Array value) : storage(std::move(value)) {}
    Value::Value(Map value) : storage(std::move(value)) {}

    Value::Value(std::unique_ptr<gkit::core::scene::Object> value) noexcept : storage(std::move(value)) {}

    Value::Value(const Value& other) {
        std::visit(
            [this](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::unique_ptr<gkit::core::scene::Object>>) {
                    throw std::logic_error("Cannot copy Value holding a non-copyable scene::Object");
                } else {
                    this->storage = v;
                }
            },
            other.storage);
    }

    // =========================================================================
    // Value - Assignment Operators
    // =========================================================================

    auto Value::operator=(const Value& other) -> Value& {
        if (this == &other) return *this;
        std::visit(
            [this](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, std::unique_ptr<gkit::core::scene::Object>>) {
                    throw std::logic_error("Cannot copy Value holding a non-copyable scene::Object");
                } else {
                    this->storage = v;
                }
            },
            other.storage);
        return *this;
    }

    // =========================================================================
    // Value - Assignment Operators
    // =========================================================================

    auto Value::operator=(Null) noexcept -> Value& {
        storage = Null{};
        return *this;
    }

    auto Value::operator=(bool value) noexcept -> Value& {
        storage = value;
        return *this;
    }

    auto Value::operator=(std::int64_t value) noexcept -> Value& {
        storage = Number{value};
        return *this;
    }

    auto Value::operator=(float value) noexcept -> Value& {
        storage = Number{value};
        return *this;
    }

    auto Value::operator=(const char* value) -> Value& {
        storage = std::string(value);
        return *this;
    }

    auto Value::operator=(std::string value) -> Value& {
        storage = std::move(value);
        return *this;
    }

    auto Value::operator=(Array value) -> Value& {
        storage = std::move(value);
        return *this;
    }

    auto Value::operator=(Map value) -> Value& {
        storage = std::move(value);
        return *this;
    }

    auto Value::operator=(std::unique_ptr<gkit::core::scene::Object> value) noexcept -> Value& {
        storage = std::move(value);
        return *this;
    }

    // =========================================================================
    // Value - Unchecked Accessors (non-constexpr - container types)
    // =========================================================================

    auto Value::as_string() const noexcept -> const std::string& {
        return std::get<std::string>(storage);
    }

    auto Value::as_array() const noexcept -> const Array& {
        return std::get<Array>(storage);
    }

    auto Value::as_map() const noexcept -> const Map& {
        return std::get<Map>(storage);
    }

    auto Value::as_array() noexcept -> Array& {
        return std::get<Array>(storage);
    }

    auto Value::as_map() noexcept -> Map& {
        return std::get<Map>(storage);
    }

    // =========================================================================
    // Value - Object Accessors
    // =========================================================================

    auto Value::as_object() -> std::unique_ptr<gkit::core::scene::Object> {
        auto ptr = std::move(std::get<std::unique_ptr<gkit::core::scene::Object>>(storage));
        storage  = Null{};
        return ptr;
    }

    // =========================================================================
    // Value - Safe Accessors with Fallback (non-constexpr - container types)
    // =========================================================================

    auto Value::as_string_or(const std::string& fallback) const noexcept -> const std::string& {
        return is_string() ? as_string() : fallback;
    }

    auto Value::as_object_or(gkit::core::scene::Object* fallback) const noexcept -> gkit::core::scene::Object* {
        return is_object() ? std::get<std::unique_ptr<gkit::core::scene::Object>>(storage).get() : fallback;
    }

    // =========================================================================
    // Value - Map/Array Helpers
    // =========================================================================

    auto Value::contains(const std::string& key) const noexcept -> bool {
        if (!is_map()) return false;
        const auto& obj = as_map();
        return obj.find(key) != obj.end();
    }

    auto Value::operator[](const std::string& key) -> Value& {
        return as_map()[key];
    }

    auto Value::operator[](const std::string& key) const -> const Value& {
        return as_map().at(key);
    }

    auto Value::operator[](std::size_t index) -> Value& {
        return as_array()[index];
    }

    auto Value::operator[](std::size_t index) const -> const Value& {
        return as_array()[index];
    }

    auto Value::at(const std::string& key) const noexcept -> std::optional<std::reference_wrapper<const Value>> {
        if (!is_map()) return std::nullopt;
        const auto& obj = as_map();
        auto it         = obj.find(key);
        if (it == obj.end()) return std::nullopt;
        return std::cref(it->second);
    }

    auto Value::at(std::size_t index) const noexcept -> std::optional<std::reference_wrapper<const Value>> {
        if (!is_array()) return std::nullopt;
        const auto& arr = as_array();
        if (index >= arr.size()) return std::nullopt;
        return std::cref(arr[index]);
    }

    auto Value::push_back(Value value) -> void {
        as_array().push_back(std::move(value));
    }
} // namespace gkit::core