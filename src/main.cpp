#include <iostream>
#include <optional>
#include "lexer.h"
#include "parser.h"

int main(int, char**){
    Lexer lexer;
    LL1Parser parser(lexer);
    auto expression = parser.parseExpression();
    if (expression) {
        expression->print(std::cout);
    } else {
        std::cout << "No expression parsed.\n";
    }
    return 0;
}
