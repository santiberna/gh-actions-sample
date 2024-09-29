cmake_minimum_required(VERSION 3.22.1)
project(Project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)
add_subdirectory(external)

add_executable(App "source/main.cpp")

target_include_directories(App PRIVATE ${Vulkan_INCLUDE_DIR})
target_link_libraries(App PRIVATE ${Vulkan_LIBRARY} External)

target_compile_features(App PRIVATE cxx_std_20)
