#include <iostream>
#include <optional>
#include <sstream>
#include "lexer.h"
#include "parser.h"

int main(int, char**){
    std::istringstream iss;
    iss.str("1 - (3 + 4)");

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
