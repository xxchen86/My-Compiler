
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

import lexer;
import grammar;

export {
  struct Action {
    enum Type { Shift, Reduce, Accept, Error };
    Type type;
    std::variant<size_t, Production> value;
  };

  class SLRParser {
  public:
    using State = SLRGrammar::State;

    SLRParser(Lexer &lexer, const SLRGrammar &grammar)
        : lexer(lexer), grammar(grammar) {}

    void buildParser() {
      auto follow = grammar.FOLLOW_Table();
      auto [collectionOfItemSet, transitions] = grammar.buildAutomaton();

      
      // for(auto& [k, v] : follow) {
      //   std::cout << k << " FOLLOW:" << std::endl;
      //   for (auto& s : v) {
      //       std::cout << s << " ";
      //   }
      //   std::cout << std::endl;
      // }
      // // debug
      // for (size_t i = 0; i < collectionOfItemSet.size(); ++i) {
      //   std::cout << "State " << i << std::endl;
      //   for (auto &item : collectionOfItemSet[i]) {
      //     std::cout << item << std::endl;
      //   }
      //   for (auto &[k, v] : transitions) {
      //     if (k.first == i) {
      //       std::cout << "when " << k.second << " goto State " << v
      //                 << std::endl;
      //     }
      //   }
      //   std::cout << std::endl;
      // }

      for (auto &[k, v] : transitions) {
        auto i = k.first;
        const auto &sym = k.second;
        if (sym.type == Symbol::Terminal)
          ACTION[{i, sym}] = {Action::Shift, transitions[{i, sym}]};
        else
          GOTO[{i, sym}] = transitions[{i, sym}];
      }
      for (size_t i = 0; i < collectionOfItemSet.size(); ++i) {
        for (auto &item : collectionOfItemSet[i]) {
          if (item.reducible()) {
            for (auto &s : follow[item.production.head]) {
              ACTION[{i, s}] = {Action::Reduce, item.production};
            }
          }
        }
        if (collectionOfItemSet[i].contains({grammar.augmentedProduction, 1}))
          ACTION[{i, InputRightEndMarker}] = {Action::Accept};
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
            std::cout << "shift current state: " << stateStack.top()
                      << std::endl;
            break;
          case Action::Reduce:
            for (auto i = 0; i < std::get<Production>(act.value).body.size();
                 ++i) {
              stateStack.pop();
            }
            stateStack.push(
                GOTO[{stateStack.top(),
                      std::get<Production>(act.value).head}]);
            std::cout << "reduce" << std::get<Production>(act.value)
                      << " current state: " << stateStack.top() << std::endl;
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

  public: // TODO: for debug
    Lexer &lexer;
    std::stack<State> stateStack;
    const SLRGrammar &grammar;
    std::unordered_map<std::pair<State, Symbol>, Action> ACTION;
    std::unordered_map<std::pair<State, Symbol>, State> GOTO;
  };
}
