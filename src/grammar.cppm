module;

#include <concepts>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

export module grammar;

export {

  template <typename Base> class StringLike {
  public:
    std::string_view getRepr() const { return repr; }

    bool operator==(const Base &other) const {
      return getRepr() == other.getRepr();
    }

  protected:
    std::string repr;
  };

  template <typename T>
    requires std::derived_from<T, StringLike<T>>
  struct std::hash<T> {
    size_t operator()(const T &p) const {
      return std::hash<std::string_view>()(p.getRepr());
    }
  };

  class Production : public StringLike<Production> {
  public:
    const std::string head;
    const std::vector<std::string> body;

    Production(std::string head, std::vector<std::string> body)
        : head(std::move(head)), body(std::move(body)) {
      repr = this->head + " ::=";
      for (auto &sym : this->body) {
        repr += " ";
        repr += sym;
      }
    }
  };

  class Grammar {
  public:
    const std::unordered_set<std::string> terminals;
    const std::unordered_set<Production> productions;

    Grammar(std::unordered_set<std::string> terminals,
            std::unordered_set<Production> productions)
        : terminals(terminals), productions(productions) {}

    bool isTerminal(const std::string &symbol) {
      return terminals.contains(symbol);
    }

  private:
  };

  class LR0Item : public StringLike<LR0Item> {
  public:
    const Production production;
    const size_t dotPosition;

    LR0Item(Production production, size_t dotPosition)
        : production(std::move(production)), dotPosition(dotPosition) {
      repr = this->production.head;
      repr += " ::=";
      for (size_t i = 0; i < this->production.body.size(); ++i) {
        if (i == dotPosition) {
          repr += " .";
        }
        repr += " ";
        repr += this->production.body[i];
      }
      if (dotPosition == this->production.body.size())
        repr += " .";
    }

    std::optional<std::string> getSymbolAfterDot() {
      if (dotPosition < production.body.size()) {
        return production.body[dotPosition];
      } else {
        return std::nullopt;
      }
    }
  };
}