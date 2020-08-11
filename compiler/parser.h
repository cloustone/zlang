#pragma once

#include <string>
#include <vector>
#include "token.h"
#include "lexer.h"
#include "error_handler.h"
#include "program_handler.h"
#include "parser_node.h"

namespace zl {

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

    // Expr
    Node* ParseExpr() { return nullptr; }
private:
    Lexer& lexer_;
    ProgramHandler& programHandler_;
    ErrorHandler& errorHandler_;
};


} // namespace zl
