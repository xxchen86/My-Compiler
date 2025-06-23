#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <iostream>

struct Token {
    enum class Type {
        Literal,
        // Identifier,
        // Keyword,
        Operator,
        // Type,
        EndOfFile
    };

    Type type;
    std::variant<int, char, std::string> value;

    static bool isCharOperator(char c) {
        switch(c) {
            case '+':
            case '-':
            case '*':
            case '/':
            case '=':
                return true;
            default:
                return false;
        }
    }

    // static bool isStringIdentifier(std::string_view str) {
    //     if (str.empty() || !std::isalpha(str[0])) return false;
    //     for (char c : str) {
    //         if (!std::isalnum(c) && c != '_') return false;
    //     }
    //     return true;
    // }

    // static bool isStringKeyword(std::string_view str) {
    //     static const std::string_view keywords[] = {
    //         "if", "else", "while", "for", "return", "function"
    //     };
    //     for (const auto& keyword : keywords) {
    //         if (str == keyword) return true;
    //     }
    //     return false;
    // }

    // static bool isStringType(std::string_view str) {
    //     static const std::string_view types[] = {
    //         "int", "float", "double", "char", "void"
    //     };
    //     for (const auto& type : types) {
    //         if (str == type) return true;
    //     }
    //     return false;
    // }
};


class Lexer {
public:
    Lexer() : bufferedToken(readToken()) {}

    Token getToken() {
        if (bufferedToken.type != Token::Type::EndOfFile) {
            auto ret = bufferedToken;
            bufferedToken = readToken();
            return ret;
        } else {
            return bufferedToken;
        }
    }

    Token peekToken() {
        return bufferedToken;
    }

private:
    /// Reads the next token from standard input. Return nullopt if EOF is reached.
    Token readToken() {
        char c = std::cin.peek();
        if (c == ' ' || c == '\n' || c == '\t') {
            std::cin.get(); // Skip whitespace
            return readToken(); // Recurse to get the next token
        } else if (c == EOF) {
            return Token{Token::Type::EndOfFile, {}};
        } else if (c > '0' && c < '9') {
            int number;
            std::cin >> number;
            return Token{Token::Type::Literal, number};
        } else if (Token::isCharOperator(c)) {
            std::cin.get();
            return Token{Token::Type::Operator, c};
        } else {
            throw std::runtime_error("Unexpected character: " + std::string(1, c));
            // std::string identifier;
            // std::cin >> identifier;
            // if (!Token::isStringIdentifier(identifier)) {
            //     throw std::runtime_error("Invalid identifier: " + identifier);
            // }
            // if (Token::isStringKeyword(identifier)) {
            //     return Token{Token::Type::Keyword, identifier};
            // } else if (Token::isStringType(identifier)) {
            //     return Token{Token::Type::Type, identifier};
            // } else {
            //     return Token{Token::Type::Identifier, identifier};
            // }
        }
    }

    Token bufferedToken;
};
