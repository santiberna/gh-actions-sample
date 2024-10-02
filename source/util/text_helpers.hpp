#pragma once
#include <optional>
#include <filesystem>
#include <fstream>
#include <string>
#include <charconv>
#include <functional>

std::optional<std::ifstream> OpenFileReadStream(
    const std::filesystem::path& path, std::ios_base::openmode flags = {});

std::vector<std::string> SplitStreamIntoLines(
    std::istream& stream, char delimiter = '\n');

std::string StreamToString(
    std::istream& stream);

std::vector<std::string> IntoWords(const std::string& str);

std::string ReplaceAll(std::string str, const std::string& search, const std::string& replace);

void ForeachLine(
    std::istream& stream, std::function<void(const std::string&)> fn, char delimiter = '\n');

std::vector<std::string> SplitString(
    std::string_view str, std::string_view delimiter = " ");

template <typename T>
std::optional<T> ParseNumber(std::string_view str)
{
    T val {};
    auto r = std::from_chars(str.data(), str.data() + str.size(), val);
    if (r.ec == std::errc {})
        return val;
    return std::nullopt;
}

template <typename T>
std::optional<T> ParseHex(std::string_view str)
{
    T val {};
    auto r = std::from_chars(str.data(), str.data() + str.size(), val, 16);
    if (r.ec == std::errc {})
        return val;
    return std::nullopt;
}

template <typename T>
std::vector<T> ParseAllNumbers(std::string_view str)
{
    std::vector<T> out {};

    auto ptr = str.data();
    auto end_sentinel = ptr + str.size();

    while (ptr != end_sentinel)
    {
        if (std::isdigit(*ptr) || *ptr == '-')
        {
            T val {};
            auto r = std::from_chars(ptr, str.data() + str.size(), val);
            if (r.ec != std::errc {})
                continue;

            out.emplace_back(val);
            ptr = r.ptr;
        }
        else
        {
            ptr++;
        }
    }

    return out;
}
