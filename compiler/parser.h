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
    void Advance(Node::Type nonterminal);

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
    Node* ParseVarDeclaration();

    // varBlockDeclaration
    //    : '(' singleVarDeclaration* ')'
    //    ;
    Node* ParseVarBlockDeclaration();

    // singleVarDeclaration:
    //    : IDENTIFIER ':" type ('=' variableInitializer)?
    //    ;
    Node* ParseSingleVarDeclaration();

    // constDeclaraton
    //    : 'const' constBlockDeclaration | singleConstDeclaration
    //    ;
    Node* ParseConstDeclaration();

    // constBlockDeclaration
    //    : '(' singleConstDeclaration* ')'
    //    ;
    Node* ParseConstBlockDeclaration();

    // singleConstDeclaration
    //    : IDENTIFIER (':' IDENTIFIER)? ('=' constExpression)?
    //    ;
    Node* ParseSingleConstDeclaration();

    // functionDeclaration
    //    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
    //    ;
    Node* ParseFunctionDeclaration();

    // formalParameters
    //    : '(' formalParameterList ? ')'
    //    ;
    Node* ParseFormalParameters();

    // qualifiedNameList
    //    : qualifiedName (',' qualifiedName)*
    //    ;
    Node* ParseQualifiedNameList();

    // formalParameterList
    // : formalParameter (',' formalParameter)*
    // ;
    Node* ParseFormalParameterList();
    // formalParameter
    // : IDENTIFIER ':' type
    // ;
    Node* ParseFormalParameter();

    // functionReturnParameters
    //    : ('void' | type) | ('(' typeList ')')
    //    ;
    Node* ParseFunctionReturnParameters();

    // functionBodyDeclaration
    //    : block
    //    ;
    Node* ParseFunctionBodyDeclaration();

    // qualifiedName
    //    : IDENTIFIER ('.' IDENTIFIER)*
    //    ;
    Node* ParseQualifiedName();

    // type
    //    : primitiveType ('[' ']')*
    //    | classType ('[' ']')*
    //    | mapType
    //    ;
    Node* ParseTypeDeclaration();

    // typeList
    //    : type (',' type)*
    //    ;
    Node* ParseTypeList();

    // classType
    //   : qualifiedName
    //   ;
    Node* ParseClassType();

    // mapType
    //    : 'map' '<' mapItemType ','  mapItemType '>' 
    //    ;
    Node* ParseMapType();

    // mapItemType
    //    : primitiveType
    //    | classType
    //    ;
    Node* ParseMapItemType();
    
    // primitiveType
    //    : 'bool'
    //    | 'char'
    //    | 'byte'
    //    | 'short'
    //    | 'int'
    //    | 'long'
    //    | 'float'
    //    | 'double'
    //    | 'string'
    //    ;
    Node* ParsePrimitiveType();

   
    Node* ParseVariableInitializer();

    
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
    Node* ParseArrayInitializer();
    Node* ParseMapInitializer();

    // Expr
    Node* ParseExpr() { return nullptr; }
    Node* ParseConstExpr() { return nullptr; }
private:
    Lexer& lexer_;
    ProgramHandler& programHandler_;
    ErrorHandler& errorHandler_;
};


} // namespace zl
