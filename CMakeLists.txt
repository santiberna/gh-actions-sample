cmake_minimum_required(VERSION 3.22.1)
project(Project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

add_executable(App)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "source/*.cpp")
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "source/*.hpp")

target_sources(App PRIVATE ${headers} ${sources})

target_include_directories(App PRIVATE ${Vulkan_INCLUDE_DIR})
target_compile_features(App PRIVATE cxx_std_20)
