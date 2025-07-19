module;

#include <cstddef>
#include <functional>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

export module grammar;

export import symbol;

export {

  struct Production {
    Symbol head;
    std::vector<Symbol> body;

    bool operator==(const Production &other) const {
      return head == other.head && body == other.body;
    }

    friend std::ostream &operator<<(std::ostream &os, const Production &prod) {
      os << "Production(head=" << prod.head << ", body=[";
      for (auto &sym : prod.body) {
        os << sym << ", ";
      }
      os << "])";
      return os;
    }
  };

  template <> struct std::hash<Production> {
    size_t operator()(const Production &prod) const {
      size_t sizeInBytes = prod.body.size() * sizeof(Symbol);
      return std::hash<std::string>()(
                 {reinterpret_cast<const char *>(prod.body.data()),
                  sizeInBytes}) ^
             std::hash<Symbol>()(prod.head);
    }
  };

  struct Grammar {
  public:
    std::unordered_set<Symbol> terminals;
    std::unordered_set<Symbol> nonTerminals;
    std::unordered_set<Production> productions;
    Symbol startSymbol;

    std::unordered_set<Symbol> FIRST(const Symbol &symbol) const {
      std::unordered_set<Symbol> first;

      if (symbol.type == Symbol::Terminal) {
        first.insert(symbol);
      } else {

        auto headMatched = [&](const auto &prod) {
          return prod.head == symbol;
        };

        for (auto &prod : productions | std::views::filter(headMatched)) {
          if (prod.body == std::vector<Symbol>{Epsilon}) {
            first.insert(Epsilon);
          } else {
            for (auto &y : prod.body) {
              auto firstOfY = FIRST(y);
              first.insert(firstOfY.begin(), firstOfY.end());
              if (!firstOfY.contains(Epsilon))
                break;
            }
          }
        }
      }

      return first;
    }

    std::unordered_map<Symbol, std::unordered_set<Symbol>>
    FOLLOW_Table() const {
      std::unordered_map<Symbol, std::unordered_set<Symbol>> follow;
      // rule 1
      follow[startSymbol].insert(InputRightEndMarker);

      auto nonTerminalHead = [](auto &prod) {
        return prod.head.type == Symbol::NonTerminal;
      };

      auto addFollowOfHead = [&](auto &prod, auto &set) {
        const auto &followOfHead = follow[prod.head];
        auto sizeBeforeInsert = set.size();
        set.insert(followOfHead.begin(), followOfHead.end());
        auto sizeAfterInsert = set.size();
        return sizeBeforeInsert != sizeAfterInsert;
      };

      bool followTableChanged;
      do {
        followTableChanged = false;
        for (auto &prod : productions | std::views::filter(nonTerminalHead)) {

          for (size_t i = 0; i < prod.body.size(); ++i) {
            if (prod.body[i].type != Symbol::NonTerminal)
              continue;
            if (i + 1 != prod.body.size()) {
              auto first = FIRST(prod.body[i + 1]);
              // rule 2
              for (auto &a : first) {
                if (a != Epsilon && !follow[prod.body[i]].contains(a)) {
                  follow[prod.body[i]].insert(a);
                  followTableChanged = true;
                }
              }
              // rule3
              if (first.contains(Epsilon)) {
                followTableChanged =
                    followTableChanged ||
                    addFollowOfHead(prod, follow[prod.body[i]]);
              }
            } else {
              // rule3
              followTableChanged = followTableChanged ||
                                   addFollowOfHead(prod, follow[prod.body[i]]);
            }
          }
        }
      } while (followTableChanged);
      return follow;
    }
  };

  struct LR0Item {
    Production production;
    size_t dotPosition;

    std::optional<Symbol> getSymbolAfterDot() const {
      if (dotPosition < production.body.size()) {
        return production.body[dotPosition];
      } else {
        return std::nullopt;
      }
    }

    bool reducible() const { return dotPosition == production.body.size(); }

    bool operator==(const LR0Item &other) const {
      return production == other.production && dotPosition == other.dotPosition;
    }

    friend std::ostream &operator<<(std::ostream &os, const LR0Item &item) {
      return os << "LR0Item(production=" << item.production
                << ", dotPosition=" << item.dotPosition << ")";
    }
  };

  template <> struct std::hash<LR0Item> {
    size_t operator()(const LR0Item &item) const {
      return std::hash<Production>()(item.production) ^
             std::hash<size_t>()(item.dotPosition);
    }
  };

  template <> struct std::hash<std::pair<size_t, Symbol>> {
    size_t operator()(const std::pair<size_t, Symbol> &p) const {
      return std::hash<size_t>()(p.first) ^ std::hash<Symbol>()(p.second);
    }
  };

  struct SLRGrammar : public Grammar {
    using State = size_t;
    Production augmentedProduction;

    std::unordered_set<LR0Item>
    CLOSURE(std::unordered_set<LR0Item> itemSet) const {
      auto hasNonTerminalAfterDot = [](const LR0Item &item) {
        auto optionalSym = item.getSymbolAfterDot();
        if (!optionalSym)
          return false;
        auto sym = optionalSym.value();
        return sym.type == Symbol::NonTerminal;
      };

      bool itemSetChanged;
      do {
        itemSetChanged = false;
        for (auto &item :
             itemSet | std::views::filter(hasNonTerminalAfterDot)) {
          for (auto &production : productions) {
            if (production.head == item.getSymbolAfterDot()) {
              LR0Item newItem = {production, 0};
              if (!itemSet.contains(newItem)) {
                itemSet.insert(newItem);
                itemSetChanged = true;
              }
            }
          }
          if (itemSetChanged)
            break; // the iterator of itemSet had become invalid
        }
      } while (itemSetChanged);

      return itemSet;
    }

    std::unordered_set<LR0Item> GOTO(const std::unordered_set<LR0Item> &itemSet,
                                     const Symbol &symbol) const {
      std::unordered_set<LR0Item> kernels;
      for (auto &item : itemSet) {
        if (item.getSymbolAfterDot() == symbol)
          kernels.insert({item.production, item.dotPosition + 1});
      }
      return CLOSURE(std::move(kernels));
    }

    std::pair<std::vector<std::unordered_set<LR0Item>>,
              std::unordered_map<std::pair<State, Symbol>, State>>
    buildAutomaton() const {
      std::vector<std::unordered_set<LR0Item>> collection = {
          {CLOSURE({{augmentedProduction, 0}})}};
      std::unordered_map<std::pair<State, Symbol>, State> transitions;

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
