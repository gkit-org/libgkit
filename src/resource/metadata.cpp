#include "gkit/resource/metadata.hpp"

#include <iomanip>
#include <optional>
#include <sstream>

namespace gkit::resource::metadata {

    

    // // =========================================================================
    // // ParseError Implementation
    // // =========================================================================

    // ParseError::ParseError(const std::string& message, std::size_t line, std::size_t column) :
    //     message(message), line(line), column(column) {}

    // auto ParseError::what() const noexcept -> const char* {
    //     return message.c_str();
    // }

    // auto ParseError::get_line() const noexcept -> std::size_t {
    //     return line;
    // }

    // auto ParseError::get_column() const noexcept -> std::size_t {
    //     return column;
    // }

    // // =========================================================================
    // // Parser - Internal Implementation
    // // =========================================================================

    // namespace {

    //     class Parser {
    //     public:
    //         explicit Parser(std::string_view input) : input(input), pos(0) {}

    //         auto parse() -> Value {
    //             skip_whitespace();
    //             auto result = parse_value();
    //             skip_whitespace();
    //             if (!is_at_end()) {
    //                 throw ParseError("Unexpected trailing content", line, column());
    //             }
    //             return result;
    //         }

    //     private:
    //         std::string_view input;
    //         std::size_t pos;
    //         std::size_t line      = 1;
    //         std::size_t linestart = 0;

    //         [[nodiscard]] auto peek() const -> char { return is_at_end() ? '\0' : input[pos]; }

    //         [[nodiscard]] auto peek_ahead(std::size_t offset) const -> char {
    //             return pos + offset >= input.size() ? '\0' : input[pos + offset];
    //         }

    //         auto advance() -> char {
    //             if (is_at_end()) return '\0';
    //             char c = input[pos++];
    //             if (c == '\n') {
    //                 line++;
    //                 linestart = pos;
    //             }
    //             return c;
    //         }

    //         [[nodiscard]] auto is_at_end() const -> bool { return pos >= input.size(); }

    //         [[nodiscard]] auto column() const -> std::size_t { return pos - linestart + 1; }

    //         auto skip_whitespace() -> void {
    //             while (!is_at_end() && (peek() == ' ' || peek() == '\t' || peek() == '\n' || peek() == '\r')) {
    //                 advance();
    //             }
    //         }

    //         auto expect(char expected) -> void {
    //             if (peek() != expected) {
    //                 std::string msg = "Expected '";
    //                 msg += expected;
    //                 msg += "' but found '";
    //                 msg += peek() ? std::string(1, peek()) : "EOF";
    //                 msg += "'";
    //                 throw ParseError(msg, line, column());
    //             }
    //             advance();
    //         }

    //         auto parse_value() -> Value {
    //             skip_whitespace();
    //             if (is_at_end()) {
    //                 throw ParseError("Unexpected end of input", line, column());
    //             }

    //             char c = peek();
    //             switch (c) {
    //             case 'n':
    //                 return Value(parse_null());
    //             case 't':
    //                 return Value(parse_true());
    //             case 'f':
    //                 return Value(parse_false());
    //             case '"':
    //                 return Value(parse_string());
    //             case '[':
    //                 return Value(parse_array());
    //             case '{':
    //                 return Value(parse_map());
    //             case '-':
    //             case '0':
    //             case '1':
    //             case '2':
    //             case '3':
    //             case '4':
    //             case '5':
    //             case '6':
    //             case '7':
    //             case '8':
    //             case '9':
    //                 return Value(parse_number());
    //             default:
    //                 throw ParseError("Unexpected character", line, column());
    //             }
    //         }

    //         auto parse_null() -> Value {
    //             if (input.substr(pos, 4) != "null") {
    //                 throw ParseError("Invalid literal", line, column());
    //             }
    //             pos += 4;
    //             return Value(Null{});
    //         }

    //         auto parse_true() -> Value {
    //             if (input.substr(pos, 4) != "true") {
    //                 throw ParseError("Invalid literal", line, column());
    //             }
    //             pos += 4;
    //             return Value(true);
    //         }

    //         auto parse_false() -> Value {
    //             if (input.substr(pos, 5) != "false") {
    //                 throw ParseError("Invalid literal", line, column());
    //             }
    //             pos += 5;
    //             return Value(false);
    //         }

    //         auto parse_string() -> std::string {
    //             expect('"');
    //             std::string result;

