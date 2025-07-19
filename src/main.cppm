module;

#include <cstddef>
#include <format>
#include <iostream>
#include <sstream>
#include <unordered_set>

export module main;

import grammar;
import lexer;
import lr_parser;

int main(int, char **) {

  using SymbolType = Symbol<int>;
  using ProductionType = Production<SymbolType>;

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

  std::istringstream iss;
  iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");

  using TokenType = Token<SymbolType, int>;
  Lexer<TokenType> lexer(iss);

  SLRParser parser(lexer, grammar);
  parser.buildParser();
  parser.parse();
  return 0;
};
