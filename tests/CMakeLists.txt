include(FetchContent)

#===============================================================================
# GoogleTest
#===============================================================================

FetchContent_Declare(

    googletest

    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

#===============================================================================
# Tests
#===============================================================================

add_executable(

    mercury_tests

    order_book_test.cpp
    matching_engine_test.cpp
    gateway_test.cpp
    risk_engine_test.cpp

    ../exchange/orderbook/order_book.cpp
    ../exchange/matching/matching_engine.cpp
    ../exchange/risk/risk_engine.cpp
    ../exchange/gateway/gateway.cpp
)

target_include_directories(

    mercury_tests

    PRIVATE

    ..
)

target_compile_options(

    mercury_tests

    PRIVATE

    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

target_link_libraries(

    mercury_tests

    PRIVATE

    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(
    mercury_tests
)