#pragma once

#include <istream>
#include <variant>
#include <iostream>

struct Token {
    using Value = std::variant<int>;

    enum Type: char {
        Literal,
        Add = '+',
        Subtract = '-',
        LeftParen = '(',
        RightParen = ')',
        EndOfFile = EOF,
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
    Lexer(std::istream& in) : in(in), bufferedToken(readToken()) {}

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
        char c = in.peek();
        if (c == ' ' || c == '\n' || c == '\t') {
            in.get();
            return readToken();
        } else if (c > '0' && c < '9') {
            int number;
            in >> number;
            return Token{Token::Type::Literal, number};
        } else {
            // a char is treated as a token
            return Token{Token::Type(in.get())};
        }
    }

    std::istream& in;
    Token bufferedToken;
};
