module;

#include <iostream>
#include <optional>
#include <sstream>


export module main;

import grammar;
import lexer;
import ll1_parser;

int main(int, char**){
    
    std::istringstream iss;
    iss.str("1 - ((3 * 10) + 4 / 1) + 5 * (6+1)");

    Lexer lexer(iss);
    LL1Parser parser(lexer);
    auto expr = parser.parseAddtiveExpr();
    if (expr) {
        expr->print(std::cout);
    } else {
        std::cout << "No expression parsed.\n";
    }
    return 0;
}
