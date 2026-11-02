#pragma once

#include <string>
#include <unordered_set>


// Defines token types
enum class TokenType {
    Break,
    DelimiterOpen,
    DelimiterClose,
    Endline,
    Quote,
    Operator,
    Number,
    Identifier,
    String,
    Function,
    Constant,
    Unknown
};

// Specifies token types
inline std::string_view breaks              = " \n\r\t\v\f";
inline std::string_view delimitersOpen      = "([{";
inline std::string_view delimitersClose     = ")]}";
inline std::string_view quotes              = "\"\'";
inline std::string_view endline             = ";";
inline std::string_view operators           = "+-/*^!%&|=";
inline std::string_view numbers             = "1234567890.";
inline std::string_view identifiers          = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

inline std::unordered_set<std::string> functions = {
    "print"
};
inline std::unordered_set<std::string> constants = {
    "PI", "INT_MAX", "INT_MIN", "UINT_MAX"
};

struct Token {
    std::string mValue;
    TokenType mType;

    // Constructor that takes
    Token(std::string value, TokenType type) : mValue(std::move(value)), mType(type) {}

    // Constructor that sets the token type based on what the value is
    explicit Token(const char value) : mValue( {value} ) {

        if (valueInTokenType(breaks)) mType = TokenType::Break;
        else if (valueInTokenType(delimitersOpen)) mType = TokenType::DelimiterOpen;
        else if (valueInTokenType(delimitersClose)) mType = TokenType::DelimiterClose;
        else if (valueInTokenType(quotes)) mType = TokenType::Quote;
        else if (valueInTokenType(endline)) mType = TokenType::Endline;
        else if (valueInTokenType(operators)) mType = TokenType::Operator;
        else if (valueInTokenType(numbers)) mType = TokenType::Number;
        else if (valueInTokenType(identifiers)) mType = TokenType::Identifier;
        else mType = TokenType::Unknown;
    }

    bool valueInTokenType(const std::string_view strView) const {
        return (strView.find(this->mValue) != std::string_view::npos);
    }
};

inline std::string strToLower(std::string str) {
    for (char& c : str) {
        if (0 <= c && c <= 127)
            c = std::tolower(c);
    }

    return str;
}

inline TokenType checkMultiCharToken(const Token& token) {
    if (functions.contains(token.mValue)) return TokenType::Function;
    if (constants.contains(token.mValue)) return TokenType::Constant;
    return TokenType::Identifier;   // not a keyword, so it's a name
}