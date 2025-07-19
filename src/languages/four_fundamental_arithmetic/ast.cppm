module;

#include <memory>
#include <ostream>

export module ast;

export namespace ast {

struct Expression {
  virtual ~Expression() = default;
  virtual void print(std::ostream &os, int indent = 0) const = 0;
};

struct LiteralExpression : public Expression {
  int value;
  LiteralExpression(int val) : value(val) {}
  virtual ~LiteralExpression() = default;
  void print(std::ostream &os, int indent = 0) const override {
    os << std::string(indent, ' ') << "Literal(" << value << ")\n";
  }
};

struct BinaryExpression : public Expression {
  std::unique_ptr<Expression> left, right;
  char op;
  BinaryExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r,
                   char oper)
      : left(std::move(l)), right(std::move(r)), op(oper) {}
  void print(std::ostream &os, int indent = 0) const override {
    os << std::string(indent, ' ') << "Binary(" << op << ")\n";
    if (left) {
      left->print(os, indent + 2);
    } else {
      os << std::string(indent + 2, ' ') << "null\n";
    }
    if (right) {
      right->print(os, indent + 2);
    } else {
      os << std::string(indent + 2, ' ') << "null\n";
    }
  }
};

} // namespace ast