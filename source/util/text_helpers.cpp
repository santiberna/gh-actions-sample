#include "text_helpers.hpp"

std::optional<std::ifstream> OpenFileReadStream(const std::filesystem::path& path, std::ios_base::openmode flags)
{
    std::ifstream s = std::ifstream(path.string(), flags);
    if (s)
        return s;
    else
        return std::nullopt;
}

std::vector<std::string> SplitStreamIntoLines(std::istream& stream, char delimiter)
{
    std::vector<std::string> out {};
    std::string temp {};

    while (std::getline(stream, temp, delimiter))
    {
        out.emplace_back(std::move(temp));
    }

    return out;
}

std::vector<std::string> IntoWords(const std::string& str)
{
    std::istringstream input { str };
    return SplitStreamIntoLines(input, ' ');
}

std::string ReplaceAll(std::string str, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = str.find(search, pos)) != std::string::npos)
    {
        str.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return str;
}

void ForeachLine(std::istream& stream, std::function<void(const std::string& line)> fn, char delimiter)
{
    std::string temp {};
    while (std::getline(stream, temp, delimiter))
    {
        fn(temp);
    }
}

std::string StreamToString(std::istream& stream)
{
    std::string out;
    auto current = stream.tellg();

    stream.seekg(0, std::ios_base::end);
    auto size = stream.tellg() - current;
    stream.seekg(current);

    out.resize(size);
    stream.read(out.data(), size);

    return out;
}

std::vector<std::string> SplitString(std::string_view str, std::string_view delimiter)
{
    std::vector<std::string> out;
    std::string temp = std::string(str);
    size_t it = 0;

    while ((it = temp.find(delimiter)) != std::string::npos)
    {
        out.emplace_back(std::move(temp.substr(0, it)));
        temp.erase(0, it + delimiter.length());
    }

    out.emplace_back(std::move(temp));

    return out;
}
