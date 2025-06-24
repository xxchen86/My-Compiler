#pragma once



#include "ast.h"
#include "lexer.h"
#include <memory>


class LL1Parser {
public:
    LL1Parser(Lexer& lexer) : lexer(lexer) {}

    /// expr ::= literal | literal op expr
    std::unique_ptr<ast::Expression> parseExpression() {
        auto token = lexer.getToken();
        if (token.type == Token::Type::EndOfFile) {
            throw std::runtime_error("Unexpected end of input");
        }
        if (token.type == Token::Type::Literal) {
            auto literal = std::make_unique<ast::LiteralExpression>(std::get<int>(token.value));
            if (lexer.peekToken().isOperator()) {
                // If the next token is an operator, we need to parse a binary expression
                token = lexer.getToken(); // Consume the operator token
                char op = token.type;
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


