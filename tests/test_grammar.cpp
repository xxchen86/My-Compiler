#include <gtest/gtest.h>
#include <unordered_map>
#include <unordered_set>

import grammar;

TEST(Grammar, FIRST) {
  using SymbolType = Symbol<int>;

  constexpr SymbolType Add{'+', SymbolType::Terminal};
  constexpr SymbolType Mul{'*', SymbolType::Terminal};
  constexpr SymbolType LP{'(', SymbolType::Terminal};
  constexpr SymbolType RP{')', SymbolType::Terminal};
  constexpr SymbolType Id{256, SymbolType::Terminal};

  constexpr SymbolType E{1, SymbolType::NonTerminal};
  constexpr SymbolType T{2, SymbolType::NonTerminal};
  constexpr SymbolType EE{3, SymbolType::NonTerminal};
  constexpr SymbolType F{4, SymbolType::NonTerminal};
  constexpr SymbolType TT{5, SymbolType::NonTerminal};

  Grammar grammar({Add, Mul, LP, RP, Id}, {E, T, EE, F, TT},
                  {{E, {T, EE}},
                   {EE, {Add, T, EE}},
                   {EE, {SymbolType::Epsilon()}},
                   {T, {F, TT}},
                   {TT, {Mul, F, TT}},
                   {TT, {SymbolType::Epsilon()}},
                   {F, {LP, E, RP}},
                   {F, {Id}}},
                  E);

  EXPECT_EQ(grammar.FIRST(F), std::unordered_set<SymbolType>({LP, Id}));
  EXPECT_EQ(grammar.FIRST(TT),
            std::unordered_set<SymbolType>({Mul, SymbolType::Epsilon()}));
}

TEST(Grammar, FOLLOW) {
  using SymbolType = Symbol<int>;

  constexpr SymbolType Add{'+', SymbolType::Terminal};
  constexpr SymbolType Mul{'*', SymbolType::Terminal};
  constexpr SymbolType LP{'(', SymbolType::Terminal};
  constexpr SymbolType RP{')', SymbolType::Terminal};
  constexpr SymbolType Id{256, SymbolType::Terminal};

  constexpr SymbolType E{1, SymbolType::NonTerminal};
  constexpr SymbolType T{2, SymbolType::NonTerminal};
  constexpr SymbolType EE{3, SymbolType::NonTerminal};
  constexpr SymbolType F{4, SymbolType::NonTerminal};
  constexpr SymbolType TT{5, SymbolType::NonTerminal};

  Grammar grammar({Add, Mul, LP, RP, Id}, {E, T, EE, F, TT},
                  {{E, {T, EE}},
                   {EE, {Add, T, EE}},
                   {EE, {SymbolType::Epsilon()}},
                   {T, {F, TT}},
                   {TT, {Mul, F, TT}},
                   {TT, {SymbolType::Epsilon()}},
                   {F, {LP, E, RP}},
                   {F, {Id}}},
                  E);

  auto follow = grammar.FOLLOW_Table();
  EXPECT_EQ(follow[E], std::unordered_set<SymbolType>(
                           {RP, SymbolType::InputRightEndMarker()}));
  EXPECT_EQ(follow[F], std::unordered_set<SymbolType>(
                           {Add, Mul, RP, SymbolType::InputRightEndMarker()}));
}

