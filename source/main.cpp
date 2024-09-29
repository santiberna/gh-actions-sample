#include <iostream>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

int main(int argc, char* argv[])
{
    std::cout << "Hello World" << std::endl;

    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }

    if (false)
    {
        vk::ApplicationInfo appInfo = {};
        appInfo.pApplicationName = "My Vulkan Application"; // Application name
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // Application version
        appInfo.pEngineName = "No Engine"; // Engine name (if any)
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); // Engine version
        appInfo.apiVersion = VK_API_VERSION_1_2; // The Vulkan API version (1.2 in this case)

        vk::InstanceCreateInfo createInfo = {};
        createInfo.pApplicationInfo = &appInfo; // Associate the appInfo with the createInfo

        vk::Instance instance = vk::createInstance(createInfo);
        instance.destroy();
    }

    return 0;
}