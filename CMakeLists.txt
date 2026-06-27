cmake_minimum_required(VERSION 3.20)

project(
    MercuryExchange
    VERSION 1.0.0
    LANGUAGES C CXX
)

#==============================================================================
# C++
#==============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#==============================================================================
# Threads
#==============================================================================

find_package(Threads REQUIRED)

#==============================================================================
# FetchContent
#==============================================================================

include(FetchContent)

#------------------------------------------------------------------------------
# GLFW
#------------------------------------------------------------------------------

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

#------------------------------------------------------------------------------
# Dear ImGui
#------------------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0
)

FetchContent_MakeAvailable(imgui)

#==============================================================================
# Dear ImGui Library
#==============================================================================

add_library(

    imgui_lib STATIC

    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(

    imgui_lib

    PUBLIC

    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(

    imgui_lib

    PUBLIC
    glfw
)

#==============================================================================
# Mercury Exchange
#==============================================================================

add_executable(

    mercury_exchange

    app/main.cpp
    app/application.cpp

    ui/dashboard.cpp

    infrastructure/threading/engine_thread.cpp

    exchange/orderbook/order_book.cpp
    exchange/matching/matching_engine.cpp
    exchange/risk/risk_engine.cpp
    exchange/gateway/gateway.cpp
)

target_include_directories(

    mercury_exchange

    PRIVATE

    .
)

target_link_libraries(

    mercury_exchange

    PRIVATE

    imgui_lib
    glfw
    Threads::Threads
)

#==============================================================================
# Benchmark
#==============================================================================

add_executable(

    mercury_benchmark

    benchmark/benchmark.cpp

    exchange/orderbook/order_book.cpp
    exchange/matching/matching_engine.cpp
    exchange/risk/risk_engine.cpp
    exchange/gateway/gateway.cpp
)

target_include_directories(

    mercury_benchmark

    PRIVATE

    .
)

target_link_libraries(

    mercury_benchmark

    PRIVATE

    Threads::Threads
)
#------------------------------------------------------------------------------
# macOS
#------------------------------------------------------------------------------

if(APPLE)

    target_compile_definitions(

        mercury_exchange

        PRIVATE

        GL_SILENCE_DEPRECATION
    )

    target_link_libraries(

        mercury_exchange

        PRIVATE

        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
        "-framework CoreVideo"
    )

endif()

#==============================================================================
# Tests
#==============================================================================

enable_testing()

add_subdirectory(tests)