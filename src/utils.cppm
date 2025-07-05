module;

#include <concepts>
#include <cstddef>
#include <format>
#include <iostream>
#include <string>
#include <string_view>

export module utils;

export {

  template <typename Base> class StringLike {
  public:
    std::string_view getRepr() const { return repr; }

    bool operator==(const Base &other) const {
      return getRepr() == other.getRepr();
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const StringLike<Base> &strLike) {
      return os << strLike.getRepr();
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

  template <typename T>
    requires std::derived_from<T, StringLike<T>>
  struct std::formatter<T> {
    template <class ParseContext> constexpr auto parse(ParseContext &ctx) {
      return ctx.begin();
    }

    template <class FmtContext> auto format(T s, FmtContext &ctx) const {
      return std::format_to(ctx.out(), "{}", s.getRepr());
    }
  };
}