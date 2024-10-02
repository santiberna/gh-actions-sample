#pragma once
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
		std::unordered_map<std::string, JSONValue>
	> data;

	bool IsNull() const { return std::holds_alternative<std::monostate>(data); }
	bool IsBool() const { return std::holds_alternative<bool>(data); }
	bool IsNumber() const { return std::holds_alternative<double>(data); }
	bool IsString() const { return std::holds_alternative<std::string>(data); }
	bool IsArray() const { return std::holds_alternative<std::vector<JSONValue>>(data); }
	bool IsObject() const { return std::holds_alternative<std::unordered_map<std::string, JSONValue>>(data); }
};

class JSONParser
{
public:

	JSONParser(std::ostream* error_log = nullptr) : error_log(error_log) {};
	std::optional<JSONValue> ParseValue(std::istream& stream);

private:
	
	std::optional<std::unordered_map<std::string, JSONValue>> ParseObject(std::istream& stream);
	std::optional<std::vector<JSONValue>> ParseArray(std::istream& stream);
	std::optional<std::string> ParseString(std::istream& stream);
	std::optional<double> ParseNumber(std::istream& stream);

	enum class LITERAL { NONE, FALSE, TRUE };
	std::optional<LITERAL> ParseLiteral(std::istream& stream);

	void SkipWhiteSpace(std::istream& stream);

	std::optional<int> ReadStream(std::istream& stream);
	void PopStream(std::istream& stream);

	template<typename T> void ErrorLog(const T& v) { if (error_log) (*error_log) << v << " "; }
	std::ostream* error_log = nullptr;
};