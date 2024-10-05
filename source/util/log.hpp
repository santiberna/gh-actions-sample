#pragma once
#include <iostream>
#include <string_view>
#include "format.hpp"

template <typename... Args>
void log(std::string_view str, Args&&... args)
{
    std::cout << fmt::vformat(str, fmt::make_format_args(args...)) << std::endl;
}
