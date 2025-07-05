module;

#include <cstddef>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>
#include <algorithm>

export module grammar;

import utils;

export {

  class Production : public StringLike<Production> {
  public:
    Production(std::string head, std::vector<std::string> body)
        : _head(std::move(head)), _body(std::move(body)) {
      repr = this->_head + " ::=";
      for (auto &sym : this->_body) {
        repr += " ";
        repr += sym;
      }
    }

    std::string_view head() const { return _head; }

    const std::vector<std::string> &body() const { return _body; }

  private:
    std::string _head;
    std::vector<std::string> _body;
  };
  static_assert(std::is_move_assignable_v<Production>);
  static_assert(std::is_move_constructible_v<Production>);

  class Grammar {
  public:
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::unordered_set<Production> productions;

    Grammar(std::unordered_set<std::string> terminals,
            std::unordered_set<std::string> nonTerminals,
            std::unordered_set<Production> productions)
        : terminals(terminals), nonTerminals(nonTerminals),
          productions(productions) {}

    bool isTerminal(std::string_view symbol) {
      return terminals.contains(symbol.data());
    }

    bool isNonTerminal(std::string_view symbol) {
      return nonTerminals.contains(symbol.data());
    }

  private:
  };

  class LR0Item : public StringLike<LR0Item> {
  public:
    LR0Item(Production production, size_t dotPosition)
        : _production(std::move(production)), _dotPosition(dotPosition) {
      repr = this->_production.head();
      repr += " ::=";
      for (size_t i = 0; i < this->_production.body().size(); ++i) {
        if (i == dotPosition) {
          repr += " .";
        }
        repr += " ";
        repr += this->_production.body()[i];
      }
      if (dotPosition == this->_production.body().size())
        repr += " .";
    }

    std::optional<std::string_view> getSymbolAfterDot() const {
      if (_dotPosition < _production.body().size()) {
        return _production.body()[_dotPosition];
      } else {
        return std::nullopt;
      }
    }

    const Production &production() const { return _production; }

    size_t dotPosition() const { return _dotPosition; }

  private:
    Production _production;
    size_t _dotPosition;
  };
  static_assert(std::is_move_assignable_v<LR0Item>);
  static_assert(std::is_move_constructible_v<LR0Item>);

  class SLRGrammar : public Grammar {
  public:
    using Grammar::Grammar;

    auto closure(std::unordered_set<LR0Item> itemSet) {
      auto hasNonTerminalAfterDot = [&](const LR0Item &item) {
        auto hasSym = item.getSymbolAfterDot();
        if (!hasSym)
          return false;
        auto sym = hasSym.value();
        return isNonTerminal(sym);
      };

      bool itemSetChanged;
      do {
        itemSetChanged = false;
        for (auto &item :
             itemSet | std::views::filter(hasNonTerminalAfterDot)) {
          auto isHeadMatched = [&](const Production &p) {
            return p.head() == item.getSymbolAfterDot();
          };
          for (auto &production :
               productions | std::views::filter(isHeadMatched)) {
            LR0Item newItem = {production, 0};
            if (!itemSet.contains(newItem)) {
              itemSet.insert(newItem);
              itemSetChanged = true;
            }
          }
          if (itemSetChanged)
            break;
        }
      } while (itemSetChanged);

      return itemSet;
    }

    auto GOTO(const std::unordered_set<LR0Item> &itemSet, std::string_view symbol) {
      std::unordered_set<LR0Item> kernels;
      for (auto &item : itemSet) {
        if (item.getSymbolAfterDot() == symbol)
          kernels.insert({item.production(), item.dotPosition() + 1});
      }
      return closure(std::move(kernels));
    }
  };
}