    //             while (!is_at_end() && peek() != '"') {
    //                 char c = advance();
    //                 if (c == '\\') {
    //                     if (is_at_end()) {
    //                         throw ParseError("Unterminated string escape", line, column());
    //                     }
    //                     char esc = advance();
    //                     switch (esc) {
    //                     case '"':
    //                         result += '"';
    //                         break;
    //                     case '\\':
    //                         result += '\\';
    //                         break;
    //                     case '/':
    //                         result += '/';
    //                         break;
    //                     case 'b':
    //                         result += '\b';
    //                         break;
    //                     case 'f':
    //                         result += '\f';
    //                         break;
    //                     case 'n':
    //                         result += '\n';
    //                         break;
    //                     case 'r':
    //                         result += '\r';
    //                         break;
    //                     case 't':
    //                         result += '\t';
    //                         break;
    //                     case 'u': {
    //                         if (pos + 4 > input.size()) {
    //                             throw ParseError("Invalid unicode escape", line, column());
    //                         }
    //                         auto hex               = input.substr(pos, 4);
    //                         unsigned int codepoint = 0;
    //                         for (char h : hex) {
    //                             codepoint *= 16;
    //                             if (h >= '0' && h <= '9') {
    //                                 codepoint += h - '0';
    //                             } else if (h >= 'a' && h <= 'f') {
    //                                 codepoint += h - 'a' + 10;
    //                             } else if (h >= 'A' && h <= 'F') {
    //                                 codepoint += h - 'A' + 10;
    //                             } else {
    //                                 throw ParseError("Invalid unicode escape", line, column());
    //                             }
    //                         }
    //                         pos += 4;
    //                         // Convert to UTF-8
    //                         if (codepoint < 0x80) {
    //                             result += static_cast<char>(codepoint);
    //                         } else if (codepoint < 0x800) {
    //                             result += static_cast<char>(0xC0 | (codepoint >> 6));
    //                             result += static_cast<char>(0x80 | (codepoint & 0x3F));
    //                         } else {
    //                             result += static_cast<char>(0xE0 | (codepoint >> 12));
    //                             result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    //                             result += static_cast<char>(0x80 | (codepoint & 0x3F));
    //                         }
    //                         break;
    //                     }
    //                     default:
    //                         throw ParseError("Invalid escape sequence", line, column());
    //                     }
    //                 } else if (static_cast<unsigned char>(c) < 0x20) {
    //                     throw ParseError("Unescaped control character", line, column());
    //                 } else {
    //                     result += c;
    //                 }
    //             }

    //             expect('"');
    //             return result;
    //         }

    //         auto parse_number() -> Value {
    //             std::size_t start = pos;

    //             if (peek() == '-') advance();

    //             if (peek() == '0') {
    //                 advance();
    //             } else if (peek() >= '1' && peek() <= '9') {
    //                 while (!is_at_end() && peek() >= '0' && peek() <= '9') {
    //                     advance();
    //                 }
    //             } else {
    //                 throw ParseError("Invalid number", line, column());
    //             }

    //             bool is_float = false;

    //             if (peek() == '.') {
    //                 is_float = true;
    //                 advance();
    //                 if (peek() < '0' || peek() > '9') {
    //                     throw ParseError("Invalid number fraction", line, column());
    //                 }
    //                 while (!is_at_end() && peek() >= '0' && peek() <= '9') {
    //                     advance();
    //                 }
    //             }

    //             if (peek() == 'e' || peek() == 'E') {
    //                 is_float = true;
    //                 advance();
    //                 if (peek() == '+' || peek() == '-') advance();
    //                 if (peek() < '0' || peek() > '9') {
    //                     throw ParseError("Invalid number exponent", line, column());
    //                 }
    //                 while (!is_at_end() && peek() >= '0' && peek() <= '9') {
    //                     advance();
    //                 }
    //             }

    //             std::string num_str(input.substr(start, pos - start));

    //             if (is_float) {
    //                 try {
    //                     double val = std::stod(num_str);
    //                     return Value(val);
    //                 } catch (...) {
    //                     throw ParseError("Invalid floating point number", line, column());
    //                 }
    //             } else {
    //                 try {
    //                     std::int64_t val = std::stoll(num_str);
    //                     return Value(val);
    //                 } catch (...) {
    //                     throw ParseError("Invalid integer number", line, column());
    //                 }
    //             }
    //         }

    //         auto parse_array() -> Value {
    //             expect('[');
    //             skip_whitespace();

    //             Array arr;
    //             if (peek() == ']') {
    //                 advance();
    //                 return Value(std::move(arr));
    //             }

    //             while (true) {
    //                 arr.push_back(parse_value());
    //                 skip_whitespace();

    //                 if (peek() == ']') {
    //                     advance();
    //                     break;
    //                 }
    //                 expect(',');
    //             }

    //             return Value(std::move(arr));
    //         }

    //         auto parse_map() -> Value {
    //             expect('{');
    //             skip_whitespace();

    //             Map obj;
    //             if (peek() == '}') {
    //                 advance();
    //                 return Value(std::move(obj));
    //             }

    //             while (true) {
    //                 skip_whitespace();
    //                 if (peek() != '"') {
    //                     throw ParseError("Expected string key", line, column());
    //                 }
    //                 std::string key = parse_string();
    //                 skip_whitespace();
    //                 expect(':');
    //                 obj[std::move(key)] = parse_value();
    //                 skip_whitespace();

