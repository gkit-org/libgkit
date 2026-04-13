#include "gkit/resource/metadata.hpp"

#include <iomanip>
#include <optional>
#include <sstream>

namespace gkit::resource::metadata {

    // =========================================================================
    // Value - Constructors
    // =========================================================================

    Value::Value(Null) noexcept : storage_(Null{}) {}
    Value::Value(bool value) noexcept : storage_(value) {}
    Value::Value(std::int64_t value) noexcept : storage_(Number{value}) {}
    Value::Value(double value) noexcept : storage_(Number{value}) {}
    Value::Value(const char* value) : storage_(std::string(value)) {}
    Value::Value(std::string value) : storage_(std::move(value)) {}
    Value::Value(Array value) : storage_(std::move(value)) {}
    Value::Value(Object value) : storage_(std::move(value)) {}

    // =========================================================================
    // Value - Assignment Operators
    // =========================================================================

    auto Value::operator=(Null) noexcept -> Value& {
        storage_ = Null{};
        return *this;
    }

    auto Value::operator=(bool value) noexcept -> Value& {
        storage_ = value;
        return *this;
    }

    auto Value::operator=(std::int64_t value) noexcept -> Value& {
        storage_ = Number{value};
        return *this;
    }

    auto Value::operator=(double value) noexcept -> Value& {
        storage_ = Number{value};
        return *this;
    }

    auto Value::operator=(const char* value) -> Value& {
        storage_ = std::string(value);
        return *this;
    }

    auto Value::operator=(std::string value) -> Value& {
        storage_ = std::move(value);
        return *this;
    }

    auto Value::operator=(Array value) -> Value& {
        storage_ = std::move(value);
        return *this;
    }

    auto Value::operator=(Object value) -> Value& {
        storage_ = std::move(value);
        return *this;
    }

    // =========================================================================
    // Value - Unchecked Accessors (non-constexpr - container types)
    // =========================================================================

    auto Value::as_string() const noexcept -> const std::string& {
        return std::get<std::string>(storage_);
    }

    auto Value::as_array() const noexcept -> const Array& {
        return std::get<Array>(storage_);
    }

    auto Value::as_object() const noexcept -> const Object& {
        return std::get<Object>(storage_);
    }

    auto Value::as_array() noexcept -> Array& {
        return std::get<Array>(storage_);
    }

    auto Value::as_object() noexcept -> Object& {
        return std::get<Object>(storage_);
    }

    // =========================================================================
    // Value - Safe Accessors with Fallback (non-constexpr - container types)
    // =========================================================================

    auto Value::as_string_or(const std::string& fallback) const noexcept -> const std::string& {
        return is_string() ? as_string() : fallback;
    }

    // =========================================================================
    // Value - Object/Array Helpers
    // =========================================================================

    auto Value::contains(const std::string& key) const noexcept -> bool {
        if (!is_object()) return false;
        const auto& obj = as_object();
        return obj.find(key) != obj.end();
    }

    auto Value::operator[](const std::string& key) -> Value& {
        return as_object()[key];
    }

    auto Value::operator[](const std::string& key) const -> const Value& {
        return as_object().at(key);
    }

    auto Value::operator[](std::size_t index) -> Value& {
        return as_array()[index];
    }

    auto Value::operator[](std::size_t index) const -> const Value& {
        return as_array()[index];
    }

