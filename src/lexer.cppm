module;

#include <iostream>
#include <istream>
#include <stdexcept>
#include <string_view>
#include <variant>

export module lexer;

export import lexer_base;
import grammar;

export {

  using SymbolType = Symbol<int>;

  constexpr SymbolType Add('+', SymbolType::Terminal);
  constexpr SymbolType Subtract('-', SymbolType::Terminal);
  constexpr SymbolType Multiply('*', SymbolType::Terminal);
  constexpr SymbolType Divide('/', SymbolType::Terminal);
  constexpr SymbolType LeftParen('(', SymbolType::Terminal);
  constexpr SymbolType RightParen(')', SymbolType::Terminal);
  constexpr SymbolType Literal(256, SymbolType::Terminal);

  template <class _TokenType> class Lexer {
  public:
    using TokenType = _TokenType;

    Lexer(std::istream &in) : in(in), bufferedToken(readToken()) {}

    TokenType getToken() {
      if (noTokenAnymore)
        throw std::runtime_error("no token anymore");
      if (bufferedToken.type != TokenType::SymbolType::InputRightEndMarker()) {
        auto ret = bufferedToken;
        bufferedToken = readToken();
        return ret;
      } else {
        noTokenAnymore = true;
        return bufferedToken;
      }
    }

    TokenType peekToken() { return bufferedToken; }

    bool eof() { return noTokenAnymore; }

  private:
    TokenType readToken() {
      char c = in.peek();
      if (c == ' ' || c == '\n' || c == '\t') {
        in.get();
        return readToken();
      } else if (c > '0' && c < '9') {
        int number;
        in >> number;
        return {Literal, number};
      } else if (c == EOF) {
        return {TokenType::SymbolType::InputRightEndMarker()};
      } else {
        // a char is treated as a token
        return {{in.get(), TokenType::SymbolType::Terminal}};
      }
    }

    std::istream &in;
    TokenType bufferedToken;
    bool noTokenAnymore = false;
  };
}