#pragma once

#include <string>
#include <unordered_set>


// Defines token types
enum class TokenType {
    Break,
    ParenOpen,
    ParenClose,
    BraceOpen,
    BraceClose,
    BracketOpen,
    BracketClose,
    Endline,
    Comma,
    Quote,
    Operator,
    Number,
    Identifier,
    String,
    Function,
    Constant,
    Unknown,
    EndOfFile
};

// Specifies token types
inline std::string_view breaks              = " \n\r\t\v\f";
inline std::string_view parenOpen           = "(";
inline std::string_view parenClose          = ")";
inline std::string_view braceOpen           = "{";
inline std::string_view braceClose          = "}";
inline std::string_view bracketOpen         = "[";
inline std::string_view bracketClose        = "]";
inline std::string_view quotes              = "\"\'";
inline std::string_view endline             = ";";
inline std::string_view comma               = ",";
inline std::string_view operators           = "+-/*^!%&|=.<>";
inline std::string_view numbers             = "1234567890";
inline std::string_view identifiers         = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

inline std::unordered_set<std::string> functions = {
    "print"
};
inline std::unordered_set<std::string> constants = {
    "PI", "INT_MAX", "INT_MIN", "UINT_MAX"
};

inline bool isTwoCharOperator(const std::string& str) {
    return str == "==" || str == "!=" || str == "<=" || str == ">=" || str == "&&" || str == "||";
}

struct Token {
    std::string mValue;
    TokenType mType;

    // Constructor that takes value and type
    Token(std::string value, TokenType type) : mValue(std::move(value)), mType(type) {}

    // Constructor that sets the token type based on what the value is
    explicit Token(const char value) : mValue( {value} ) {

        if (valueInTokenType(breaks)) mType = TokenType::Break;
        else if (valueInTokenType(parenOpen)) mType = TokenType::ParenOpen;
        else if (valueInTokenType(parenClose)) mType = TokenType::ParenClose;
        else if (valueInTokenType(braceOpen)) mType = TokenType::BraceOpen;
        else if (valueInTokenType(braceClose)) mType = TokenType::BraceClose;
        else if (valueInTokenType(bracketOpen)) mType = TokenType::BracketOpen;
        else if (valueInTokenType(bracketClose)) mType = TokenType::BracketClose;
        else if (valueInTokenType(quotes)) mType = TokenType::Quote;
        else if (valueInTokenType(endline)) mType = TokenType::Endline;
        else if (valueInTokenType(comma)) mType = TokenType::Comma;
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

inline const char* typeName(TokenType t) {
    switch (t) {
        case TokenType::Break:          return "Break";
        case TokenType::ParenOpen:      return "ParenOpen";
        case TokenType::ParenClose:     return "ParenClose";
        case TokenType::BraceOpen:      return "BraceOpen";
        case TokenType::BraceClose:     return "BraceClose";
        case TokenType::BracketOpen:    return "BracketOpen";
        case TokenType::BracketClose:   return "BracketClose";
        case TokenType::Endline:        return "Endline";
        case TokenType::Comma:          return "Comma";
        case TokenType::Quote:          return "Quote";
        case TokenType::Operator:       return "Operator";
        case TokenType::Number:         return "Number";
        case TokenType::Identifier:     return "Identifier";
        case TokenType::String:         return "String";
        case TokenType::Function:       return "Function";
        case TokenType::Constant:       return "Constant";
        case TokenType::Unknown:        return "Unknown";
        case TokenType::EndOfFile:      return "EndOfFile";
    }
    return "?";
}