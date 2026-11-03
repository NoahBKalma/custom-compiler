#include <algorithm>
#include <utility>

#include "Lexer.h"
#include "Tokens.h"

std::pair<std::vector<Token>, std::vector<std::string>> Lexer::tokenize(const std::string& inputString) {
    if(inputString.empty()) return { std::vector<Token> {{ "", TokenType::EndOfFile }}, {} };

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
                case TokenType::ParenOpen:
                case TokenType::ParenClose:
                case TokenType::BraceOpen:
                case TokenType::BraceClose:
                case TokenType::BracketOpen:
                case TokenType::BracketClose:
                case TokenType::Operator:
                case TokenType::Endline:
                case TokenType::Comma:
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

        // merges two-char operators like == and <=
        if (newToken.mType == TokenType::Operator && tokens.back().mType == TokenType::Operator
            && !isWhitespace(inputString[i - 1])
            && isTwoCharOperator(tokens.back().mValue + newToken.mValue)) {
            tokens.back().mValue += newToken.mValue;
            continue;
        }

        // handles delimiters, operators, and endline
        if (newToken.mType == TokenType::ParenOpen ||
        newToken.mType == TokenType::ParenClose ||
        newToken.mType == TokenType::BraceOpen ||
        newToken.mType == TokenType::BraceClose ||
        newToken.mType == TokenType::BracketOpen ||
        newToken.mType == TokenType::BracketClose ||
        newToken.mType == TokenType::Comma ||
        newToken.mType == TokenType::Operator ||
        newToken.mType == TokenType::Endline) {
            // remove unneeded breaks
            if (tokens.back().mType == TokenType::Break)
                tokens.pop_back();
            tokens.push_back(newToken);
        }
    }

    // Checks for unterminated strings
    if (inQuotes) errors.emplace_back("Unterminated string");

    // final pass to convert identifiers to constants or functions. Also remove all breaks
    // copy to new vector due to break removals

    std::vector<Token> cleanTokens;
    for (Token& token : tokens) {
        if (token.mType == TokenType::Break) continue;

        if (token.mType == TokenType::Identifier)
            token.mType = checkMultiCharToken(token);
        else if (token.mType == TokenType::Unknown)
            errors.emplace_back("Unknown token: " + token.mValue);

        cleanTokens.push_back(std::move(token));
    }

    // add end of file token to the end
    cleanTokens.emplace_back( "", TokenType::EndOfFile );
    return {std::move(cleanTokens), std::move(errors)};
}