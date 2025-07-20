cmake_minimum_required(VERSION 3.28.0)
project(MyCompiler VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(MyCompiler)
target_sources(MyCompiler
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        src/parsing/common/grammar.cppm
        src/parsing/common/lr_parser.cppm
        src/parsing/common/lexer_base.cppm
        src/parsing/ast.cppm
        src/parsing/lexer.cppm
        src/parsing/ll1_parser.cppm
        src/parsing/main.cppm
)


###  TESTS

enable_testing()

include(GoogleTest)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL file://${CMAKE_SOURCE_DIR}/googletest-release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(TestCommon)
target_sources(TestCommon
    PRIVATE
        tests/test_grammar.cpp
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        src/parsing/common/grammar.cppm
        src/parsing/common/lr_parser.cppm
        src/parsing/common/lexer_base.cppm
)
target_link_libraries(TestCommon PRIVATE GTest::gtest_main)
gtest_discover_tests(TestCommon)

add_executable(TestFourFundamentalArithmetic)
target_sources(TestFourFundamentalArithmetic
    PRIVATE
        tests/test_four_fundamental_arithmetic.cpp
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        src/parsing/common/grammar.cppm
        src/parsing/common/lr_parser.cppm
        src/parsing/common/lexer_base.cppm
        src/parsing/ast.cppm
        src/parsing/lexer.cppm
)
target_link_libraries(TestFourFundamentalArithmetic PRIVATE GTest::gtest_main)
gtest_discover_tests(TestFourFundamentalArithmetic)
