module;

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

export module grammar;

import utils;

export {

  class Production : public StringLike<Production> {
  public:
    Production(std::string_view s) {
      std::istringstream iss(s.data());
      iss >> _head;
      std::string tmp;
      iss >> tmp;
      while (iss >> tmp) {
        _body.push_back(tmp);
      }
      updateRepr();
    }

    Production(std::string head, std::vector<std::string> body)
        : _head(std::move(head)), _body(std::move(body)) {
      updateRepr();
    }

    std::string_view head() const { return _head; }

    const std::vector<std::string> &body() const { return _body; }

  private:
    void updateRepr() {
      repr = this->_head + " ::=";
      for (auto &sym : this->_body) {
        repr += " ";
        repr += sym;
      }
    }

    std::string _head;
    std::vector<std::string> _body;
  };
  static_assert(std::movable<Production>);

  class Grammar {
  public:
    std::unordered_set<std::string> terminals;
    std::unordered_set<std::string> nonTerminals;
    std::unordered_set<Production> productions;
    std::string startSymbol;

    Grammar(std::unordered_set<std::string> terminals,
            std::unordered_set<std::string> nonTerminals,
            std::unordered_set<Production> productions, std::string startSymbol)
        : terminals(std::move(terminals)),
          nonTerminals(std::move(nonTerminals)),
          productions(std::move(productions)),
          startSymbol(std::move(startSymbol)) {}

    bool isTerminal(std::string_view symbol) const {
      return terminals.contains(symbol.data());
    }

    bool isNonTerminal(std::string_view symbol) const {
      return nonTerminals.contains(symbol.data());
    }

    std::unordered_set<std::string> FIRST(std::string_view symbol) const {
      std::unordered_set<std::string> first;
      if (isTerminal(symbol)) {
        first.insert(symbol.data());
      } else {
        for (auto &production : productions) {
          if (production.head() != symbol) {
            continue;
          } else if (production.body().size() == 1 &&
                     production.body().front() == "Epsilon") {
            first.insert("Epsilon");
          } else {
            for (auto &y : production.body()) {
              auto y1First = FIRST(y);
              first.insert(y1First.begin(), y1First.end());
              if (!y1First.contains("Epsilon"))
                break;
            }
          }
        }
      }

      return first;
    }

    std::unordered_map<std::string, std::unordered_set<std::string>>
    FOLLOW_Table() const {
      std::unordered_map<std::string, std::unordered_set<std::string>> follow;
      // rule 1
      follow[startSymbol].insert("$");
      bool changed;
      do {
        changed = false;
        for (auto &production : productions) {
          if (!isNonTerminal(production.head()))
            continue;
          for (size_t i = 0; i < production.body().size(); ++i) {
            if (!isNonTerminal(production.body()[i]))
              continue;
            if (i != production.body().size() - 1) {
              auto betaFirst = FIRST(production.body()[i + 1]);
              std::cout << production.body()[i + 1] << " FIRST:" << std::endl;
              for (auto &s : betaFirst) {
                std::cout << s << " ";
              }
              std::cout << std::endl;
              // rule 2
              for (auto &a : betaFirst) {
                if (a != "Epsilon" &&
                    !follow[production.body()[i]].contains(a)) {
                  follow[production.body()[i]].insert(a);
                  changed = true;
                }
              }
              // rule3
              if (betaFirst.contains("Epsilon")) {
                const auto &headFollow = follow[production.head().data()];
                if (!headFollow.empty()) {
                  auto sizeBeforeInsert = follow[production.body()[i]].size();
                  follow[production.body()[i]].insert(headFollow.begin(),
                                                      headFollow.end());
                  auto sizeAfterInsert = follow[production.body()[i]].size();
                  changed = sizeBeforeInsert != sizeAfterInsert;
                }
              }
            } else {
              // rule3
              const auto &headFollow = follow[production.head().data()];
              if (!headFollow.empty()) {
                auto sizeBeforeInsert = follow[production.body()[i]].size();
                follow[production.body()[i]].insert(headFollow.begin(),
                                                    headFollow.end());
                auto sizeAfterInsert = follow[production.body()[i]].size();
                changed = sizeBeforeInsert != sizeAfterInsert;
              }
            }
          }
        }
        for (auto &[k, v] : follow) {
          std::cout << k << " FOLLOW:" << std::endl;
          for (auto &s : v) {
            std::cout << s << " ";
          }
          std::cout << std::endl;
        }
        std::cout << "==========" << std::endl;
      } while (changed);
      return follow;
    }
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

    bool canReduce() const { return _dotPosition == _production.body().size(); }

  private:
    Production _production;
    size_t _dotPosition;
  };
  static_assert(std::movable<LR0Item>);

  template <> struct std::hash<std::pair<size_t, std::string>> {
    size_t operator()(const std::pair<size_t, std::string> &p) const {
      return std::hash<size_t>()(p.first) ^ std::hash<std::string>()(p.second);
    }
  };

  template <typename Item> class LRGrammar : public Grammar {
  public:
    LRGrammar(std::unordered_set<std::string> terminals,
              std::unordered_set<std::string> nonTerminals,
              std::unordered_set<Production> productions,
              Production startProduction)
        : Grammar(std::move(terminals), std::move(nonTerminals),
                  std::move(productions), startProduction.head().data()),
          startProduction(std::move(startProduction)) {}

    virtual ~LRGrammar() = default;
    virtual std::unordered_set<Item>
    closure(std::unordered_set<Item> itemSet) const = 0;
    virtual std::unordered_set<Item>
    GOTO(const std::unordered_set<Item> &itemSet,
         std::string_view symbol) const = 0;
    virtual std::pair<
        std::vector<std::unordered_set<Item>>,
        std::unordered_map<std::pair<size_t, std::string>, size_t>>
    constructCollectionOfItemSet() const = 0;

    Production startProduction;
  };

  class SLRGrammar : public LRGrammar<LR0Item> {
  public:
    using LRGrammar::LRGrammar;

    std::unordered_set<LR0Item>
    closure(std::unordered_set<LR0Item> itemSet) const override {
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

    std::unordered_set<LR0Item> GOTO(const std::unordered_set<LR0Item> &itemSet,
                                     std::string_view symbol) const override {
      std::unordered_set<LR0Item> kernels;
      for (auto &item : itemSet) {
        if (item.getSymbolAfterDot() == symbol)
          kernels.insert({item.production(), item.dotPosition() + 1});
      }
      return closure(std::move(kernels));
    }

    std::pair<std::vector<std::unordered_set<LR0Item>>,
              std::unordered_map<std::pair<size_t, std::string>, size_t>>
    constructCollectionOfItemSet() const override {
      std::vector<std::unordered_set<LR0Item>> collection = {
          {closure({{startProduction, 0}})}};
      std::unordered_map<std::pair<size_t, std::string>, size_t> transitions;

      for (size_t i = 0; i < collection.size(); ++i) {
        for (auto &terminal : terminals) {
          auto nextItemSet = GOTO(collection[i], terminal);
          if (nextItemSet.empty())
            continue;
          auto nextItemSetIter = std::ranges::find(collection, nextItemSet);
          if (nextItemSetIter == collection.end()) {
            transitions[{i, terminal}] = collection.size();
            collection.push_back(std::move(nextItemSet));
          } else {
            transitions[{i, terminal}] = nextItemSetIter - collection.begin();
          }
        }
        for (auto &nonTerminal : nonTerminals) {
          auto nextItemSet = GOTO(collection[i], nonTerminal);
          if (nextItemSet.empty())
            continue;
          auto nextItemSetIter = std::ranges::find(collection, nextItemSet);
          if (nextItemSetIter == collection.end()) {
            transitions[{i, nonTerminal}] = collection.size();
            collection.push_back(std::move(nextItemSet));
          } else {
            transitions[{i, nonTerminal}] =
                nextItemSetIter - collection.begin();
          }
        }
      }
      return {std::move(collection), std::move(transitions)};
    }
  };
}