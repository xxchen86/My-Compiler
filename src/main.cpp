#include <iostream>
#include <optional>
#include "lexer.h"

int main(int, char**){
    Lexer lexer;
    std::optional<Token> token;
    while((token = lexer.nextToken())) {
        switch(token->type) {
            case Token::Type::Literal:
                std::cout << "Literal: " << std::get<int>(token->value) << std::endl;
                break;
            // case Token::Type::Identifier:
            //     std::cout << "Identifier: " << std::get<std::string>(token->value) << std::endl;
            //     break;
            case Token::Type::Operator:
                std::cout << "Operator: " << std::get<char>(token->value) << std::endl;
                break;
            // case Token::Type::Keyword:
            //     std::cout << "Keyword: " << std::get<std::string>(token->value) << std::endl;
            //     break;
            default:
                break;
        }
    }
    return 0;
}
