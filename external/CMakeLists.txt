add_library(External INTERFACE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(SDL)

target_include_directories(External
    INTERFACE SDL/include
)

target_link_libraries(External
    INTERFACE SDL3-static
)