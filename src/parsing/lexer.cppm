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
  
  constexpr SymbolType Start{1, SymbolType::NonTerminal};
  constexpr SymbolType AddtiveExpr{2, SymbolType::NonTerminal};
  constexpr SymbolType MultiplicativeExpr{3, SymbolType::NonTerminal};
  constexpr SymbolType PrimaryExpr{4, SymbolType::NonTerminal};

  struct ReadToken {
    using TokenType = Token<SymbolType, int>;

    TokenType operator()(std::istream& in) {
      char c = in.peek();
      if (c == ' ' || c == '\n' || c == '\t') {
        in.get();
        return this->operator()(in);
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
  };

  using Lexer = LexerBase<ReadToken>;

}