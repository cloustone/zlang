#pragma once

#include <string>
#include <vector>
#include "token.h"
#include "lexer.h"

namespace zl {

// Analyze tree declaration
class Node {
public:
    std::string name_;
    std::string assic_;
    int type_;
    Node* parent_;
    std::vector<Node*> childs_;

public:
    Node() {}
    Node(const std::string& name, int type, const std::string& val, const Location& location) {}
    ~Node() {}
    int ChildNumber() { return childs_.size(); }
    void AddChild(const Node* node) {}
};

class Parser {
public:
    explicit Parser(Lexer& lexer):lexer_(lexer) {}
    ~Parser() {}
    Node* Build();
private:
    Parser();
    Node* ParseCompilationUnit();
    Node* ParseDeclaration();
    Node* ParsePackageDeclaration();
    Node* ParseImportDeclaration();
    Node* ParseTypeDeclaration();
    Node* ParseClassDeclaration();
    Node* ParseEnumDeclaration();
    Node* ParseConstDeclaration();
    Node* ParseModifier();
    Node* ParseClassBodyDeclaration();
    Node* ParseMemberDeclaration();
    Node* ParseMethodDeclaration();
    Node* ParseVariableDeclaration();
    Node* ParseVariableInitializer();
    Node* ParseArrayInitializer();
    Node* ParseMapInitializer();
    Node* ParseQualifiedName();
private:
    Node* root_;
    Lexer& lexer_;
};

class PackageDeclNode : public Node {};
class ImportDeclNaode : public Node {};


} // namespace zl
