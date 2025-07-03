export module slr;

import grammar;

export {
  class SLRGrammar : public Grammar {
  public:
    SLRGrammar()
        : Grammar(
              {"Add", "Subtract", "Multiply", "Divide", "LeftParen",
               "RightParen", "Literal"},
              {{"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
               {"AddtiveExpr",
                {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
               {"AddtiveExpr", {"MultiplicativeExpr"}},
               {"MultiplicativeExpr",
                {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
               {"MultiplicativeExpr",
                {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
               {"MultiplicativeExpr", {"PrimaryExpr"}},
               {"PrimaryExpr", {"Literal"}},
               {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}}){};
  };
}