TEST(SLRGrammar, CLOSURE) {
  using SymbolType = Symbol<int>;

  constexpr SymbolType Add{'+', SymbolType::Terminal};
  constexpr SymbolType Subtract{'-', SymbolType::Terminal};
  constexpr SymbolType Multiply{'*', SymbolType::Terminal};
  constexpr SymbolType Divide{'/', SymbolType::Terminal};
  constexpr SymbolType LeftParen{'(', SymbolType::Terminal};
  constexpr SymbolType RightParen{')', SymbolType::Terminal};
  constexpr SymbolType Literal{256, SymbolType::Terminal};

  constexpr SymbolType Start{1, SymbolType::NonTerminal};
  constexpr SymbolType AddtiveExpr{2, SymbolType::NonTerminal};
  constexpr SymbolType MultiplicativeExpr{3, SymbolType::NonTerminal};
  constexpr SymbolType PrimaryExpr{4, SymbolType::NonTerminal};

  Grammar basicGrammar{
      {Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
      {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
      {{Start, {AddtiveExpr}},
       {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
       {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
       {AddtiveExpr, {MultiplicativeExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
       {MultiplicativeExpr, {PrimaryExpr}},
       {PrimaryExpr, {Literal}},
       {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}},
      Start};

  SLRGrammar grammar{std::move(basicGrammar), {Start, {AddtiveExpr}}};

  auto closure = grammar.CLOSURE(
      {{{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 0}});

  std::unordered_set<decltype(grammar)::LR0ItemType> expected = {
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

TEST(SLRGrammar, GOTO) {
  using SymbolType = Symbol<int>;

  constexpr SymbolType Add{'+', SymbolType::Terminal};
  constexpr SymbolType Subtract{'-', SymbolType::Terminal};
  constexpr SymbolType Multiply{'*', SymbolType::Terminal};
  constexpr SymbolType Divide{'/', SymbolType::Terminal};
  constexpr SymbolType LeftParen{'(', SymbolType::Terminal};
  constexpr SymbolType RightParen{')', SymbolType::Terminal};
  constexpr SymbolType Literal{256, SymbolType::Terminal};

  constexpr SymbolType Start{1, SymbolType::NonTerminal};
  constexpr SymbolType AddtiveExpr{2, SymbolType::NonTerminal};
  constexpr SymbolType MultiplicativeExpr{3, SymbolType::NonTerminal};
  constexpr SymbolType PrimaryExpr{4, SymbolType::NonTerminal};

  Grammar basicGrammar{
      {Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
      {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
      {{Start, {AddtiveExpr}},
       {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
       {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
       {AddtiveExpr, {MultiplicativeExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
       {MultiplicativeExpr, {PrimaryExpr}},
       {PrimaryExpr, {Literal}},
       {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}},
      Start};

  SLRGrammar grammar{std::move(basicGrammar), {Start, {AddtiveExpr}}};

  auto nextState =
      grammar.GOTO({{{Start, {AddtiveExpr}}, 1},
                    {{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 1}},
                   Add);

  std::unordered_set<decltype(grammar)::LR0ItemType> expected = {
      {{AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}}, 2},
      {{MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}}, 0},
      {{MultiplicativeExpr, {PrimaryExpr}}, 0},
      {{PrimaryExpr, {Literal}}, 0},
      {{PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}, 0},
  };
  EXPECT_EQ(nextState, expected);
}

TEST(SLRGrammar, buildAutomaton) {
  using SymbolType = Symbol<int>;

  constexpr SymbolType Add{'+', SymbolType::Terminal};
  constexpr SymbolType Subtract{'-', SymbolType::Terminal};
  constexpr SymbolType Multiply{'*', SymbolType::Terminal};
  constexpr SymbolType Divide{'/', SymbolType::Terminal};
  constexpr SymbolType LeftParen{'(', SymbolType::Terminal};
  constexpr SymbolType RightParen{')', SymbolType::Terminal};
  constexpr SymbolType Literal{256, SymbolType::Terminal};

  constexpr SymbolType Start{1, SymbolType::NonTerminal};
  constexpr SymbolType AddtiveExpr{2, SymbolType::NonTerminal};
  constexpr SymbolType MultiplicativeExpr{3, SymbolType::NonTerminal};
  constexpr SymbolType PrimaryExpr{4, SymbolType::NonTerminal};

  Grammar basicGrammar{
      {Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
      {Start, AddtiveExpr, MultiplicativeExpr, PrimaryExpr},
      {{Start, {AddtiveExpr}},
       {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
       {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
       {AddtiveExpr, {MultiplicativeExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
       {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
       {MultiplicativeExpr, {PrimaryExpr}},
       {PrimaryExpr, {Literal}},
       {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}},
      Start};

  SLRGrammar grammar{std::move(basicGrammar), {Start, {AddtiveExpr}}};

  auto [collection, _] = grammar.buildAutomaton();
  // TODO
  EXPECT_EQ(std::vector<std::unordered_set<decltype(grammar)::LR0ItemType>>{},
            collection);
}