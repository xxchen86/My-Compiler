module;

#include <concepts>
#include <variant>
#include <iostream>

export module lexer_base;

import grammar;

export {

  template <class _SymbolType, class... _ValueTypes> struct Token {
    using SymbolType = _SymbolType;
    using ValueType = std::variant<_ValueTypes...>;

    SymbolType type;
    ValueType value;
  };

  template <class _LexerType>
  concept LexerTypeTrait = requires(_LexerType lexer) {
    { lexer.getToken() } -> std::convertible_to<typename _LexerType::TokenType>;
    {
      lexer.peekToken()
    } -> std::convertible_to<typename _LexerType::TokenType>;
    { lexer.eof() } -> std::convertible_to<bool>;
  };

  template <class _ReadTokenFunc> class LexerBase {
  public:
    using TokenType = typename _ReadTokenFunc::TokenType;

    LexerBase(std::istream &in) : in(in) {
      bufferedToken = readToken(in);
    }

    TokenType getToken() {
      if (noTokenAnymore)
        throw std::runtime_error("no token anymore");
      if (bufferedToken.type != TokenType::SymbolType::InputRightEndMarker()) {
        auto ret = bufferedToken;
        bufferedToken = readToken(in);
        return ret;
      } else {
        noTokenAnymore = true;
        return bufferedToken;
      }
    }

    TokenType peekToken() { return bufferedToken; }

    bool eof() { return noTokenAnymore; }

  private:
    _ReadTokenFunc readToken;
    std::istream &in;
    TokenType bufferedToken;
    bool noTokenAnymore = false;
  };
}