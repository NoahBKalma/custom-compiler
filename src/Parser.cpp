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

        if (peekTokenType() == TokenType::Operator && peekToken().mValue == "=") {
            if (statement->type() != NodeType::Identifier) {
                mErrorMessage = "Left side of '=' must be an identifier.";
                return 1;
            }
            Token op = nextToken();
            std::unique_ptr<Node> right = parseExpression();
            if (!right) return 1;
            statement = std::make_unique<BinaryOperation>(op, std::move(statement), std::move(right));
        }

        if (peekTokenType() != TokenType::Endline) {
            if (peekTokenType() == TokenType::EndOfFile ||
                peekTokenType() == TokenType::Break && peekToken().mValue == "\n")
                mErrorMessage = "Expected ';' at end of line.";
            else
                mErrorMessage = "Expected ';' but got: " + peekToken().mValue;
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
            (peekToken().mValue == "*" || peekToken().mValue == "/" || peekToken().mValue == "%")) {
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

        // a function call must start with an opening parenthesis
        if (peekTokenType() != TokenType::ParenOpen) {
            if (peekTokenType() == TokenType::EndOfFile)
                mErrorMessage = "Reached end of file before function call closed.";
            else
                mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
            return nullptr;
        }
        nextToken(); // skip opening parenthesis

        // parse arguments until the closing parenthesis, so foo() has no arguments
        while (peekTokenType() != TokenType::ParenClose) {
            std::unique_ptr<Node> argument = parseExpression();
            if (!argument) return nullptr;
            function->arguments.push_back(std::move(argument));

            // read next if more arguments exist
            if (peekTokenType() == TokenType::Comma)
                nextToken();
            else
                break;
        }

        if (peekTokenType() != TokenType::ParenClose) {
            if (peekTokenType() == TokenType::EndOfFile)
                mErrorMessage = "Reached end of file before function call closed.";
            else
                mErrorMessage = "Unexpected token " + peekToken().mValue + " while trying to parse function call.";
            return nullptr;
        }

        nextToken(); // skip closing parenthesis
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