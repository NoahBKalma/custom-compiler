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
            switch (tokens.back().mType) {
                case TokenType::DelimiterOpen:
                case TokenType::DelimiterClose:
                case TokenType::Operator:
                case TokenType::Endline:
                case TokenType::Break:
                    break;                       // drop redundant whitespace
                default:
                    tokens.push_back(newToken);
            }
            continue;
        }

        // handle decimals/periods
        if (newToken.mValue[0] == '.' && tokens.back().mType == TokenType::Number) {
            tokens.back().mValue += newToken.mValue;
            if (std::ranges::count(tokens.back().mValue, '.') == 2)
                errors.push_back("Two decimals in " + tokens.back().mValue);
            continue;
        }

        // handle numbers
        if (newToken.mType == TokenType::Number) {
            switch (tokens.back().mType) {
                case TokenType::Number:
                case TokenType::Identifier:         // x1, foo2
                    tokens.back().mValue += newToken.mValue;
                    break;
                default:
                    tokens.push_back(newToken);
            }
            continue;
        }

        // letters and underscore
        if (newToken.mType == TokenType::Identifier) {
            switch (tokens.back().mType) {
                case TokenType::Identifier:
                    tokens.back().mValue += newToken.mValue;
                    break;
                case TokenType::Number:
                    errors.emplace_back("Identifier cannot begin with a number");
                    tokens.push_back(newToken);
                    break;
                default:
                    tokens.push_back(newToken);
            }
            continue;
        }

        // handles unrecognized symbols
        if (newToken.mType == TokenType::Unknown) {
            tokens.push_back(newToken);
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
    if (inQuotes) errors.emplace_back("Unterminated string");

    // final pass to convert identifiers to constants or functions
    for (Token& token : tokens) {
        if (token.mType == TokenType::Identifier) {
            token.mType = checkMultiCharToken(token);
        } else if (token.mType == TokenType::Unknown) {
            errors.emplace_back("Unknown token: " + token.mValue);
        }
    }
    // check if unknowns are multichar things
    return {tokens, errors};
}