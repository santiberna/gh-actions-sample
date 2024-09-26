cmake_minimum_required(VERSION 3.22.1)
project(Project)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(App "source/main.cpp")
target_compile_features(App PRIVATE cxx_std_20)