    //                 if (peek() == '}') {
    //                     advance();
    //                     break;
    //                 }
    //                 expect(',');
    //             }

    //             return Value(std::move(obj));
    //         }
    //     };

    // } // anonymous namespace

    // // =========================================================================
    // // Public Parse Functions
    // // =========================================================================

    // auto parse(std::string_view json) -> Value {
    //     Parser parser(json);
    //     return parser.parse();
    // }

    // auto try_parse(std::string_view json) noexcept -> std::optional<Value> {
    //     try {
    //         return parse(json);
    //     } catch (...) {
    //         return std::nullopt;
    //     }
    // }

    // // =========================================================================
    // // Serializer - Internal Implementation
    // // =========================================================================

    // namespace {

    //     class Serializer {
    //     public:
    //         explicit Serializer(const FormatOptions& opts) : options(opts) {}

    //         auto serialize(const Value& value) -> std::string {
    //             serialize_value(value, 0);
    //             return result.str();
    //         }

    //     private:
    //         const FormatOptions& options;
    //         std::ostringstream result;

    //         auto indent(std::size_t depth) -> void {
    //             if (options.pretty) {
    //                 result << '\n';
    //                 for (std::size_t i = 0; i < depth * options.indent_size; ++i) {
    //                     result << ' ';
    //                 }
    //             }
    //         }

    //         auto serialize_string(const std::string& str) -> void {
    //             result << '"';
    //             for (unsigned char c : str) {
    //                 switch (c) {
    //                 case '"':
    //                     result << "\\\"";
    //                     break;
    //                 case '\\':
    //                     result << "\\\\";
    //                     break;
    //                 case '\b':
    //                     result << "\\b";
    //                     break;
    //                 case '\f':
    //                     result << "\\f";
    //                     break;
    //                 case '\n':
    //                     result << "\\n";
    //                     break;
    //                 case '\r':
    //                     result << "\\r";
    //                     break;
    //                 case '\t':
    //                     result << "\\t";
    //                     break;
    //                 default:
    //                     if (c < 0x20 || (options.escape_unicode && c > 0x7F)) {
    //                         char buf[7];
    //                         std::snprintf(buf, sizeof(buf), "\\u%04x", c);
    //                         result << buf;
    //                     } else {
    //                         result << c;
    //                     }
    //                 }
    //             }
    //             result << '"';
    //         }

    //         auto serialize_value(const Value& value, std::size_t depth) -> void {
    //             std::visit(
    //                 [this, depth](const auto& v) {
    //                     using T = std::decay_t<decltype(v)>;
    //                     if constexpr (std::is_same_v<T, Null>) {
    //                         result << "null";
    //                     } else if constexpr (std::is_same_v<T, bool>) {
    //                         result << (v ? "true" : "false");
    //                     } else if constexpr (std::is_same_v<T, Number>) {
    //                         std::visit(
    //                             [this](const auto& n) {
    //                                 if constexpr (std::is_same_v<std::decay_t<decltype(n)>, std::int64_t>) {
    //                                     result << n;
    //                                 } else {
    //                                     std::ostringstream oss;
    //                                     oss << std::setprecision(17) << n;
    //                                     result << oss.str();
    //                                 }
    //                             },
    //                             v);
    //                     } else if constexpr (std::is_same_v<T, std::string>) {
    //                         serialize_string(v);
    //                     } else if constexpr (std::is_same_v<T, Array>) {
    //                         result << '[';
    //                         bool first = true;
    //                         for (const auto& elem : v) {
    //                             if (!first) result << ',';
    //                             first = false;
    //                             indent(depth + 1);
    //                             serialize_value(elem, depth + 1);
    //                         }
    //                         if (!v.empty()) indent(depth);
    //                         result << ']';
    //                     } else if constexpr (std::is_same_v<T, Map>) {
    //                         result << '{';
    //                         bool first = true;
    //                         for (const auto& [key, val] : v) {
    //                             if (!first) result << ',';
    //                             first = false;
    //                             indent(depth + 1);
    //                             serialize_string(key);
    //                             result << ':';
    //                             if (options.pretty) result << ' ';
    //                             serialize_value(val, depth + 1);
    //                         }
    //                         if (!v.empty()) indent(depth);
    //                         result << '}';
    //                     }
    //                 },
    //                 value.raw());
    //         }
    //     };

    // } // anonymous namespace

    // // =========================================================================
    // // Public Serialize Functions
    // // =========================================================================

    // auto serialize(const Value& value, const FormatOptions& options) -> std::string {
    //     Serializer serializer(options);
    //     return serializer.serialize(value);
    // }

    // auto serialize_pretty(const Value& value, std::uint8_t indent_size) -> std::string {
    //     FormatOptions opts;
    //     opts.pretty      = true;
    //     opts.indent_size = indent_size;
    //     return serialize(value, opts);
    // }

} // namespace gkit::resource::metadata
