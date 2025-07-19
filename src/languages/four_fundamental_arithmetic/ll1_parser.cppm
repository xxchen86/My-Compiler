module;

#include <variant>
#include <cassert>
#include <memory>

export module ll1_parser;

import lexer;
import ast;

export {

    /// \brief LL(1) parser for a simple arithmetic expression language.
    /// 
    /// The grammar is:
    ///     AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr | AddtiveExpr Subtract MultiplicativeExpr | MultiplicativeExpr
    ///     MultiplicativeExpr ::= MultiplicativeExpr Multiply PrimaryExpr | MultiplicativeExpr Divide PrimaryExpr | PrimaryExpr
    ///     PrimaryExpr ::= Literal | LeftParen AddtiveExpr RightParen
    class LL1Parser {
    public:
        LL1Parser(Lexer& lexer) : lexer(lexer) {}

        std::unique_ptr<ast::Expression> parse() {
            return parseAddtiveExpr();
        }

        /// PrimaryExpr ::= Literal | LeftParen AdditiveExpr RightParen
        std::unique_ptr<ast::Expression> parsePrimaryExpr() {
            auto token = lexer.getToken();
            if (token.type == Literal) {
                return std::make_unique<ast::LiteralExpression>(std::get<int>(token.value));
            } else if (token.type == LeftParen) {
                auto ret = parseAddtiveExpr();
                lexer.getToken();
                return ret;
            } else {
                throw std::runtime_error("Expected Literal or LeftParen");
            }
        }

        /// MultiplicativeExpr ::= PrimaryExpr MultiplicativeExprRemainder
        std::unique_ptr<ast::Expression> parseMultiplicativeExpr() {
            auto left = parsePrimaryExpr();
            return parseMultiplicativeExprRemainder(std::move(left));
        }

        /// MultiplicativeExprRemainder ::= Multiply PrimaryExpr MultiplicativeExprRemainder | Divide PrimaryExpr MultiplicativeExprRemainder | ε
        std::unique_ptr<ast::Expression> parseMultiplicativeExprRemainder(std::unique_ptr<ast::Expression> left) {
            auto token = lexer.peekToken();
            if (token.type != Multiply && token.type != Divide) {
                return left;
            }
            auto op = lexer.getToken();
            auto right = parsePrimaryExpr();
            auto expr = std::make_unique<ast::BinaryExpression>(std::move(left), std::move(right), std::get<int>(op.value));
            return parseMultiplicativeExprRemainder(std::move(expr));
        }

        /// AdditiveExpr ::= MultiplicativeExpr AdditiveExprRemainder
        std::unique_ptr<ast::Expression> parseAddtiveExpr() {
            auto left = parseMultiplicativeExpr();
            return parseAddtiveExprRemainder(std::move(left));
        }

        /// AdditiveExprRemainder ::= Add MultiplicativeExpr AdditiveExprRemainder | Subtract MultiplicativeExpr AdditiveExprRemainder | ε
        std::unique_ptr<ast::Expression> parseAddtiveExprRemainder(std::unique_ptr<ast::Expression> left) {
            auto token = lexer.peekToken();
            if (token.type != Add && token.type != Subtract) {
                return left;
            }
            auto op = lexer.getToken();
            auto right = parseMultiplicativeExpr();
            auto expr = std::make_unique<ast::BinaryExpression>(std::move(left), std::move(right), std::get<int>(op.value));
            return parseAddtiveExprRemainder(std::move(expr));
        }

    private:
        Lexer& lexer;
    };


}