    auto Value::at(const std::string& key) const noexcept -> std::optional<std::reference_wrapper<const Value>> {
        if (!is_object()) return std::nullopt;
        const auto& obj = as_object();
        auto it = obj.find(key);
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

    // =========================================================================
    // ParseError Implementation
    // =========================================================================

    ParseError::ParseError(const std::string& message, std::size_t line, std::size_t column)
        : message_(message), line_(line), column_(column) {
    }

    auto ParseError::what() const noexcept -> const char* {
        return message_.c_str();
    }

    auto ParseError::line() const noexcept -> std::size_t {
        return line_;
    }

    auto ParseError::column() const noexcept -> std::size_t {
        return column_;
    }

    // =========================================================================
    // Parser - Internal Implementation
    // =========================================================================

    namespace {

        class Parser {
        public:
            explicit Parser(std::string_view input) : input_(input), pos_(0) {}

            auto parse() -> Value {
                skip_whitespace();
                auto result = parse_value();
                skip_whitespace();
                if (!is_at_end()) {
                    throw ParseError("Unexpected trailing content", line_, column());
                }
                return result;
            }

        private:
            std::string_view input_;
            std::size_t pos_;
            std::size_t line_ = 1;
            std::size_t line_start_ = 0;

            [[nodiscard]] auto peek() const -> char {
                return is_at_end() ? '\0' : input_[pos_];
            }

            [[nodiscard]] auto peek_ahead(std::size_t offset) const -> char {
                return pos_ + offset >= input_.size() ? '\0' : input_[pos_ + offset];
            }

            auto advance() -> char {
                if (is_at_end()) return '\0';
                char c = input_[pos_++];
                if (c == '\n') {
                    line_++;
                    line_start_ = pos_;
                }
                return c;
            }

            [[nodiscard]] auto is_at_end() const -> bool {
                return pos_ >= input_.size();
            }

            [[nodiscard]] auto column() const -> std::size_t {
                return pos_ - line_start_ + 1;
            }

            auto skip_whitespace() -> void {
                while (!is_at_end() && (peek() == ' ' || peek() == '\t' ||
                                        peek() == '\n' || peek() == '\r')) {
                    advance();
                }
            }

            auto expect(char expected) -> void {
                if (peek() != expected) {
                    std::string msg = "Expected '";
                    msg += expected;
                    msg += "' but found '";
                    msg += peek() ? std::string(1, peek()) : "EOF";
                    msg += "'";
                    throw ParseError(msg, line_, column());
                }
                advance();
            }

            auto parse_value() -> Value {
                skip_whitespace();
                if (is_at_end()) {
                    throw ParseError("Unexpected end of input", line_, column());
                }

                char c = peek();
                switch (c) {
                    case 'n': return parse_null();
                    case 't': return parse_true();
                    case 'f': return parse_false();
                    case '"': return parse_string();
                    case '[': return parse_array();
                    case '{': return parse_object();
                    case '-':
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        return parse_number();
                    default:
                        throw ParseError("Unexpected character", line_, column());
                }
            }

            auto parse_null() -> Value {
                if (input_.substr(pos_, 4) != "null") {
                    throw ParseError("Invalid literal", line_, column());
                }
                pos_ += 4;
                return Value(Null{});
            }

            auto parse_true() -> Value {
                if (input_.substr(pos_, 4) != "true") {
                    throw ParseError("Invalid literal", line_, column());
                }
                pos_ += 4;
                return Value(true);
            }

            auto parse_false() -> Value {
                if (input_.substr(pos_, 5) != "false") {
                    throw ParseError("Invalid literal", line_, column());
                }
                pos_ += 5;
                return Value(false);
            }

            auto parse_string() -> std::string {
                expect('"');
                std::string result;

                while (!is_at_end() && peek() != '"') {
                    char c = advance();
                    if (c == '\\') {
                        if (is_at_end()) {
                            throw ParseError("Unterminated string escape", line_, column());
                        }
                        char esc = advance();
                        switch (esc) {
                            case '"':  result += '"';  break;
                            case '\\': result += '\\'; break;
                            case '/':  result += '/';  break;
                            case 'b':  result += '\b'; break;
                            case 'f':  result += '\f'; break;
                            case 'n':  result += '\n'; break;
                            case 'r':  result += '\r'; break;
                            case 't':  result += '\t'; break;
                            case 'u': {
                                if (pos_ + 4 > input_.size()) {
                                    throw ParseError("Invalid unicode escape", line_, column());
                                }
                                auto hex = input_.substr(pos_, 4);
                                unsigned int codepoint = 0;
                                for (char h : hex) {
                                    codepoint *= 16;
                                    if (h >= '0' && h <= '9') codepoint += h - '0';
                                    else if (h >= 'a' && h <= 'f') codepoint += h - 'a' + 10;
                                    else if (h >= 'A' && h <= 'F') codepoint += h - 'A' + 10;
                                    else throw ParseError("Invalid unicode escape", line_, column());
                                }
                                pos_ += 4;
                                // Convert to UTF-8
                                if (codepoint < 0x80) {
                                    result += static_cast<char>(codepoint);
                                } else if (codepoint < 0x800) {
                                    result += static_cast<char>(0xC0 | (codepoint >> 6));
                                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                                } else {
                                    result += static_cast<char>(0xE0 | (codepoint >> 12));
                                    result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                                    result += static_cast<char>(0x80 | (codepoint & 0x3F));
                                }
                                break;
                            }
                            default:
                                throw ParseError("Invalid escape sequence", line_, column());
                        }
                    } else if (static_cast<unsigned char>(c) < 0x20) {
                        throw ParseError("Unescaped control character", line_, column());
                    } else {
                        result += c;
                    }
                }

                expect('"');
                return result;
            }

            auto parse_number() -> Value {
                std::size_t start = pos_;

                if (peek() == '-') advance();

                if (peek() == '0') {
                    advance();
                } else if (peek() >= '1' && peek() <= '9') {
                    while (!is_at_end() && peek() >= '0' && peek() <= '9') {
                        advance();
                    }
                } else {
                    throw ParseError("Invalid number", line_, column());
                }

                bool is_float = false;

                if (peek() == '.') {
                    is_float = true;
                    advance();
                    if (peek() < '0' || peek() > '9') {
                        throw ParseError("Invalid number fraction", line_, column());
                    }
                    while (!is_at_end() && peek() >= '0' && peek() <= '9') {
                        advance();
                    }
                }

                if (peek() == 'e' || peek() == 'E') {
                    is_float = true;
                    advance();
                    if (peek() == '+' || peek() == '-') advance();
                    if (peek() < '0' || peek() > '9') {
                        throw ParseError("Invalid number exponent", line_, column());
                    }
                    while (!is_at_end() && peek() >= '0' && peek() <= '9') {
                        advance();
                    }
                }

                std::string num_str(input_.substr(start, pos_ - start));

                if (is_float) {
                    try {
                        double val = std::stod(num_str);
                        return Value(val);
                    } catch (...) {
                        throw ParseError("Invalid floating point number", line_, column());
                    }
                } else {
                    try {
                        std::int64_t val = std::stoll(num_str);
                        return Value(val);
                    } catch (...) {
                        throw ParseError("Invalid integer number", line_, column());
                    }
                }
            }

            auto parse_array() -> Value {
                expect('[');
                skip_whitespace();

                Array arr;
                if (peek() == ']') {
                    advance();
                    return Value(std::move(arr));
                }

                while (true) {
                    arr.push_back(parse_value());
                    skip_whitespace();

                    if (peek() == ']') {
                        advance();
                        break;
                    }
                    expect(',');
                }

                return Value(std::move(arr));
            }

            auto parse_object() -> Value {
                expect('{');
                skip_whitespace();

                Object obj;
                if (peek() == '}') {
                    advance();
                    return Value(std::move(obj));
                }

                while (true) {
                    skip_whitespace();
                    if (peek() != '"') {
                        throw ParseError("Expected string key", line_, column());
                    }
                    std::string key = parse_string();
                    skip_whitespace();
                    expect(':');
                    obj[std::move(key)] = parse_value();
                    skip_whitespace();

                    if (peek() == '}') {
                        advance();
                        break;
                    }
                    expect(',');
                }

                return Value(std::move(obj));
            }
        };

    } // anonymous namespace

    // =========================================================================
    // Public Parse Functions
    // =========================================================================

    auto parse(std::string_view json) -> Value {
        Parser parser(json);
        return parser.parse();
    }

    auto try_parse(std::string_view json) noexcept -> std::optional<Value> {
        try {
            return parse(json);
        } catch (...) {
            return std::nullopt;
        }
    }

    // =========================================================================
    // Serializer - Internal Implementation
    // =========================================================================

    namespace {

        class Serializer {
        public:
            explicit Serializer(const FormatOptions& opts) : options_(opts) {}

            auto serialize(const Value& value) -> std::string {
                serialize_value(value, 0);
                return result_.str();
            }

        private:
            const FormatOptions& options_;
            std::ostringstream result_;

            auto indent(std::size_t depth) -> void {
                if (options_.pretty) {
                    result_ << '\n';
                    for (std::size_t i = 0; i < depth * options_.indent_size; ++i) {
                        result_ << ' ';
                    }
                }
            }

            auto serialize_string(const std::string& str) -> void {
                result_ << '"';
                for (unsigned char c : str) {
                    switch (c) {
                        case '"':  result_ << "\\\""; break;
                        case '\\': result_ << "\\\\"; break;
                        case '\b': result_ << "\\b";  break;
                        case '\f': result_ << "\\f";  break;
                        case '\n': result_ << "\\n";  break;
                        case '\r': result_ << "\\r";  break;
                        case '\t': result_ << "\\t";  break;
                        default:
                            if (c < 0x20 || (options_.escape_unicode && c > 0x7F)) {
                                char buf[7];
                                std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                                result_ << buf;
                            } else {
                                result_ << c;
                            }
                    }
                }
                result_ << '"';
            }

            auto serialize_value(const Value& value, std::size_t depth) -> void {
                std::visit([this, depth](const auto& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, Null>) {
                        result_ << "null";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        result_ << (v ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, Number>) {
                        std::visit([this](const auto& n) {
                            if constexpr (std::is_same_v<std::decay_t<decltype(n)>, std::int64_t>) {
                                result_ << n;
                            } else {
                                std::ostringstream oss;
                                oss << std::setprecision(17) << n;
                                result_ << oss.str();
                            }
                        }, v);
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        serialize_string(v);
                    } else if constexpr (std::is_same_v<T, Array>) {
                        result_ << '[';
                        bool first = true;
                        for (const auto& elem : v) {
                            if (!first) result_ << ',';
                            first = false;
                            indent(depth + 1);
                            serialize_value(elem, depth + 1);
                        }
                        if (!v.empty()) indent(depth);
                        result_ << ']';
                    } else if constexpr (std::is_same_v<T, Object>) {
                        result_ << '{';
                        bool first = true;
                        for (const auto& [key, val] : v) {
                            if (!first) result_ << ',';
                            first = false;
                            indent(depth + 1);
                            serialize_string(key);
                            result_ << ':';
                            if (options_.pretty) result_ << ' ';
                            serialize_value(val, depth + 1);
                        }
                        if (!v.empty()) indent(depth);
                        result_ << '}';
                    }
                }, value.raw());
            }
        };

    } // anonymous namespace

    // =========================================================================
    // Public Serialize Functions
    // =========================================================================

    auto serialize(const Value& value, const FormatOptions& options) -> std::string {
        Serializer serializer(options);
        return serializer.serialize(value);
    }

    auto serialize_pretty(const Value& value, std::uint8_t indent_size) -> std::string {
        FormatOptions opts;
        opts.pretty = true;
        opts.indent_size = indent_size;
        return serialize(value, opts);
    }

} // namespace gkit::resource::metadata
