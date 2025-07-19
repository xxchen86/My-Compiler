module;

#include <concepts>
#include <variant>

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
}