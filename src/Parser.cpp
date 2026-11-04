#include <iostream>

#include "Parser.h"

const Token& Parser::peekToken() const {
    return mLexedTokens[mCursor];
}

TokenType Parser::peekTokenType() const {
    return mLexedTokens[mCursor].mType;
}

const Token& Parser::nextToken() {
    if (mFinished)
        return mLexedTokens[mCursor];

    if (mLexedTokens[++mCursor].mType == TokenType::EndOfFile)
        mFinished = true;

    return mLexedTokens[mCursor - 1];
}

int Parser::parse() {
    auto program = std::make_unique<Program>();

    while (peekTokenType() != TokenType::EndOfFile) {
        // allow empty statements like ";;"
        if (peekTokenType() == TokenType::Endline) {
            nextToken();
            continue;
        }

        std::unique_ptr<Node> statement = parseExpression();
        if (!statement) return 1;

        if (peekTokenType() != TokenType::Endline) {
            if (peekTokenType() == TokenType::EndOfFile)
                mErrorMessage = "Expected ';' at end of statement.";
            else
                mErrorMessage = "Expected ';' but got " + peekToken().mValue;
            return 1;
        }
        // move past ;
        nextToken();

        program->lines.push_back(std::move(statement));
    }

    mResultTree = std::move(program);
    return 0;
}

std::unique_ptr<Node> Parser::parseExpression() {
    std::unique_ptr<Node> left = parseTerm();

    if (!left) return nullptr;

    while (peekTokenType() == TokenType::Operator &&
            (peekToken().mValue == "+" || peekToken().mValue == "-")) {
        Token op = nextToken();
        std::unique_ptr<Node> right = parseTerm();
        if (!right) return nullptr;
        left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Node> Parser::parseTerm() {
    std::unique_ptr<Node> left = parseFinal();

    if (!left) return nullptr;

    while (peekTokenType() == TokenType::Operator &&
            (peekToken().mValue == "*" || peekToken().mValue == "/")) {
        Token op = nextToken();
        std::unique_ptr<Node> right = parseFinal();
        if (!right) return nullptr;
        left = std::make_unique<BinaryOperation>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Node> Parser::parseFinal() {
    if (peekTokenType() == TokenType::Number) {
        return std::make_unique<Number>(nextToken());
    }
    if (peekTokenType() == TokenType::String) {
        return std::make_unique<String>(nextToken());
    }
    if (peekTokenType() == TokenType::Constant) {
        return std::make_unique<Constant>(nextToken());
    }
    if (peekTokenType() == TokenType::Identifier) {
        return std::make_unique<Identifier>(nextToken());
    }
    if (peekTokenType() == TokenType::ParenOpen) {

        // Skip past opening delimiter then parse expression within
        nextToken();
        std::unique_ptr<Node> result = parseExpression();
        if (result == nullptr) return nullptr;
        if (peekTokenType() == TokenType::ParenClose) {
            nextToken();
            return result;
        }

        if (peekTokenType() == TokenType::EndOfFile)
            mErrorMessage = "Unclosed parenthesis. Reached end of file.";
        else
            mErrorMessage = "Expected closing parenthesis. Got " + peekToken().mValue;
        return nullptr;

    }
    if (peekTokenType() == TokenType::Operator && peekToken().mValue == "-") {

        Token negate = nextToken();
        std::unique_ptr<Node> operand = parseFinal();
        if (operand == nullptr) return nullptr;
        return std::make_unique<Negate>(negate, std::move(operand));

    }
    if (peekTokenType() == TokenType::LangFunction) {
        Token functionToken = nextToken();
        std::unique_ptr<LangFunction> function = std::make_unique<LangFunction>(functionToken);
        do {
            if (peekTokenType() == TokenType::ParenOpen ||
                (peekToken().mValue == "," && peekTokenType() == TokenType::Comma)) {

                nextToken(); // skip opening parenthesis or comma while in loop
                } else {
                    if (peekTokenType() == TokenType::EndOfFile)
                        mErrorMessage = "Reached end of file before function call closed.";
                    else
                        mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
                    return nullptr;
                }

            std::unique_ptr<Node> argument = parseExpression();
            if (!argument) return nullptr;

            function->arguments.push_back(std::move(argument));
        } while (peekToken().mValue == "," && peekTokenType() == TokenType::Comma);
        if (peekToken().mType != TokenType::ParenClose) {
            if (peekTokenType() == TokenType::EndOfFile)
                mErrorMessage = "Reached end of file before function call closed.";
            else
                mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
            return nullptr;
        }

        nextToken();
        return function;
    }
    if (peekTokenType() == TokenType::EndOfFile) {
        mErrorMessage = "Reached end of file before expression parsed.";
    }
    else {
        mErrorMessage = "Unexpected token " + peekToken().mValue;
    }
    return nullptr;
}