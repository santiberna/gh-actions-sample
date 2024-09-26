cmake_minimum_required(VERSION 3.22.1)
project(Project)

add_executable(App "source/main.cpp")
target_compile_features(App PRIVATE cxx_std_20)