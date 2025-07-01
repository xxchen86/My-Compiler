#include <gtest/gtest.h>

#include "LR_utils.h"
#include "pretty_print.h"

using namespace pretty_print;

TEST(Closure, BasicClosure) {
    std::vector<LR0Item> items = {
        {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 0}
    };

    auto closure = computeClosure(std::move(items));

    std::vector<LR0Item> expected = {
        {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 0},
        {{"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}}, 0},
        {{"AddtiveExpr", {"MultiplicativeExpr"}}, 0},
        {{"MultiplicativeExpr", {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}}, 0},
        {{"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}}, 0},
        {{"MultiplicativeExpr", {"PrimaryExpr"}}, 0},
        {{"PrimaryExpr", {"Literal"}}, 0},
        {{"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}, 0}
    };
    EXPECT_EQ(closure, expected);
}