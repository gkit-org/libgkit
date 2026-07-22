// #pragma once

// #include <cstdint>
// #include <exception>
// #include <string_view>

// namespace gkit::resource::metadata {

//     /**
//      * @brief Parse error exception type
//      */
//     class ParseError : public std::exception {
//     public:
//         explicit ParseError(const std::string& message, std::size_t line = 0, std::size_t column = 0);
//         [[nodiscard]] auto what() const noexcept -> const char* override;
//         [[nodiscard]] auto get_line() const noexcept -> std::size_t;
//         [[nodiscard]] auto get_column() const noexcept -> std::size_t;

//     private:
//         std::string message;
//         std::size_t line;
//         std::size_t column;
//     };

//     /**
//      * @brief Serialization format options
//      */
//     struct FormatOptions {
//         bool pretty              = false; ///< Pretty-print with indentation
//         std::uint8_t indent_size = 4; ///< Number of spaces per indent level (when pretty)
//         bool escape_unicode      = false; ///< Escape non-ASCII characters as \uXXXX
//     };

//     /**
//      * @brief Deserialize a JSON string into a Value
//      * @param json The JSON string to parse
//      * @return The parsed Value
//      * @throws ParseError if the input is not valid JSON
//      */
//     [[nodiscard]] auto parse(std::string_view json) -> Value;

//     /**
//      * @brief Try to deserialize a JSON string (non-throwing)
//      * @return std::nullopt on parse failure instead of throwing
//      */
//     [[nodiscard]] auto try_parse(std::string_view json) noexcept -> std::optional<Value>;

//     /**
//      * @brief Serialize a Value to a JSON string
//      * @param value The value to serialize
//      * @param options Formatting options
//      * @return The JSON string representation
//      */
//     [[nodiscard]] auto serialize(const Value& value, const FormatOptions& options = {}) -> std::string;

//     /**
//      * @brief Serialize a Value to a JSON string with pretty formatting
//      * @param value The value to serialize
//      * @param indent_size Number of spaces per indent level
//      * @return The pretty-printed JSON string
//      */
//     [[nodiscard]] auto serialize_pretty(const Value& value, std::uint8_t indent_size = 4) -> std::string;

// } // namespace gkit::resource::metadata
