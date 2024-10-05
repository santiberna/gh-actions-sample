#pragma once
#include <ostream>
#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <unordered_map>
#include <istream>

struct JSONValue
{
    std::variant<
        std::monostate,
        bool,
        double,
        std::string,
        std::vector<JSONValue>,
        std::unordered_map<std::string, JSONValue>>
        m_data;

    [[nodiscard]] bool is_null() const { return std::holds_alternative<std::monostate>(m_data); }
    [[nodiscard]] bool is_bool() const { return std::holds_alternative<bool>(m_data); }
    [[nodiscard]] bool is_number() const { return std::holds_alternative<double>(m_data); }
    [[nodiscard]] bool is_string() const { return std::holds_alternative<std::string>(m_data); }
    [[nodiscard]] bool is_array() const { return std::holds_alternative<std::vector<JSONValue>>(m_data); }
    [[nodiscard]] bool is_object() const { return std::holds_alternative<std::unordered_map<std::string, JSONValue>>(m_data); }
};

class JSONParser
{
public:
    JSONParser(std::ostream* error_log = nullptr)
        : m_error_log(error_log) { };
    std::optional<JSONValue> parse_value(std::istream& stream);

private:
    std::optional<std::unordered_map<std::string, JSONValue>> parse_object(std::istream& stream);
    std::optional<std::vector<JSONValue>> parse_array(std::istream& stream);
    std::optional<std::string> parse_string(std::istream& stream);
    std::optional<double> parse_number(std::istream& stream);

    enum class LITERAL
    {
        NONE,
        FALSE,
        TRUE
    };
    std::optional<LITERAL> parse_literal(std::istream& stream);

    static void skip_white_space(std::istream& stream);

    std::optional<int> read_stream(std::istream& stream);
    static void pop_stream(std::istream& stream);

    template <typename T>
    void error_log(const T& v)
    {
        if (m_error_log) {
            (*m_error_log) << v << " ";
}
    }
    std::ostream* m_error_log = nullptr;
};