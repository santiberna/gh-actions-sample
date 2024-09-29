#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    std::vector<std::string> cmd_args;

    for (int i = 0; i < argc; i++)
    {
        cmd_args.emplace_back(std::string(argv[i]));
    }

    for (int i = 0; i < cmd_args.size(); i++)
    {
        std::cout << /*std::format("Arg {}: {}", i, cmd_args[i])*/ cmd_args[i] << std::endl;
    }

    return 0;
}