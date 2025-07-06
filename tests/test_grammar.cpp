#include <gtest/gtest.h>
#include <unordered_map>
#include <unordered_set>

import grammar;

TEST(Production, getRepr) {
  Production production = {"AddtiveExpr",
                           {"AddtiveExpr", "Add", "MultiplicativeExpr"}};
  EXPECT_EQ(production.getRepr(),
            std::string("AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr"));
}

TEST(Production, ConstructFromString) {
  Production production1 = {"AddtiveExpr",
                            {"AddtiveExpr", "Add", "MultiplicativeExpr"}};
  Production production2 = {
      "AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr"};
  EXPECT_EQ(production1, production2);
}

TEST(LR0Item, getRepr1) {
  LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
                  0};
  EXPECT_EQ(
      item.getRepr(),
      std::string("AddtiveExpr ::= . AddtiveExpr Add MultiplicativeExpr"));
}

TEST(LR0Item, getRepr2) {
  LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
                  1};
  EXPECT_EQ(
      item.getRepr(),
      std::string("AddtiveExpr ::= AddtiveExpr . Add MultiplicativeExpr"));
}

TEST(LR0Item, getRepr3) {
  LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
                  3};
  EXPECT_EQ(
      item.getRepr(),
      std::string("AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr ."));
}

TEST(LR0Item, canReduce) {
  LR0Item item = {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
                  3};
  EXPECT_TRUE(item.canReduce());
}

TEST(Grammar, FIRST) {
    Grammar grammar(
        {"+", "*", "(", ")", "id"},
        {"E", "T", "EE", "F", "TT"},
        {
            {"E", {"T", "EE"}},
            {"EE", {"+", "T", "EE"}},
            {"EE", {"Epsilon"}},
            {"T", {"F", "TT"}},
            {"TT", {"*", "F", "TT"}},
            {"TT", {"Epsilon"}},
            {"F", {"(", "E", ")"}},
            {"F", {"id", }}
        },
        "E"
    );

    EXPECT_EQ(grammar.FIRST("F"), std::unordered_set<std::string>({"(", "id"}));
    EXPECT_EQ(grammar.FIRST("TT"), std::unordered_set<std::string>({"*", "Epsilon"}));
}

TEST(Grammar, FOLLOW) {
    Grammar grammar(
        {"+", "*", "(", ")", "id"},
        {"E", "T", "EE", "F", "TT"},
        {
            {"E", {"T", "EE"}},
            {"EE", {"+", "T", "EE"}},
            {"EE", {"Epsilon"}},
            {"T", {"F", "TT"}},
            {"TT", {"*", "F", "TT"}},
            {"TT", {"Epsilon"}},
            {"F", {"(", "E", ")"}},
            {"F", {"id", }}
        },
        "E"
    );

    auto follow = grammar.FOLLOW_Table();
    EXPECT_EQ(follow["E"], std::unordered_set<std::string>({")", "$"}));
    EXPECT_EQ(follow["F"], std::unordered_set<std::string>({"+", "*", ")", "$"}));

}

TEST(SLRGrammar, closure) {
  SLRGrammar grammar(
      {"Add", "Subtract", "Multiply", "Divide", "LeftParen", "RightParen",
       "Literal"},
      {"Start", "AddtiveExpr", "MultiplicativeExpr", "PrimaryExpr"},
      {{"Start", {"AddtiveExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"MultiplicativeExpr"}},
       {"MultiplicativeExpr",
        {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"PrimaryExpr"}},
       {"PrimaryExpr", {"Literal"}},
       {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}},
      {"Start", {"AddtiveExpr"}});

  auto closure = grammar.closure({LR0Item(
      {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 0)});

  std::unordered_set<LR0Item> expected = {
      {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 0},
      {{"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}}, 0},
      {{"AddtiveExpr", {"MultiplicativeExpr"}}, 0},
      {{"MultiplicativeExpr",
        {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
       0},
      {{"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
       0},
      {{"MultiplicativeExpr", {"PrimaryExpr"}}, 0},
      {{"PrimaryExpr", {"Literal"}}, 0},
      {{"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}, 0},
  };
  EXPECT_EQ(closure, expected);
}

TEST(SLRGrammar, GOTO) {
  SLRGrammar grammar(
      {"Add", "Subtract", "Multiply", "Divide", "LeftParen", "RightParen",
       "Literal"},
      {"Start", "AddtiveExpr", "MultiplicativeExpr", "PrimaryExpr"},
      {{"Start", {"AddtiveExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"MultiplicativeExpr"}},
       {"MultiplicativeExpr",
        {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"PrimaryExpr"}},
       {"PrimaryExpr", {"Literal"}},
       {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}},
      {"Start", {"AddtiveExpr"}});

  auto nextState = grammar.GOTO(
      {{{"Start", {"AddtiveExpr"}}, 1},
       {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 1}},
      "Add");

  std::unordered_set<LR0Item> expected = {
      {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 2},
      {{"MultiplicativeExpr",
        {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
       0},
      {{"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
       0},
      {{"MultiplicativeExpr", {"PrimaryExpr"}}, 0},
      {{"PrimaryExpr", {"Literal"}}, 0},
      {{"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}, 0},
  };
  EXPECT_EQ(nextState, expected);
}

TEST(SLRGrammar, constructCollectionOfItemSet) {
      SLRGrammar grammar(
      {"Add", "Subtract", "Multiply", "Divide", "LeftParen", "RightParen",
       "Literal"},
      {"Start", "AddtiveExpr", "MultiplicativeExpr", "PrimaryExpr"},
      {{"Start", {"AddtiveExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
       {"AddtiveExpr", {"MultiplicativeExpr"}},
       {"MultiplicativeExpr",
        {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
       {"MultiplicativeExpr", {"PrimaryExpr"}},
       {"PrimaryExpr", {"Literal"}},
       {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}},
      {"Start", {"AddtiveExpr"}});

      auto [collection, _] = grammar.constructCollectionOfItemSet();
      // TODO
      EXPECT_EQ(std::vector<std::unordered_set<LR0Item>>{}, collection);
}