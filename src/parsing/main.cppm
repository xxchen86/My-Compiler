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
import ll1_parser;

int main(int, char **) {

  std::istringstream iss;
  iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");

  Lexer lexer(iss);

  if (false) {
    LL1Parser parser(lexer);
    auto ast = parser.parse();
    ast->print(std::cout);
  } else {
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

    SLRParser parser(lexer, grammar);
    parser.buildParser();
    parser.parse();
  }

  return 0;
};
