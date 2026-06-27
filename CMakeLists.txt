cmake_minimum_required(VERSION 3.20)

project(
    MercuryExchange
    VERSION 1.0.0
    LANGUAGES CXX
)

#===============================================================================
# C++
#===============================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

#===============================================================================
# macOS
#===============================================================================

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

#===============================================================================
# Dependencies
#===============================================================================

include(FetchContent)

#------------------------------------------------------------------------------
# GLFW
#------------------------------------------------------------------------------

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
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
    GIT_TAG v1.90.4
)

FetchContent_MakeAvailable(imgui)

#===============================================================================
# ImGui Library
#===============================================================================

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
    SYSTEM PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(
    imgui_lib
    PRIVATE
    glfw
)

#===============================================================================
# Mercury Engine
#===============================================================================

add_executable(

    mercury_exchange

    app/main.cpp
    app/application.cpp


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

target_compile_options(

    mercury_exchange

    PRIVATE

    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")

    target_compile_options(

        mercury_exchange

        PRIVATE

        -O3
        -march=native
    )

endif()

target_link_libraries(

    mercury_exchange

    PRIVATE

    imgui_lib
    glfw
)

if(APPLE)

    target_link_libraries(

        mercury_exchange

        PRIVATE

        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )

else()

    target_link_libraries(

        mercury_exchange

        PRIVATE

        GL
    )

endif()

#===============================================================================
# Tests
#===============================================================================

enable_testing()

add_subdirectory(tests)