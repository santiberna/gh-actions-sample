#pragma once
#include <iostream>
#include <format>

template<typename... Args>
void Log(std::string_view str, Args&&... args)
{
	std::cout << std::vformat(str, std::make_format_args(args...)) << std::endl;
}
