cmake_minimum_required(VERSION 3.14)
project(InterSchem)

set(CMAKE_CXX_STANDARD 23)

# Include FetchContent module for automatic dependency management
include(FetchContent)

# ============================================================================
# WinBGIm-64 Dependency
# ============================================================================
# Check if WinBGIm is already present locally
set(WINBGIM_LOCAL_PATH "${CMAKE_SOURCE_DIR}/headers/Winbgim")

if(EXISTS "${WINBGIM_LOCAL_PATH}/CMakeLists.txt")
    # Use local copy if available
    message(STATUS "Found local WinBGIm at: ${WINBGIM_LOCAL_PATH}")
    add_subdirectory(${WINBGIM_LOCAL_PATH} winbgim_build)
    set(WINBGIM_SOURCE_DIR ${WINBGIM_LOCAL_PATH})
else()
    # Automatically fetch WinBGIm from GitHub
    message(STATUS "WinBGIm not found locally. Fetching from GitHub...")
    FetchContent_Declare(
        winbgim
        GIT_REPOSITORY https://github.com/ki9gpin/WinBGIm-64.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(winbgim)
    set(WINBGIM_SOURCE_DIR ${winbgim_SOURCE_DIR})
    message(STATUS "WinBGIm fetched to: ${WINBGIM_SOURCE_DIR}")
endif()

# ============================================================================
# InterSchem Executable
# ============================================================================
add_executable(InterSchem 
    src/main.cpp
    src/renderer.cpp
    src/interaction.cpp
    src/file_io.cpp
    src/interpreter.cpp
    src/codegen.cpp
)

# Link directories for the bgi library
target_link_directories(InterSchem PRIVATE
    ${WINBGIM_SOURCE_DIR}/libbgi/lib
)

# Include directories
# 1. Allow #include "headers/geometry.h" etc so we can match project structure
target_include_directories(InterSchem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 2. Allow #include <graphics.h> directly (it's in include/bgi)
target_include_directories(InterSchem PRIVATE ${WINBGIM_SOURCE_DIR}/libbgi/include/bgi)

# Link against bgi (the target defined in Winbgim/libbgi)
target_link_libraries(InterSchem PRIVATE
    bgi
)