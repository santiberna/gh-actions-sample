#include "json.hpp"

#include "fmt/format.h"
#include "format.hpp"
#include <cctype>
#include <charconv>
#include <string>
#include <optional>
#include <istream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <system_error>

constexpr int STRING_STARTER = '\"';
constexpr int STRING_ENDER = '\"';

constexpr int OBJECT_STARTER = '{';
constexpr int OBJECT_ENDER = '}';

constexpr int ARRAY_STARTER = '[';
constexpr int ARRAY_ENDER = ']';

constexpr int ELEMENT_SEPARATOR = ',';
constexpr int ELEMENT_ASSIGNER = ':';

constexpr int NUMBER_START_RANGE = '0';
constexpr int NUMBER_END_RANGE = '9';
constexpr int NEGATIVE_SIGN = '-';
constexpr int POSITIVE_SIGN = '+';

constexpr int END_OF_FILE = -1;

const std::string NULL_LITERAL = "null";
const std::string TRUE_LITERAL = "true";
const std::string FALSE_LITERAL = "false";

std::optional<JSONValue> JSONParser::ParseValue(std::istream& stream)
{
    SkipWhiteSpace(stream);

    int next_token {};
    if (auto next = ReadStream(stream))
    {
        next_token = next.value();
    }
    else
    {
        return std::nullopt;
    }

    if (next_token == STRING_STARTER)
    {
        if (auto str = ParseString(stream))
        {
            return JSONValue { str.value() };
        }
    }
    else if (next_token == OBJECT_STARTER)
    {
        if (auto arr = ParseObject(stream))
        {
            return JSONValue { arr.value() };
        }
    }
    else if (next_token == ARRAY_STARTER)
    {
        if (auto arr = ParseArray(stream))
        {
            return JSONValue { arr.value() };
        }
    }
    else if (next_token == NEGATIVE_SIGN || (next_token >= NUMBER_START_RANGE && next_token <= NUMBER_END_RANGE))
    {
        if (auto val = ParseNumber(stream))
        {
            return JSONValue { val.value() };
        }
    }
    else
    {
        if (auto val = ParseLiteral(stream))
        {
            switch (val.value())
            {
            case LITERAL::NONE:
                return JSONValue { std::monostate() };
                break;
            case LITERAL::FALSE:
                return JSONValue { false };
                break;
            case LITERAL::TRUE:
                return JSONValue { true };
                break;
            }
        }
    }

    ErrorLog("Value empty");
    return std::nullopt;
}

std::optional<std::unordered_map<std::string, JSONValue>> JSONParser::ParseObject(std::istream& stream)
{
    PopStream(stream); // Pop starting {

    std::unordered_map<std::string, JSONValue> out;
    bool complete = false;

    // Check empty objects
    if (auto c = ReadStream(stream))
    {
        if (c == OBJECT_ENDER)
        {
            return out;
        }
    }
    else
    {
        ErrorLog("Failed to Parse Object");
    }

    while (!complete)
    {
        SkipWhiteSpace(stream);

        // Find new key

        std::string key;
        if (auto new_key = ParseString(stream))
        {
            key = std::move(new_key.value());
        }
        else
        {
            ErrorLog("Failed to Parse Object Key");
            return std::nullopt;
        }

        SkipWhiteSpace(stream);

        // Check :

        if (auto next = ReadStream(stream))
        {
            if (next.value() != ELEMENT_ASSIGNER)
            {
                ErrorLog("Expected : After Key");
                return std::nullopt;
            }
        }
        else
        {
            ErrorLog("Failed to Read Key-Object Pair Continuation");
            return std::nullopt;
        }

        PopStream(stream);
        SkipWhiteSpace(stream);

        // Find next value

        if (auto value = ParseValue(stream))
        {
            if (auto it = out.find(key); it == out.end())
            {
                out.emplace(key, value.value());
            }
            else
            {
                ErrorLog("Equal Name Keys not Supported");
            }
        }
        else
        {
            ErrorLog("Failed to Parse Value");
            return std::nullopt;
        }

        SkipWhiteSpace(stream);

        // Check } or ,

        int next_token = {};
        if (auto next = ReadStream(stream))
        {
            next_token = next.value();
        }
        else
        {
            ErrorLog("Failed to Read Object Continuation");
            return std::nullopt;
        }

        PopStream(stream);

        if (next_token == OBJECT_ENDER)
        {
            complete = true;
        }
        else if (next_token != ELEMENT_SEPARATOR)
        {
            ErrorLog("Expected \",\" separating elements");
            return std::nullopt;
        }
    }

    return out;
}

