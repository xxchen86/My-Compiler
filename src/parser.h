#pragma once



#include "ast.h"
#include "lexer.h"
#include <cassert>
#include <memory>


class LL1Parser {
public:
    LL1Parser(Lexer& lexer) : lexer(lexer) {}

    /// PrimaryExpr ::= LiteralExpr | LeftParen AdditiveExpr RightParen
    std::unique_ptr<ast::Expression> parsePrimaryExpr() {
        auto token = lexer.getToken();
        if (token.type == Token::Literal) {
            return std::make_unique<ast::LiteralExpression>(std::get<int>(token.value));
        } else if (token.type == Token::LeftParen) {
            auto ret = parseAddtiveExpr();
            lexer.getToken();
            return ret;
        } else {
            throw std::runtime_error("Expected Literal or LeftParen");
        }
    }

    /// AdditiveExpr ::= PrimaryExpr AdditiveExprRemainder
    std::unique_ptr<ast::Expression> parseAddtiveExpr() {
        auto left = parsePrimaryExpr();
        return parseAddtiveExprRemainder(std::move(left));
    }

    /// AdditiveExprRemainder ::= Add PrimaryExpr AdditiveExprRemainder | Subtract PrimaryExpr AdditiveExprRemainder | ε
    std::unique_ptr<ast::Expression> parseAddtiveExprRemainder(std::unique_ptr<ast::Expression> left) {
        auto op = lexer.getToken();
        if (op.type != Token::Add && op.type != Token::Subtract) {
            return left;
        }
        auto right = parsePrimaryExpr();
        auto expr = std::make_unique<ast::BinaryExpression>(std::move(left), std::move(right), op.type);
        return parseAddtiveExprRemainder(std::move(expr));
    }

private:
    Lexer& lexer;
};


