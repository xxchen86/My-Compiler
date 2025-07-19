#include <gtest/gtest.h>
#include <unordered_map>
#include <unordered_set>

import grammar;

TEST(Grammar2, FIRST) {
  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Mul(Symbol::Terminal, '*');
  constexpr Symbol LP(Symbol::Terminal, '(');
  constexpr Symbol RP(Symbol::Terminal, ')');
  constexpr Symbol Id(Symbol::Terminal, 256);

  constexpr Symbol E(Symbol::NonTerminal, 1);
  constexpr Symbol T(Symbol::NonTerminal, 2);
  constexpr Symbol EE(Symbol::NonTerminal, 3);
  constexpr Symbol F(Symbol::NonTerminal, 4);
  constexpr Symbol TT(Symbol::NonTerminal, 5);

  Grammar grammar({Add, Mul, LP, RP, Id}, {E, T, EE, F, TT},
                  {{E, {T, EE}},
                   {EE, {Add, T, EE}},
                   {EE, {Epsilon}},
                   {T, {F, TT}},
                   {TT, {Mul, F, TT}},
                   {TT, {Epsilon}},
                   {F, {LP, E, RP}},
                   {F, {Id}}},
                  E);

  EXPECT_EQ(grammar.FIRST(F), std::unordered_set<Symbol>({LP, Id}));
  EXPECT_EQ(grammar.FIRST(TT), std::unordered_set<Symbol>({Mul, Epsilon}));
}

TEST(Grammar2, FOLLOW) {
  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Mul(Symbol::Terminal, '*');
  constexpr Symbol LP(Symbol::Terminal, '(');
  constexpr Symbol RP(Symbol::Terminal, ')');
  constexpr Symbol Id(Symbol::Terminal, 256);

  constexpr Symbol E(Symbol::NonTerminal, 1);
  constexpr Symbol T(Symbol::NonTerminal, 2);
  constexpr Symbol EE(Symbol::NonTerminal, 3);
  constexpr Symbol F(Symbol::NonTerminal, 4);
  constexpr Symbol TT(Symbol::NonTerminal, 5);

  Grammar grammar({Add, Mul, LP, RP, Id}, {E, T, EE, F, TT},
                  {{E, {T, EE}},
                   {EE, {Add, T, EE}},
                   {EE, {Epsilon}},
                   {T, {F, TT}},
                   {TT, {Mul, F, TT}},
                   {TT, {Epsilon}},
                   {F, {LP, E, RP}},
                   {F, {Id}}},
                  E);

  auto follow = grammar.FOLLOW_Table();
  EXPECT_EQ(follow[E], std::unordered_set<Symbol>({RP, InputRightEndMarker}));
  EXPECT_EQ(follow[F],
            std::unordered_set<Symbol>({Add, Mul, RP, InputRightEndMarker}));
}

TEST(SLRGrammar2, CLOSURE) {
  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Subtract(Symbol::Terminal, '-');
  constexpr Symbol Multiply(Symbol::Terminal, '*');
  constexpr Symbol Divide(Symbol::Terminal, '/');
  constexpr Symbol LeftParen(Symbol::Terminal, '(');
  constexpr Symbol RightParen(Symbol::Terminal, ')');
  constexpr Symbol Literal(Symbol::Terminal, 256);

  constexpr Symbol Start(Symbol::NonTerminal, 1);
  constexpr Symbol AddtiveExpr(Symbol::NonTerminal, 2);
  constexpr Symbol MultiplicativeExpr(Symbol::NonTerminal, 3);
  constexpr Symbol PrimaryExpr(Symbol::NonTerminal, 4);

  SLRGrammar grammar(
      {{Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
       {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
       {{Start, {AddtiveExpr}},
        {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
        {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
        {AddtiveExpr, {MultiplicativeExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
        {MultiplicativeExpr, {PrimaryExpr}},
        {PrimaryExpr, {Literal}},
        {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}}},
      {Start, {AddtiveExpr}});

  auto closure = grammar.CLOSURE(
      {LR0Item({AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 0)});

  std::unordered_set<LR0Item> expected = {
      {{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 0},
      {{AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}}, 0},
      {{AddtiveExpr, {MultiplicativeExpr}}, 0},
      {{MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {PrimaryExpr}}, 0},
      {{PrimaryExpr, {Literal}}, 0},
      {{PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}, 0},
  };
  EXPECT_EQ(closure, expected);
}

TEST(SLRGrammar2, GOTO) {
  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Subtract(Symbol::Terminal, '-');
  constexpr Symbol Multiply(Symbol::Terminal, '*');
  constexpr Symbol Divide(Symbol::Terminal, '/');
  constexpr Symbol LeftParen(Symbol::Terminal, '(');
  constexpr Symbol RightParen(Symbol::Terminal, ')');
  constexpr Symbol Literal(Symbol::Terminal, 256);

  constexpr Symbol Start(Symbol::NonTerminal, 1);
  constexpr Symbol AddtiveExpr(Symbol::NonTerminal, 2);
  constexpr Symbol MultiplicativeExpr(Symbol::NonTerminal, 3);
  constexpr Symbol PrimaryExpr(Symbol::NonTerminal, 4);

  SLRGrammar grammar(
      {{Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
       {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
       {{Start, {AddtiveExpr}},
        {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
        {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
        {AddtiveExpr, {MultiplicativeExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
        {MultiplicativeExpr, {PrimaryExpr}},
        {PrimaryExpr, {Literal}},
        {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}}},
      {Start, {AddtiveExpr}});

  auto nextState =
      grammar.GOTO({{{Start, {AddtiveExpr}}, 1},
                    {{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 1}},
                   Add);

  std::unordered_set<LR0Item> expected = {
      {{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 2},
      {{MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {PrimaryExpr}}, 0},
      {{PrimaryExpr, {Literal}}, 0},
      {{PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}, 0},
  };
  EXPECT_EQ(nextState, expected);
}

TEST(SLRGrammar2, constructCollectionOfItemSet) {
  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Subtract(Symbol::Terminal, '-');
  constexpr Symbol Multiply(Symbol::Terminal, '*');
  constexpr Symbol Divide(Symbol::Terminal, '/');
  constexpr Symbol LeftParen(Symbol::Terminal, '(');
  constexpr Symbol RightParen(Symbol::Terminal, ')');
  constexpr Symbol Literal(Symbol::Terminal, 256);

  constexpr Symbol Start(Symbol::NonTerminal, 1);
  constexpr Symbol AddtiveExpr(Symbol::NonTerminal, 2);
  constexpr Symbol MultiplicativeExpr(Symbol::NonTerminal, 3);
  constexpr Symbol PrimaryExpr(Symbol::NonTerminal, 4);

  SLRGrammar grammar(
      {{Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
       {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
       {{Start, {AddtiveExpr}},
        {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
        {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
        {AddtiveExpr, {MultiplicativeExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
        {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
        {MultiplicativeExpr, {PrimaryExpr}},
        {PrimaryExpr, {Literal}},
        {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}}},
      {Start, {AddtiveExpr}});

  auto [collection, _] = grammar.buildAutomaton();
  // TODO
  EXPECT_EQ(std::vector<std::unordered_set<LR0Item>>{}, collection);
}