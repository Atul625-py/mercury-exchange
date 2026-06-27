cmake_minimum_required(VERSION 3.20)
project(MercuryExchange VERSION 1.0.0 LANGUAGES CXX)

# Force modern C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands so Sublime Text's LSP/clangd can index our code perfectly
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure compiler warning flags for rigorous performance engineering
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -O3)
endif()

# Setup FetchContent for modern library management (we'll fetch GoogleTest first)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For now, we will hold off on adding ImGui flags until our core engine baseline is compiled.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Define our core executable target
add_executable(mercury_exchange src/main.cpp src/order_book.cpp)

# Include directories
target_include_directories(mercury_exchange PRIVATE include)

# Enable testing framework
enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)