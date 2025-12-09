cmake_minimum_required(VERSION 3.11)
project(InterSchem)

set(CMAKE_CXX_STANDARD 17)

# Add the WinBGIm subdirectory
# This assumes headers/Winbgim is a valid CMake project (which we verified)
add_subdirectory(headers/Winbgim)

# Define the executable
add_executable(InterSchem src/main.cpp)

# Link against bgi (the target defined in Winbgim/libbgi)
target_link_libraries(InterSchem PRIVATE bgi)

# Include directories
# 1. Allow #include "headers/geometry.h" etc so we can match project structure
target_include_directories(InterSchem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 2. Allow #include <graphics.h> directly (it's in include/bgi)
target_include_directories(InterSchem PRIVATE headers/Winbgim/libbgi/include/bgi)
