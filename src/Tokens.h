#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>


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
    LangFunction,
    Constant,
    Unknown,
    EndOfFile
};

// Specifies token types
namespace types {
    inline constexpr std::string_view breaks              = " \n\r\t\v\f";
    inline constexpr std::string_view parenOpen           = "(";
    inline constexpr std::string_view parenClose          = ")";
    inline constexpr std::string_view braceOpen           = "{";
    inline constexpr std::string_view braceClose          = "}";
    inline constexpr std::string_view bracketOpen         = "[";
    inline constexpr std::string_view bracketClose        = "]";
    inline constexpr std::string_view quotes              = "\"\'";
    inline constexpr std::string_view endline             = ";";
    inline constexpr std::string_view comma               = ",";
    inline constexpr std::string_view operators           = "+-/*^!%&|=.<>";
    inline constexpr std::string_view numbers             = "1234567890";
    inline constexpr std::string_view identifiers         = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

    inline const std::unordered_set<std::string> langFunctions = {
        "print"
    };
    inline const std::unordered_map<std::string, float> constants = {
        {"PI", 3.14},
    };
}

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

        if (valueInTokenType(types::breaks)) mType = TokenType::Break;
        else if (valueInTokenType(types::parenOpen)) mType = TokenType::ParenOpen;
        else if (valueInTokenType(types::parenClose)) mType = TokenType::ParenClose;
        else if (valueInTokenType(types::braceOpen)) mType = TokenType::BraceOpen;
        else if (valueInTokenType(types::braceClose)) mType = TokenType::BraceClose;
        else if (valueInTokenType(types::bracketOpen)) mType = TokenType::BracketOpen;
        else if (valueInTokenType(types::bracketClose)) mType = TokenType::BracketClose;
        else if (valueInTokenType(types::quotes)) mType = TokenType::Quote;
        else if (valueInTokenType(types::endline)) mType = TokenType::Endline;
        else if (valueInTokenType(types::comma)) mType = TokenType::Comma;
        else if (valueInTokenType(types::operators)) mType = TokenType::Operator;
        else if (valueInTokenType(types::numbers)) mType = TokenType::Number;
        else if (valueInTokenType(types::identifiers)) mType = TokenType::Identifier;
        else mType = TokenType::Unknown;
    }

    bool valueInTokenType(const std::string_view strView) const {
        return (strView.find(this->mValue) != std::string_view::npos);
    }
};

inline TokenType checkMultiCharToken(const Token& token) {
    if (types::langFunctions.contains(token.mValue)) return TokenType::LangFunction;
    if (types::constants.contains(token.mValue)) return TokenType::Constant;
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
        case TokenType::LangFunction:       return "Function";
        case TokenType::Constant:       return "Constant";
        case TokenType::Unknown:        return "Unknown";
        case TokenType::EndOfFile:      return "EndOfFile";
    }
    return "?";
}