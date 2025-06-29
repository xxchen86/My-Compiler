#pragma once

#include <istream>
#include <variant>
#include <iostream>

struct Token {
    using Value = std::variant<int>;

    enum Type: int {
        EndOfFile = EOF,
        Add = '+',
        Subtract = '-',
        Multiply = '*',
        Divide = '/',
        LeftParen = '(',
        RightParen = ')',

        Literal = 256, // Start from 256 to avoid conflicts with char tokens
    };

    Token(Type type, Value value = {})
        : type(type), value(value) {}

    Type type;
    Value value;
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
