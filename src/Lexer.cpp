#include <algorithm>
#include <stdexcept>

#include "Lexer.h"
#include "Tokens.h"

std::pair<std::vector<Token>, std::vector<std::string>> Lexer::tokenize(const std::string& inputString) {
    if(inputString.empty()) return {};
    bool inQuotes = false;
    char openingQuote = ' ';
    std::vector<Token> tokens;
    std::vector<std::string> errors;

    // Skipping whitespace, each char is added as a token
    for(size_t i = 0; i < inputString.length(); i++) {

        Token newToken { inputString[i] };

        // handles quotes
        if (newToken.mType == TokenType::Quote) {
            if (inQuotes && newToken.mValue[0] == openingQuote) {
                inQuotes = false;
            } else if (inQuotes) {
                tokens.back().mValue += newToken.mValue;
            } else {
                inQuotes = true;
                openingQuote = newToken.mValue[0];
                tokens.push_back(Token{ "", TokenType::String });
            }
            continue;
        }

        // adds all characters in quotes
        if (inQuotes) {
            tokens.back().mValue += newToken.mValue;
            continue;
        }

        // ignores beginning whitspace
        if (tokens.empty()) {
            if (isWhitespace(inputString[i])) continue;
            tokens.push_back(newToken);
            continue;
        }
        // handles breaks
        if (newToken.mType == TokenType::Break) {

            if (tokens.back().mType != TokenType::DelimiterOpen ||
                tokens.back().mType != TokenType::DelimiterClose ||
                tokens.back().mType != TokenType::Operator ||
                tokens.back().mType != TokenType::Endline ||
                tokens.back().mType != TokenType::Break)
                   tokens.push_back( newToken );

            continue;
        }

        // handles number token input
        if (newToken.mType == TokenType::Number) {
            // Turns groups of numbers into a multi-digit number
            switch (tokens.back().mType) {
                case TokenType::Number:
                    tokens.back().mValue += newToken.mValue;
                    if(std::ranges::count(tokens.back().mValue, '.') == 2)
                        errors.push_back("Two decimals in " + tokens.back().mValue);
                    break;
                case TokenType::Unknown:
                    tokens.back().mValue += newToken.mValue;
                    break;
                case TokenType::Character:
                    tokens.back().mValue += newToken.mValue;
                    tokens.back().mType = TokenType::Unknown;
                    break;
                default:
                    tokens.push_back(newToken);
                    break;
            }
            continue;
        }

        // handles letter token input
        if (newToken.mType == TokenType::Character) {
            switch (tokens.back().mType) {
                case TokenType::Unknown:
                    tokens.back().mValue += newToken.mValue;
                    break;
                case TokenType::Character:
                    tokens.back().mValue += newToken.mValue;
                    tokens.back().mType = TokenType::Unknown;
                    break;
                case TokenType::Number:
                    errors.push_back("Cannot begin with a number.");
                default:
                    tokens.push_back(newToken);
                    break;
            }
            continue;
        }

        // handles unknown input
        if (newToken.mType == TokenType::Unknown) {
            switch (tokens.back().mType) {
                case TokenType::Unknown:
                    tokens.back().mValue += newToken.mValue;
                    break;
                default:
                    tokens.push_back( newToken );
                    break;
            }
            continue;
        }

        // handles delimiters, operators, and endline
        if (newToken.mType == TokenType::DelimiterOpen ||
        newToken.mType == TokenType::DelimiterClose ||
        newToken.mType == TokenType::Operator ||
        newToken.mType == TokenType::Endline) {
            // remove unneeded breaks
            if (tokens.back().mType == TokenType::Break)
                tokens.pop_back();
            tokens.push_back(newToken);
        }
    }
    // removes trailing whitespace
    if (!tokens.empty() && tokens.back().mType == TokenType::Break) tokens.pop_back();

    // Checks for unterminated strings
    if (inQuotes) errors.push_back("Unterminated string");

    // final pass to convert chars and unknowns to language constants, vars, and funcs
    for (size_t i{ 0 }; i < tokens.size(); i++) {
        if (tokens[i].mType == TokenType::Unknown || tokens[i].mType == TokenType::Character)
            tokens[i].mType = checkMultiCharToken(tokens[i]);
    }

    // check if unknowns are multichar things
    return {tokens, errors};
}