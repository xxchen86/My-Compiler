#pragma once

#include <string>
#include <vector>
#include <set>
#include <algorithm>


struct Production {
    std::string head;
    std::vector<std::string> body;

    bool operator==(const Production& other) const {
        return head == other.head && body == other.body;
    }
};

const std::set<std::string> terminals = {
    "Add", "Subtract", "Multiply", "Divide",
    "LeftParen", "RightParen", "Literal"
};


///     AddtiveExpr ::= AddtiveExpr Add MultiplicativeExpr | AddtiveExpr Subtract MultiplicativeExpr | MultiplicativeExpr
///     MultiplicativeExpr ::= MultiplicativeExpr Multiply PrimaryExpr | MultiplicativeExpr Divide PrimaryExpr | PrimaryExpr
///     PrimaryExpr ::= Literal | LeftParen AddtiveExpr RightParen
const std::vector<Production> allProductions = {
    {"AddtiveExpr", {"AddtiveExpr", "Add", "MultiplicativeExpr"}},
    {"AddtiveExpr", {"AddtiveExpr", "Subtract", "MultiplicativeExpr"}},
    {"AddtiveExpr", {"MultiplicativeExpr"}},
    {"MultiplicativeExpr", {"MultiplicativeExpr", "Multiply", "PrimaryExpr"}},
    {"MultiplicativeExpr", {"MultiplicativeExpr", "Divide", "PrimaryExpr"}},
    {"MultiplicativeExpr", {"PrimaryExpr"}},
    {"PrimaryExpr", {"Literal"}},
    {"PrimaryExpr", {"LeftParen", "AddtiveExpr", "RightParen"}}
};


bool isTerminal(const std::string& symbol) {
    return std::find(terminals.begin(), terminals.end(), symbol) != terminals.end();
}


struct LR0Item {
    Production production;
    size_t dotPosition;

    std::optional<std::string> getSymbolAfterDot() {
        if (dotPosition < production.body.size()) {
            return production.body[dotPosition];
        } else {
            return std::nullopt;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const LR0Item& item) {
        os << item.production.head << " ::=";
        for (size_t i = 0; i < item.production.body.size(); ++i) {
            if (i == item.dotPosition) {
                os << " .";
            } 
            os << " " << item.production.body[i];
        }
        return os;
    }

    bool operator==(const LR0Item& other) const {
        return production == other.production && dotPosition == other.dotPosition;
    }
};


auto computeClosure(std::vector<LR0Item>&& items) {
    std::vector<LR0Item> closureItems = std::move(items);

    auto len = closureItems.size();
    for (size_t i = 0; i < len; ++i) {
        auto& item = closureItems[i];
        auto hasSymbol = item.getSymbolAfterDot();
        if (!hasSymbol)
            continue;
        auto& symbol = hasSymbol.value();
        if (!isTerminal(symbol)) {
            for (auto& production : allProductions) {
                if (production.head == symbol) {
                    LR0Item newItem{production, 0};
                    if (std::find(closureItems.begin(), closureItems.end(), newItem) == closureItems.end()) {
                        closureItems.push_back(newItem);
                        len++; // Increase length since we added a new item
                        i = 0;
                    }
                }
            }
        }
    }

    return closureItems;
}

