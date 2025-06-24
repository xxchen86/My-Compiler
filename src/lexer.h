#pragma once

#include <variant>
#include <iostream>

struct Token {
    using Value = std::variant<int>;

    enum Type: char {
        Literal,
        EndOfFile = EOF,
        Add = '+',
        Subtract = '-',
    };

    Token(Type type, Value value = {})
        : type(type), value(value) {}

    Type type;
    Value value;

    bool isOperator() const {
        return type == Add || type == Subtract;
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
        if (bufferedToken.type != EOF) {
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
    Token readToken() {
        char c = std::cin.peek();
        if (c == ' ' || c == '\n' || c == '\t') {
            std::cin.get();
            return readToken();
        } else if (c > '0' && c < '9') {
            int number;
            std::cin >> number;
            return Token{Token::Type::Literal, number};
        } else {
            return Token{Token::Type(std::cin.get())};
        }
    }

    Token bufferedToken;
};
