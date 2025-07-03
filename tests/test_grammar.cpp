#include <gtest/gtest.h>

import grammar;

TEST(Production, getRepr) {
    Production production = {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}};
    EXPECT_EQ(production.getRepr(), std::string("AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr"));
}

TEST(LR0Item, getRepr1) {
    LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 0};
    EXPECT_EQ(item.getRepr(), std::string("AddtiveExpr ::= . AddtiveExpr Add MultiplicativeExpr"));
}

TEST(LR0Item, getRepr2) {
    LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 1};
    EXPECT_EQ(item.getRepr(), std::string("AddtiveExpr ::= AddtiveExpr . Add MultiplicativeExpr"));
}


TEST(LR0Item, getRepr3) {
    LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 3};
    EXPECT_EQ(item.getRepr(), std::string("AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr ."));
}
