cmake_minimum_required(VERSION 3.28.0)
project(MyCompiler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB MODULE_FILES src/*.cppm)

add_executable(MyCompiler)
target_sources(MyCompiler
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${MODULE_FILES}
)

enable_testing()

include(GoogleTest)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL file://${CMAKE_SOURCE_DIR}/googletest-release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Add test executable
aux_source_directory(tests TESTS_LIST)
add_executable(Tests)
target_sources(Tests
    PRIVATE
        ${TESTS_LIST}
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        src/utils.cppm
        src/grammar.cppm
)
target_link_libraries(Tests PRIVATE GTest::gtest_main)
target_include_directories(Tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
gtest_discover_tests(Tests)