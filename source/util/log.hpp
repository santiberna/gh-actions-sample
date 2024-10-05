#pragma once
#include <iostream>
#include "format.hpp"

template <typename... Args>
void Log(std::string_view str, Args&&... args)
{
    std::cout << fmt::vformat(str, fmt::make_format_args(args...)) << std::endl;
}
