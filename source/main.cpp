#include <format>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    std::vector<std::string> args;

    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(std::string(argv[i]));
    }

    for (int i = 0; i < args.size(); i++)
    {
        std::cout << std::format("Arg {}: {}", i, argv[i]) << std::endl;
    }

    return 0;
}