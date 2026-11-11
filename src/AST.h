#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <utility>

#include "Tokens.h"

enum class ValueType {
    None,
    Int,
    Float,
    String,
    Identifier,
    Assignment,
    Error
};

enum class NodeType {
    Program,
    Number, String, Identifier, Constant,
    BinaryOperation, LangFunction, Negate
};

struct Node {
    virtual ~Node() = default;
    virtual NodeType type() const = 0;
};

struct Program : Node {
    std::vector<std::unique_ptr<Node>> lines;

    NodeType type() const override { return NodeType::Program; }

};

// -------------------- single value tokens (terminals) --------------------
struct Number : Node {
    Token token;
    explicit Number(Token pValue) :
        token{ std::move(pValue) } {
        if (token.mType != TokenType::Number)
            throw std::runtime_error("Number node doesnt contain number token");
    }

    NodeType type() const override { return NodeType::Number; }
};

struct String : Node {
    Token token;
    explicit String(Token pValue) :
        token{ std::move(pValue) } {
        if (token.mType != TokenType::String)
            throw std::runtime_error("String node doesnt contain string token");
    }

    NodeType type() const override { return NodeType::String; }

};

struct Identifier : Node {
    Token token;
    explicit Identifier(Token pValue) :
        token { std::move(pValue) } {
        if (token.mType != TokenType::Identifier)
            throw std::runtime_error("Identifier node doesnt contain identifier token");
    }

    NodeType type() const override { return NodeType::Identifier; }

};

struct Constant : Node {
    Token token;
    explicit Constant(Token pValue) :
        token { std::move(pValue) } {
        if (token.mType != TokenType::Constant)
            throw std::runtime_error("Constant node doesnt contain constant token");
    }

    NodeType type() const override { return NodeType::Constant; }

};

// --------- operations, expresisons, and functions (nonterminals) ---------

// Binary operation
struct BinaryOperation : Node {

    Token token;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    explicit BinaryOperation(Token pValue, std::unique_ptr<Node> pLeft = nullptr,
                                           std::unique_ptr<Node> pRight = nullptr) :
        token{ std::move(pValue) },
        left{ std::move(pLeft) },
        right{ std::move(pRight) } {

        if (token.mType != TokenType::Operator)
            throw std::runtime_error("BinaryOperation node doesnt contain operator token");
    }

    NodeType type() const override { return NodeType::BinaryOperation; }

};

// Unary operations
struct LangFunction : Node {

    Token functionToken;
    std::vector<std::unique_ptr<Node>> arguments;

    explicit LangFunction(Token pValue) :
        functionToken { std::move(pValue) } {
        if (functionToken.mType != TokenType::LangFunction)
            throw std::runtime_error("Function node doesnt contain function token");
    }

    NodeType type() const override { return NodeType::LangFunction; }
};

struct Negate : Node {

    Token negateToken;
    std::unique_ptr<Node> operand;

    explicit Negate(Token pValue, std::unique_ptr<Node> pOperand = nullptr) :
        negateToken { std::move(pValue) },
        operand { std::move(pOperand) } {

        if (!(negateToken.mValue == "-" && negateToken.mType == TokenType::Operator))
            throw std::runtime_error("Negate node doesnt contain negate token");
    }

    NodeType type() const override { return NodeType::Negate; }

};