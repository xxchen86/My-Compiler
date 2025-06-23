#pragma once



#include "ast.h"
#include "lexer.h"
#include <memory>


class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer) {}

    std::unique_ptr<ast::Expression> parseExpression() {
        auto token = lexer.getToken();
        if (token.type == Token::Type::EndOfFile) {
            throw std::runtime_error("Unexpected end of input");
        }
        if (token.type == Token::Type::Literal) {
            auto literal = std::make_unique<ast::LiteralExpression>(std::get<int>(token.value));
            if (lexer.peekToken().type == Token::Type::Operator) {
                // If the next token is an operator, we need to parse a binary expression
                token = lexer.getToken(); // Consume the operator token
                char op = std::get<char>(token.value);
                auto right = parseExpression();
                return std::make_unique<ast::BinaryExpression>(std::move(literal), std::move(right), op);
            } else {
                return literal;
            }
        } else {
            throw std::runtime_error("Unexpected token type");
        }
    }

private:
    Lexer& lexer;
};


