module;

#include <cstddef>
#include <functional>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

export module grammar;

export {
  template <class _IdType> struct Symbol {
    using IdType = _IdType;
    enum Kind { Terminal, NonTerminal, _Epsilon, _InputRightEndMarker };
    IdType id;
    Kind type;

    bool operator==(const Symbol &other) const {
      return type == other.type && id == other.id;
    }

    friend std::ostream &operator<<(std::ostream &os, const Symbol &sym) {
      return os << "Symbol(type=" << sym.type << ", id=" << sym.id << ")";
    }

    static constexpr Symbol Epsilon() { return {.type = Kind::_Epsilon}; }

    static constexpr Symbol InputRightEndMarker() {
      return {.type = Kind::_InputRightEndMarker};
    }
  };

  template <class IdType> struct std::hash<Symbol<IdType>> {
    size_t operator()(const Symbol<IdType> &symbol) const {
      size_t seed = std::hash<int>()(symbol.type);
      seed ^=
          std::hash<int>()(symbol.id) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }
  };

  template <class _SymbolType> struct Production {
    using SymbolType = _SymbolType;
    SymbolType head;
    std::vector<SymbolType> body;

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

  template <class SymbolType> struct std::hash<Production<SymbolType>> {
    size_t operator()(const Production<SymbolType> &prod) const {
      size_t seed = std::hash<SymbolType>()(prod.head);
      for (const auto &symbol : prod.body) {
        // Boost-style hash combination
        seed ^= std::hash<SymbolType>()(symbol) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
      }
      return seed;
    }
  };

  template <class _SymbolType> struct Grammar {
    using SymbolType = _SymbolType;
    using ProductionType = Production<SymbolType>;
    std::unordered_set<SymbolType> terminals;
    std::unordered_set<SymbolType> nonTerminals;
    std::unordered_set<ProductionType> productions;
    SymbolType startSymbol;

    std::unordered_set<SymbolType> FIRST(const SymbolType &symbol) const {
      std::unordered_set<SymbolType> first;

      if (symbol.type == SymbolType::Terminal) {
        first.insert(symbol);
      } else {

        auto headMatched = [&](const auto &prod) {
          return prod.head == symbol;
        };

        for (auto &prod : productions | std::views::filter(headMatched)) {
          if (prod.body == std::vector<SymbolType>{SymbolType::Epsilon()}) {
            first.insert(SymbolType::Epsilon());
          } else {
            for (auto &y : prod.body) {
              auto firstOfY = FIRST(y);
              first.insert(firstOfY.begin(), firstOfY.end());
              if (!firstOfY.contains(SymbolType::Epsilon()))
                break;
            }
          }
        }
      }

      return first;
    }

    std::unordered_map<SymbolType, std::unordered_set<SymbolType>>
    FOLLOW_Table() const {
      std::unordered_map<SymbolType, std::unordered_set<SymbolType>> follow;
      // rule 1
      follow[startSymbol].insert(SymbolType::InputRightEndMarker());

      auto nonTerminalHead = [](auto &prod) {
        return prod.head.type == SymbolType::NonTerminal;
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
            if (prod.body[i].type != SymbolType::NonTerminal)
              continue;
            if (i + 1 != prod.body.size()) {
              auto first = FIRST(prod.body[i + 1]);
              // rule 2
              for (auto &a : first) {
                if (a != SymbolType::Epsilon() &&
                    !follow[prod.body[i]].contains(a)) {
                  follow[prod.body[i]].insert(a);
                  followTableChanged = true;
                }
              }
              // rule3
              if (first.contains(SymbolType::Epsilon())) {
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

  template <class _SymbolType> struct LR0Item {
    using SymbolType = _SymbolType;
    using ProductionType = Production<SymbolType>;

    ProductionType production;
    size_t dotPosition;

    std::optional<SymbolType> getSymbolAfterDot() const {
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

  template <class _SymbolType> struct std::hash<LR0Item<_SymbolType>> {
    size_t operator()(const LR0Item<_SymbolType> &item) const {
      size_t seed = std::hash<Production<_SymbolType>>()(item.production);
      seed ^= std::hash<size_t>()(item.dotPosition) + 0x9e3779b9 + (seed << 6) +
              (seed >> 2);
      return seed;
    }
  };

  template <class _SymbolType>
  struct std::hash<std::pair<size_t, _SymbolType>> {
    size_t operator()(const std::pair<size_t, _SymbolType> &p) const {
      size_t seed = std::hash<size_t>()(p.first);
      seed ^= std::hash<_SymbolType>()(p.second) + 0x9e3779b9 + (seed << 6) +
              (seed >> 2);
      return seed;
    }
  };

  template <class _SymbolType> struct SLRGrammar : public Grammar<_SymbolType> {
    using StateType = size_t;
    using SymbolType = _SymbolType;
    using ProductionType = Production<SymbolType>;
    using LR0ItemType = LR0Item<SymbolType>;

    ProductionType augmentedProduction;

    std::unordered_set<LR0ItemType>
    CLOSURE(std::unordered_set<LR0ItemType> itemSet) const {
      auto hasNonTerminalAfterDot = [](const LR0ItemType &item) {
        auto optionalSym = item.getSymbolAfterDot();
        if (!optionalSym)
          return false;
        auto sym = optionalSym.value();
        return sym.type == SymbolType::NonTerminal;
      };

      bool itemSetChanged;
      do {
        itemSetChanged = false;
        for (auto &item :
             itemSet | std::views::filter(hasNonTerminalAfterDot)) {
          for (auto &production : this->productions) {
            if (production.head == item.getSymbolAfterDot()) {
              LR0ItemType newItem = {production, 0};
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

    std::unordered_set<LR0ItemType>
    GOTO(const std::unordered_set<LR0ItemType> &itemSet,
         const SymbolType &symbol) const {
      std::unordered_set<LR0ItemType> kernels;
      for (auto &item : itemSet) {
        if (item.getSymbolAfterDot() == symbol)
          kernels.insert({item.production, item.dotPosition + 1});
      }
      return CLOSURE(std::move(kernels));
    }

    std::pair<std::vector<std::unordered_set<LR0ItemType>>,
              std::unordered_map<std::pair<StateType, SymbolType>, StateType>>
    buildAutomaton() const {
      std::vector<std::unordered_set<LR0ItemType>> collection = {
        
          {CLOSURE({{augmentedProduction, 0}})}};
      std::unordered_map<std::pair<StateType, SymbolType>, StateType>
          transitions;

      for (size_t i = 0; i < collection.size(); ++i) {
        for (auto &terminal : this->terminals) {
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
        for (auto &nonTerminal : this->nonTerminals) {
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
};
