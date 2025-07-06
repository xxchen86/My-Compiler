
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

  template <typename Item> class LRParser {
  public:
    LRParser(Lexer &lexer, const LRGrammar<Item> &grammar) : lexer(lexer) {}

    void constructAction() {
      auto follow = grammar.FOLLOW();
      auto [collectionOfItemSet, transitions] =
          grammar.constructCollectionOfItemSet();
      for (auto &[i, sym] : transitions) {
        if (grammar.isTerminal(sym))
          action[{i, sym}] = {Action::Shift, transitions[{i, sym}]};
        else
          GOTO[{i, sym}] = transitions[{i, sym}];
      }
      for (size_t i = 0; i < collectionOfItemSet.size(); ++i) {
        for (auto &item : collectionOfItemSet[i]) {
          if (item.canReduce()) {
            for (auto &s : follow[item.production().head().data()]) {
              action[{i, s}] = {Action::Reduce, item.production()};
            }
          }
        }
        if (collectionOfItemSet[i].contains({grammar.startProduction, 1}))
          action[{i, "$"}] = {Action::Accept};
      }
    }

  public: // TODO: for debug
    Lexer &lexer;
    const LRGrammar<Item> &grammar;
    std::unordered_map<std::pair<size_t, std::string>, Action> action;
    std::unordered_map<std::pair<size_t, std::string>, size_t> GOTO;
  };

  class SLRParser {
  public:
    SLRParser(Lexer &lexer, const LRGrammar<LR0Item> &grammar)
        : lexer(lexer), grammar(grammar) {}

    void constructTable() {
      auto follow = grammar.FOLLOW();
      auto [collectionOfItemSet, transitions] =
          grammar.constructCollectionOfItemSet();
        
        // debug
        for (size_t i = 0;i < collectionOfItemSet.size(); ++i) {
            std::cout << "State " << i << std::endl;
            for (auto& item : collectionOfItemSet[i]) {
                std::cout << item << std::endl;
            }
            for (auto& [k, v] : transitions) {
                if (k.first == i) {
                    std::cout << "when " << k.second << " goto State " << v << std::endl;
                }
            }
            std::cout << std::endl;
        }

      for (auto &[k, v] : transitions) {
        auto i = k.first;
        const auto &sym = k.second;
        if (grammar.isTerminal(sym))
          actions[{i, sym}] = {Action::Shift, transitions[{i, sym}]};
        else
          GOTO[{i, sym}] = transitions[{i, sym}];
      }
      for (size_t i = 0; i < collectionOfItemSet.size(); ++i) {
        for (auto &item : collectionOfItemSet[i]) {
          if (item.canReduce()) {
            for (auto &s : follow[item.production().head().data()]) {
              actions[{i, s}] = {Action::Reduce, item.production()};
            }
          }
        }
        if (collectionOfItemSet[i].contains({grammar.startProduction, 1}))
          actions[{i, "$"}] = {Action::Accept};
      }
    }

    void parse() {
      stateStack.push(0);
      while (true) {
        auto token = lexer.peekToken();
        if (actions.contains({stateStack.top(), token.str()})) {
          auto act = actions[{stateStack.top(), token.str()}];
          switch (act.type) {
          case Action::Shift:
            lexer.getToken();
            stateStack.push(std::get<size_t>(act.value));
            std::cout << "shift current state: " << stateStack.top()
                      << std::endl;
            break;
          case Action::Reduce:
            for (auto i = 0; i < std::get<Production>(act.value).body().size();
                 ++i) {
              stateStack.pop();
            }
            stateStack.push(
                GOTO[{stateStack.top(),
                      std::get<Production>(act.value).head().data()}]);
            std::cout << "reduce " << std::get<Production>(act.value)
                      << " current state: " << stateStack.top() << std::endl;
            break;
          case Action::Accept:
            std::cout <<"Accept" << std::endl;
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
    std::stack<size_t> stateStack;
    const LRGrammar<LR0Item> &grammar;
    std::unordered_map<std::pair<size_t, std::string>, Action> actions;
    std::unordered_map<std::pair<size_t, std::string>, size_t> GOTO;
  };
}