std::optional<std::vector<JSONValue>> JSONParser::ParseArray(std::istream& stream)
{
    PopStream(stream); // Pop starting [

    std::vector<JSONValue> out;
    bool complete = false;

    // Check empty arrays
    if (auto c = ReadStream(stream))
    {
        if (c == ARRAY_ENDER)
        {
            return out;
        }
    }
    else
    {
        ErrorLog("Failed to Parse Array");
    }

    while (!complete)
    {
        SkipWhiteSpace(stream);

        if (auto new_elem = ParseValue(stream))
        {
            out.emplace_back(new_elem.value());
        }
        else
        {
            ErrorLog("Failed to Parse Array Element");
            return std::nullopt;
        }

        SkipWhiteSpace(stream);

        int next_token = {};
        if (auto next = ReadStream(stream))
        {
            next_token = next.value();
        }
        else
        {
            ErrorLog("Failed to Read Array Continuation");
            return std::nullopt;
        }

        PopStream(stream);

        if (next_token == ARRAY_ENDER)
        {
            complete = true;
        }
        else if (next_token != ELEMENT_SEPARATOR)
        {
            ErrorLog("Expected \",\" separating elements");
            return std::nullopt;
        }
    }

    return out;
}

std::optional<std::string> JSONParser::ParseString(std::istream& stream)
{
    PopStream(stream); // Pop starting "

    std::string out;
    bool complete = false;

    while (auto new_char = ReadStream(stream))
    {
        PopStream(stream);

        if (new_char.value() == STRING_ENDER)
        {
            complete = true;
            break;
        }
        
        
            out += new_char.value();
       
    }

    if (!complete)
    {
        ErrorLog("Failed to find trailing \" that ends a string");
    }

    return out;
}

std::optional<double> JSONParser::ParseNumber(std::istream& stream)
{
    std::string num_buf {};

    while (auto c = ReadStream(stream))
    {
        if ((std::isdigit(c.value()) != 0) || c.value() == NEGATIVE_SIGN || c.value() == 'e' || c.value() == 'E' || c.value() == POSITIVE_SIGN || c.value() == '.')
        {
            PopStream(stream);
            num_buf += c.value();
        }
        else
        {
            break;
        }
    }

    double out {};
    auto result = std::from_chars(num_buf.data(), num_buf.data() + num_buf.size(), out);

    if (result.ec == std::errc {})
    {
        return out;
    }
    
    
        ErrorLog("Failed to parse number");
        return std::nullopt;
   
}

void JSONParser::SkipWhiteSpace(std::istream& stream)
{
    while (stream)
    {
        if (std::isspace(stream.peek()) == 0) {
            break;
}
        stream.get();
    }
}

std::optional<JSONParser::LITERAL> JSONParser::ParseLiteral(std::istream& stream)
{
    std::string literalBuf {};

    while (auto c = ReadStream(stream))
    {
        if (c.value() == ELEMENT_SEPARATOR || (std::isspace(c.value()) != 0))
        {
            break;
        }

        PopStream(stream);
        literalBuf += c.value();
    }

    if (literalBuf == NULL_LITERAL) {
        return LITERAL::NONE;
    } if (literalBuf == FALSE_LITERAL)
        return LITERAL::FALSE;
    else if (literalBuf == TRUE_LITERAL)
        return LITERAL::TRUE;
    else
    {
        ErrorLog(fmt::format("Unknown literal found: \"{}\"", literalBuf));
        return std::nullopt;
    }
}

std::optional<int> JSONParser::ReadStream(std::istream& stream)
{
    if (stream)
    {
        int c = stream.peek();
        if (c == END_OF_FILE)
        {
            return std::nullopt;
        }
        
        
            return c;
       
    }
    else
    {
        ErrorLog("Stream Error");
        return std::nullopt;
    }
}
void JSONParser::PopStream(std::istream& stream)
{
    if (stream) {
        stream.get();
}
}
