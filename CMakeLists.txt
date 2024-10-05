cmake_minimum_required(VERSION 3.22.1)
project(Project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(App)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "source/*.cpp")
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "source/*.hpp")

target_sources(App PRIVATE ${headers} ${sources})

target_include_directories(App PRIVATE "third-party" "source")
target_compile_features(App PRIVATE cxx_std_20)

# External library test
# find_package(Vulkan REQUIRED)
# target_link_libraries(App PRIVATE ${Vulkan_LIBRARIES} )
# target_include_directories(App PRIVATE ${Vulkan_INCLUDE_DIRS})

option(ENABLE_PCH "Compile with precompiled header" OFF)
if (ENABLE_PCH)
    target_precompile_headers(App PRIVATE [["pch.hpp"]])
endif ()