#pragma once

#include <string>
#include <vector>
#include "token.h"
#include "lexer.h"
#include "error_handler.h"
#include "program_handler.h"

namespace zl {

// Analyze tree declaration
class Node {
public:
    std::string name_;
    std::string assic_;
    int type_;
    Node* parent_;
    std::vector<Node*> childs_;
    bool public_;

public:
    Node() {}
    Node(const std::string& name, int type, const std::string& val, const Location& location) {}
    ~Node() {}
    int ChildNumber() { return childs_.size(); }
    void AddChild(const Node* node) {}
    bool IsPublic() const { return public_; }
    void SetPublic(bool publicity) { public_ = publicity; }
};

class Parser {
public:
    explicit Parser(Lexer& lexer, ProgramHandler& programHandler, ErrorHandler& errorHandler):
        lexer_(lexer), programHandler_(programHandler), errorHandler_(errorHandler) {}
    ~Parser() {}
    Node* Build();
private:
    Parser() = delete;
    // The function just output systax error messge to ErrorHandler
    void SyntaxErrorAt(const Token& token, const std::string& msg);
    void SyntaxErrorAt(const Location& location, const std::string& msg);
    void SyntaxError(const std::string& msg);

    // The function will advance token until one of followset found
    // The followset is the list of valid tokens that can follow a production,
    // if it is empty, exactly one (non-EOF) token is consumed to ensure progress.
    void Advance(int followset[]);
    void Advance(const std::vector<int>& followset);

    // compilationUnit
    //    : scopeModifier? declaration* EOF
    //    ;
    Node* ParseCompilationUnit();

    // declaration
    //    : packageDeclaration 
    //    | importDeclaration 
    //    | usingDeclaration
    //    | constDeclaration
    //    | varDeclaration
    //    | functionDeclaration
    //    | classDeclaration
    //    | interfaceDeclaration
    //    ;
    Node* ParseDeclaration();
    
    // packageDeclaration
    //    : 'package' qualifiedName  
    Node* ParsePackageDeclaration();

    // importDeclaration
    //   : 'import' qualifiedName 
    //    ;
    Node* ParseImportDeclaration();

    // usingDeclaration
    //    : 'using' qualifiedName '=' IDENTIFIER
    //    ;
    Node* ParseUsingDeclaration();

    // varDeclaration
    //        : 'var' varBlockDeclaration | singleVarDeclaration
    //        ;
    Node* ParseVarDeclarartion();

    // varBlockDeclaration
    //    : '(' singleVarDeclaration* ')'
    //    ;
    Node* ParseVarBlockDeclaration();

    // singleVarDeclaration:
    //    : IDENTIFIER ':" IDENTIFIER ('=' expression)?
    //    ;
    Node* ParseSingleVarDeclaration();

    Node* ParseConstDeclaration();
    Node* ParseFunctionDeclaration();

    // classDeclaration
    //    : 'class' IDENTIFIER ('extends' qualifiedName)?
    //      ('implements' qualifiedNameList)? 
    //     '{' classBodyDeclaration* '}'
    //    ;
    Node* ParseClassDeclaration();
    Node* ParseInterfaceDeclaration();

    Node* ParseClassBodyDeclaration();
    Node* ParseMemberDeclaration();
    Node* ParseMethodDeclaration();
    Node* ParseVariableDeclaration();
    Node* ParseVariableInitializer();
    Node* ParseArrayInitializer();
    Node* ParseMapInitializer();
    Node* ParseQualifiedName();
private:
    Lexer& lexer_;
    ProgramHandler& programHandler_;
    ErrorHandler& errorHandler_;
};

class PackageDeclNode : public Node {
public:
    explicit PackageDeclNode(const Location& location, Node* child) {}
};
class ImportDeclNode : public Node {
public:
    explicit ImportDeclNode(const Location& location, Node* child);
};
class UsingDeclNode : public Node {
public:
    explicit UsingDeclNode(const Location& location, Node* qualifiedName, const Token& token) {}
};

class VarBlockDeclNode : public Node {
public:
    explicit VarBlockDeclNode(const Location& location, std::vector<Node*>& nodes) {}
};

class VarDeclNode : public Node {
public:
    explicit VarDeclNode(const Location& location, std::vector<Node*>& nodes) {}
};








} // namespace zl
