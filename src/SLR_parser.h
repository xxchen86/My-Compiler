#pragma once

#include "lexer.h"


class SLRParser {
public:
    SLRParser(Lexer& lexer) : lexer(lexer) {}

    

private:
    Lexer& lexer;
};