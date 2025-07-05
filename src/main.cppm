module;

#include <iostream>
#include <sstream>
#include <format>

export module main;

import grammar;
import lexer;
import ll1_parser;

int main(int, char**){
    SLRGrammar grammar(
            {"Add", "Subtract", "Multiply", "Divide", "LeftParen",
            "RightParen", "Literal"},
            {"AddtiveExpr", "MultiplicativeExpr", "PrimaryExpr"},
            {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
            {"AddtiveExpr",
            {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
            {"AddtiveExpr", {"MultiplicativeExpr"}},
            {"MultiplicativeExpr",
            {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
            {"MultiplicativeExpr",
            {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
            {"MultiplicativeExpr", {"PrimaryExpr"}},
            {"PrimaryExpr", {"Literal"}},
            {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}});

    auto nextState = grammar.GOTO(
        {
            {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}}, 1}
        }, 
        "Add");
    for (auto& item : nextState) {
        std::cout << item << std::endl;
    }
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
