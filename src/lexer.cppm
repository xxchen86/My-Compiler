module;

#include <istream>
#include <variant>
#include <iostream>
#include <string_view>
#include <stdexcept>

export module lexer;

export {

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

        std::string str() const {
            switch (type) {
            case EndOfFile:
                return "$";
            case Add:
                return "Add";
            case Subtract:
                return "Subtract";
            case Multiply:
                return "Multiply";
            case Divide:
                return "Divide";
            case LeftParen:
                return "LeftParen";
            case RightParen:
                return "RightParen";
            case Literal:
                return "Literal";
            }
        }

        Type type;
        Value value;
    };


    class Lexer {
    public:
        Lexer(std::istream& in) : in(in), bufferedToken(readToken()) {}

        Token getToken() {
            if (noTokenAnymore)
                throw std::runtime_error("no token anymore");
            if (bufferedToken.type != EOF) {
                auto ret = bufferedToken;
                bufferedToken = readToken();
                return ret;
            } else {
                noTokenAnymore = true;
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
        bool noTokenAnymore = false;
    };

}