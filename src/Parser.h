#pragma once

#include <vector>
#include <string>

#include "AST.h"

class Parser {
private:
    std::vector<Token> mLexedTokens;
    size_t mCursor;
    bool mFinished;
    std::unique_ptr<Node> mResultTree;

    std::string mErrorMessage;

public:

    // constructor
    explicit Parser(std::vector<Token> tokenList) :
        mLexedTokens { std::move(tokenList) },
        mCursor { 0 },
        mFinished { mLexedTokens[mCursor].mType == TokenType::EndOfFile } {}

    // Return resulting tree
    const Node* getResultTree() const { return mResultTree.get(); }

    // Return error message
    const std::string& getErrorMessage() const { return mErrorMessage; }

    // Return current token
    const Token& peekToken() const;

    // Return current token's type
    TokenType peekTokenType() const;

    const Token* peekNextToken() {
        if (mFinished)
            return nullptr;
        return &mLexedTokens[mCursor + 1];
    }

    // Return current and move to next
    const Token& nextToken();

    // parses everything and stores resulting tree in mResultTree
    // return 0 is success, 1 is fail so print error
    int parse();

    // parses expression and returns pointer to tree ( +, - )
    std::unique_ptr<Node> parseExpression();

    // parses term in expression and returns pointer to tree ( *, / )
    std::unique_ptr<Node> parseTerm();

    // parses a terminal and returns pointer to tree ( single operand )
    std::unique_ptr<Node> parseFinal();
};