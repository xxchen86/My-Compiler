#include <gtest/gtest.h>

import grammar;
import lexer;

TEST(Lexer, 1) {
  std::istringstream iss;
  iss.str("1 - 3 + 5");

  using SymbolType = Symbol<int>;
  using TokenType = Token<SymbolType, int>;

  Lexer lexer(iss);
  std::vector<TokenType> tokens;
  while (!lexer.eof()) {
    auto token = lexer.getToken();
    tokens.push_back(token);
  }

  EXPECT_EQ(tokens.size(), 6);
  EXPECT_EQ(tokens[0].type, Literal);
  EXPECT_EQ(tokens[1].type, Subtract);
  EXPECT_EQ(tokens[2].type, Literal);
  EXPECT_EQ(tokens[3].type, Add);
  EXPECT_EQ(tokens[4].type, Literal);
  EXPECT_EQ(tokens[5].type, SymbolType::InputRightEndMarker());
  EXPECT_EQ(std::get<int>(tokens[0].value), 1);
  EXPECT_EQ(std::get<int>(tokens[2].value), 3);
  EXPECT_EQ(std::get<int>(tokens[4].value), 5);
}