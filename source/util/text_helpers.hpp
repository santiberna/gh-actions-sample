#pragma once
#include <ios>
#include <istream>
#include <cctype>
#include <optional>
#include <filesystem>
#include <fstream>
#include <string>
#include <charconv>
#include <functional>
#include <vector>
#include <string_view>
#include <system_error>

std::optional<std::ifstream> open_file_read_stream(
    const std::filesystem::path& path, std::ios_base::openmode flags = {});

std::vector<std::string> split_stream_into_lines(
    std::istream& stream, char delimiter = '\n');

std::string stream_to_string(
    std::istream& stream);

std::vector<std::string> into_words(const std::string& str);

std::string replace_all(std::string str, const std::string& search, const std::string& replace);

void foreach_line(
    std::istream& stream, const std::function<void(const std::string&)>& fn, char delimiter = '\n');

std::vector<std::string> split_string(
    std::string_view str, std::string_view delimiter = " ");

template <typename T>
std::optional<T> parse_number(std::string_view str)
{
    T val {};
    auto r = std::from_chars(str.data(), str.data() + str.size(), val);
    if (r.ec == std::errc {}) {
        return val;
}
    return std::nullopt;
}

template <typename T>
std::optional<T> parse_hex(std::string_view str)
{
    T val {};
    auto r = std::from_chars(str.data(), str.data() + str.size(), val, 16);
    if (r.ec == std::errc {}) {
        return val;
}
    return std::nullopt;
}

template <typename T>
std::vector<T> parse_all_numbers(std::string_view str)
{
    std::vector<T> out {};

    const auto *ptr = str.data();
    const auto *end_sentinel = ptr + str.size();

    while (ptr != end_sentinel)
    {
        if (std::isdigit(*ptr) || *ptr == '-')
        {
            T val {};
            auto r = std::from_chars(ptr, str.data() + str.size(), val);
            if (r.ec != std::errc {}) {
                continue;
}

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
