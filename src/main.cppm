module;

#include <cstddef>
#include <format>
#include <iostream>
#include <sstream>
#include <unordered_set>

export module main;

#if 0

import grammar;
import lexer;
import ll1_parser;
import lr_parser;

int main(int, char **) {
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

  std::istringstream iss;
  iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");
//   iss.str("1 - 3");

  Lexer lexer(iss);
  SLRParser parser(lexer, grammar);
  parser.buildParser();
//   for (auto &[stateInputPair, act] : parser.actions) {
//     std::cout << "[" << stateInputPair.first << ", " << stateInputPair.second
//               << "]: ";
//     switch (act.type) {
//     case Action::Shift:
//       std::cout << " shift and jump to state " << std::get<size_t>(act.value)
//                 << std::endl;
//       break;
//     case Action::Reduce:
//       std::cout << " reduce by production " << std::get<Production>(act.value)
//                 << std::endl;
//       break;
//     case Action::Accept:
//       std::cout << " accept" << std::endl;
//       break;
//     }
//   }
  parser.parse();
  // auto nextState = grammar.GOTO(
  //     {
  //         {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 1}
  //     },
  //     "Add");
  // for (auto& item : nextState) {
  //     std::cout << item << std::endl;
  // }
  // std::istringstream iss;
  // iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");

  // Production production = {"dasf", {"dsadf", "ff"}};
  // std::cerr << std::format("{}", production);

  // Lexer lexer(iss);
  // LL1Parser parser(lexer);
  // auto expr = parser.parseAddtiveExpr();
  // if (expr) {
  //     expr->print(std::cout);
  // } else {
  //     std::cout << "No expression parsed.\n";
  // }
  return 0;
}

#else

import grammar2;
import lexer2;
import lr_parser2;

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
//   SLRGrammar grammar(
//       {Add, Subtract, Multiply, Divide, LeftParen, RightParen, Literal},
//       {"Start", "AddtiveExpr", "MultiplicativeExpr", "PrimaryExpr"},
//       {{"Start", {"AddtiveExpr"}},
//        {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
//        {"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
//        {"AddtiveExpr", {"MultiplicativeExpr"}},
//        {"MultiplicativeExpr",
//         {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
//        {"MultiplicativeExpr", {"MultiplicativeExpr", "Divide",
//        "PrimaryExpr"}},
//        {"MultiplicativeExpr", {"PrimaryExpr"}},
//        {"PrimaryExpr", {"Literal"}},
//        {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}},
//       {"Start", {"AddtiveExpr"}});

//   std::istringstream iss;
//   iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");
// //   iss.str("1 - 3");

//   Lexer lexer(iss);
//   SLRParser parser(lexer, grammar);
//   parser.buildParser();
//   for (auto &[stateInputPair, act] : parser.actions) {
//     std::cout << "[" << stateInputPair.first << ", " << stateInputPair.second
//               << "]: ";
//     switch (act.type) {
//     case Action::Shift:
//       std::cout << " shift and jump to state " << std::get<size_t>(act.value)
//                 << std::endl;
//       break;
//     case Action::Reduce:
//       std::cout << " reduce by production " <<
//       std::get<Production>(act.value)
//                 << std::endl;
//       break;
//     case Action::Accept:
//       std::cout << " accept" << std::endl;
//       break;
//     }
//   }
//   parser.parse();
// auto nextState = grammar.GOTO(
//     {
//         {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 1}
//     },
//     "Add");
// for (auto& item : nextState) {
//     std::cout << item << std::endl;
// }
// std::istringstream iss;
// iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");

// Production production = {"dasf", {"dsadf", "ff"}};
// std::cerr << std::format("{}", production);

// Lexer lexer(iss);
// LL1Parser parser(lexer);
// auto expr = parser.parseAddtiveExpr();
// if (expr) {
//     expr->print(std::cout);
// } else {
//     std::cout << "No expression parsed.\n";
// }


#endif