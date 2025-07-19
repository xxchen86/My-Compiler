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

  constexpr Symbol Start(Symbol::NonTerminal, 1);
  constexpr Symbol AddtiveExpr(Symbol::NonTerminal, 2);
  constexpr Symbol MultiplicativeExpr(Symbol::NonTerminal, 3);
  constexpr Symbol PrimaryExpr(Symbol::NonTerminal, 4);

  std::unordered_set<Symbol> terminals = {
      Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal};
  std::unordered_set<Symbol> nonTerminals = {Start, AddtiveExpr,
                                             MultiplicativeExpr, PrimaryExpr};
  std::unordered_set<Production> productions = {
      {Start, {AddtiveExpr}},
      {AddtiveExpr, {AddtiveExpr, Add, MultiplicativeExpr}},
      {AddtiveExpr, {AddtiveExpr, Subtract, MultiplicativeExpr}},
      {AddtiveExpr, {MultiplicativeExpr}},
      {MultiplicativeExpr, {MultiplicativeExpr, Multiply, PrimaryExpr}},
      {MultiplicativeExpr, {MultiplicativeExpr, Divide, PrimaryExpr}},
      {MultiplicativeExpr, {PrimaryExpr}},
      {PrimaryExpr, {Literal}},
      {PrimaryExpr, {LeftParen, AddtiveExpr, RightParen}}};
  SLRGrammar grammar({terminals, nonTerminals, productions, Start},
                     {Start, {AddtiveExpr}});

  std::istringstream iss;
  iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");
//   iss.str("1");

  Lexer lexer(iss);
  SLRParser parser(lexer, grammar);
  parser.buildParser();
  parser.parse();
  return 0;
};
