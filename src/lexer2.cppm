module;

#include <iostream>
#include <istream>
#include <stdexcept>
#include <string_view>
#include <variant>

export module lexer2;

import grammar2;

export {

  constexpr Symbol Add(Symbol::Terminal, '+');
  constexpr Symbol Subtract(Symbol::Terminal, '-');
  constexpr Symbol Multiply(Symbol::Terminal, '*');
  constexpr Symbol Divide(Symbol::Terminal, '/');
  constexpr Symbol LeftParen(Symbol::Terminal, '(');
  constexpr Symbol RightParen(Symbol::Terminal, ')');
  constexpr Symbol Literal(Symbol::Terminal, 256);

  struct Token {
    using Value = std::variant<int>;

    Symbol type;
    Value value;
  };

  class Lexer {
  public:
    Lexer(std::istream &in) : in(in), bufferedToken(readToken()) {}

    Token getToken() {
      if (noTokenAnymore)
        throw std::runtime_error("no token anymore");
      if (bufferedToken.type != InputRightEndMarker) {
        auto ret = bufferedToken;
        bufferedToken = readToken();
        return ret;
      } else {
        noTokenAnymore = true;
        return bufferedToken;
      }
    }

    Token peekToken() { return bufferedToken; }

  private:
    Token readToken() {
      char c = in.peek();
      if (c == ' ' || c == '\n' || c == '\t') {
        in.get();
        return readToken();
      } else if (c > '0' && c < '9') {
        int number;
        in >> number;
        return Token{Literal, number};
      } else if (c == EOF) {
        return Token{InputRightEndMarker};
      } else {
        // a char is treated as a token
        return Token{Symbol(Symbol::Terminal, in.get())};
      }
    }

    std::istream &in;
    Token bufferedToken;
    bool noTokenAnymore = false;
  };
}