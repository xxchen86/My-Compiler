cmake_minimum_required(VERSION 3.10.0)
project(MyCompiler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Executable
aux_source_directory(src SRC_LIST)
add_executable(MyCompiler ${SRC_LIST})


# Add Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# enable GoogleTest
enable_testing()

# Add test executable
aux_source_directory(tests TESTS_LIST)
add_executable(test_gtest ${TESTS_LIST})
target_link_libraries(test_gtest gtest gtest_main)
target_include_directories(test_gtest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Add tests
add_test(NAME test_gtest COMMAND test_gtest)

