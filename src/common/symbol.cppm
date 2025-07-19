module;

#include <iostream>
#include <initializer_list>
#include <unordered_set>
#include <unordered_map>

export module symbol;

export {
  struct Symbol {
    enum Type : int { Terminal, NonTerminal, Special };
    Type type;
    int id;

    bool operator==(const Symbol &other) const {
      return type == other.type && id == other.id;
    }

    friend std::ostream &operator<<(std::ostream &os, const Symbol &sym) {
      return os << "Symbol(type=" << sym.type << ", id=" << sym.id << ")";
    }
  };

  template <> struct std::hash<Symbol> {
    size_t operator()(const Symbol &symbol) const {
      return std::hash<int>()(symbol.type) ^ std::hash<int>()(symbol.id);
    }
  };

  class SymbolManager {
    std::unordered_set<Symbol> symbols;
    std::unordered_map<std::string, Symbol> symbolMap;

  public:
    const Symbol& createSymbol(std::string name, Symbol::Type type) {
      Symbol sym{type, static_cast<int>(symbols.size())};
      symbols.insert(sym);
      symbolMap[name] = sym;
      return getSymbol(name);
    }

    const Symbol& getSymbol(std::string name) { return symbolMap[name]; }
  };

  SymbolManager symbolManager;
  const Symbol& Epsilon = symbolManager.createSymbol("Epsilon", Symbol::Special);
  const Symbol& InputRightEndMarker =
      symbolManager.createSymbol("$", Symbol::Special);
}