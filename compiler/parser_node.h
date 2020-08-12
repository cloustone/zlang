#pragma once

#include <string>
#include <vector>
#include "token.h"
#include "parser_node.h"

namespace zl {

// Analyze tree declaration
class Node {
public:
    // All nonterminals have a matched node type
    enum class Type: uint8_t {
        ROOT = 0,
        PACKAGE,
        IMPORT,
        USING,
        VAR_BLOCK,
        VAR,
        ID,
        EXPR,
        CONST_EXPR,
        CONST,
        CONST_BLOCK,


        MAX,
    };

    Node() = delete;
    Node(const Location& location) {}
    Node(Type type, const Location& location) {}
    template<typename ...T>
    Node(Type type, const Location& location, T... nodes) {}
    Node(Type type, const Location& location, const std::vector<Node*>& nodes) {}
    Node(Type type, const Token& token) {}

    virtual ~Node() {}
    int ChildNumber() { return childs_.size(); }

    template <typename... Node>
    void AddChild(Node*... nodes) {}

    template <typename... Token>
    void AddChild(Token... tokens) {}


    bool IsPublic() const { return public_; }
    void SetPublic(bool publicity) { public_ = publicity; }
public:
    std::string name_;
    std::string assic_;
    Node* parent_;
    std::vector<Node*> childs_;
    bool public_;
    Type type_;
};


} // namespace zl
