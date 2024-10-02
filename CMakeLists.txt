cmake_minimum_required(VERSION 3.22.1)
project(Project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(App)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "source/*.cpp")
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "source/*.hpp")

target_sources(App PRIVATE ${headers} ${sources})

target_include_directories(App PRIVATE "third-party")
target_compile_features(App PRIVATE cxx_std_20)
