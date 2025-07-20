
module;

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>

export module lr_parser;

import lexer_base;
import grammar;

export {

  template <LexerTypeTrait LexerType, class GrammarType> class SLRParser {
  public:
    using StateType = GrammarType::StateType;
    using SymbolType = GrammarType::SymbolType;
    using ProductionType = GrammarType::ProductionType;
    using LR0ItemType = GrammarType::LR0ItemType;

    struct Action {
      enum Type { Shift, Reduce, Accept, Error };
      Type type;
      std::variant<size_t, ProductionType> value;
    };

    SLRParser(LexerType &lexer, const GrammarType &grammar)
        : lexer(lexer), grammar(grammar) {}

    void buildParser() {
      auto follow = grammar.FOLLOW_Table();
      auto [collectionOfItemSet, transitions] = grammar.buildAutomaton();

      for (auto &[k, v] : transitions) {
        auto i = k.first;
        const auto &sym = k.second;
        if (sym.type == SymbolType::Terminal) {
          if (ACTION.contains({i, sym}))
            throw std::runtime_error("conflict");
          else
            ACTION[{i, sym}] = {Action::Shift, transitions[{i, sym}]};
        } else {
          GOTO[{i, sym}] = transitions[{i, sym}];
        }
      }
      for (size_t i = 0; i < collectionOfItemSet.size(); ++i) {
        for (auto &item : collectionOfItemSet[i]) {
          if (item.reducible()) {
            for (auto &s : follow[item.production.head]) {
              if (ACTION.contains({i, s}))
                throw std::runtime_error("conflict");
              ACTION[{i, s}] = {Action::Reduce, item.production};
            }
          }
        }
        if (collectionOfItemSet[i].contains({grammar.augmentedProduction, 1})) {
          if (ACTION.contains({i, SymbolType::InputRightEndMarker()}))
            throw std::runtime_error("conflict");
          else
            ACTION[{i, SymbolType::InputRightEndMarker()}] = {Action::Accept};
        }
      }
    }


    void parse() {
      stateStack.push(0);
      while (true) {
        auto token = lexer.peekToken();
        if (ACTION.contains({stateStack.top(), token.type})) {
          auto act = ACTION[{stateStack.top(), token.type}];
          switch (act.type) {
          case Action::Shift:
            lexer.getToken();
            stateStack.push(std::get<size_t>(act.value));
            break;
          case Action::Reduce:
            for (auto i = 0;
                 i < std::get<ProductionType>(act.value).body.size(); ++i) {
              stateStack.pop();
            }
            stateStack.push(GOTO[{stateStack.top(),
                                  std::get<ProductionType>(act.value).head}]);
            break;
          case Action::Accept:
            std::cout << "Accept" << std::endl;
            return;
          case Action::Error:
            assert(false);
            break;
          }
        } else {
          throw std::runtime_error("error");
        }
      }
    }

  private:
    LexerType &lexer;
    std::stack<StateType> stateStack;
    const GrammarType &grammar;
    std::unordered_map<std::pair<StateType, SymbolType>, Action> ACTION;
    std::unordered_map<std::pair<StateType, SymbolType>, StateType> GOTO;
  };
}
