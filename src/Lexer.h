#pragma once

#include <vector>


#include "Tokens.h"

namespace Lexer {
    // Turns a string into a vector of tokens and a list of error messages
    std::pair<std::vector<Token>, std::vector<std::string>> tokenize(const std::string&);

    // Checks if a given constant c is whitespace
    inline bool isWhitespace(const char c) {
        return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f');
    }

}