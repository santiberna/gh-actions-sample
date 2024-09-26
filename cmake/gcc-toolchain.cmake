set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER "gcc")   
set(CMAKE_CXX_COMPILER "g++")

# Set the appropriate flags if needed
set(CMAKE_CXX_FLAGS "-fexceptions -frtti -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")