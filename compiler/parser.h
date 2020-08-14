#pragma once

#include <string>
#include <vector>
#include "token.h"
#include "lexer.h"
#include "error_handler.h"
#include "program_handler.h"
#include "ast.h"
#include "scope.h"

namespace zl {
using namespace ast;

class Parser {
public:
    explicit Parser(Lexer& lexer, ProgramHandler& programHandler, ErrorHandler& errorHandler):
        lexer_(lexer), programHandler_(programHandler), errorHandler_(errorHandler) {}
    ~Parser() {}
    void Build(std::vector<Node*>& decls);
private:
    Parser() = delete;
    // The function just output systax error messge to ErrorHandler
    void SyntaxErrorAt(const Token& token, const std::string& msg);
    void SyntaxErrorAt(const Location& location, const std::string& msg);
    void SyntaxError(const std::string& msg);
    // The function will advance token until one of followset found
    // The followset is the list of valid tokens that can follow a production,
    // if it is empty, exactly one (non-EOF) token is consumed to ensure progress.
    template<typename ...Args>
    void Advance(Args... args) {}

    // SyncStmt advances to the next statement, Used for synchronization after an error.
    void SyncStmt();
    // SyncDecl advances to the next declaration. Used for synchronization after an error.
    void SyncDecl(); 

    // compilationUnit
    //    : scopeModifier? declaration* EOF
    //    ;
    void ParseCompilationUnit(std::vector<Node*>& decls);

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
    ast::Decl* ParseDeclaration(const Token& token);
    
    // packageDeclaration
    //    : 'package' qualifiedName  
    ast::Decl* ParsePackageDeclaration();

    // importDeclaration
    //   : 'import' qualifiedName 
    //    ;
    ast::Decl* ParseImportDeclaration();

    // usingDeclaration
    //    : 'using' qualifiedName '=' IDENTIFIER
    //    ;
    ast::Decl* ParseUsingDeclaration();

    // varDeclaration
    //        : 'var' varBlockDeclaration | singleVarDeclaration
    //        ;
    ast::Decl* ParseVarDeclaration();

    // varBlockDeclaration
    //    : '(' singleVarDeclaration* ')'
    //    ;
    ast::Decl* ParseVarBlockDeclaration();

    // singleVarDeclaration:
    //    : IDENTIFIER ':" type ('=' variableInitializer)?
    //    ;
    ast::Decl* ParseSingleVarDeclaration();

    // constDeclaraton
    //    : 'const' constBlockDeclaration | singleConstDeclaration
    //    ;
    ast::Decl* ParseConstDeclaration();

    // constBlockDeclaration
    //    : '(' singleConstDeclaration* ')'
    //    ;
    ast::Decl* ParseConstBlockDeclaration();

    // singleConstDeclaration
    //    : IDENTIFIER (':' IDENTIFIER)? ('=' constExpression)?
    //    ;
    ast::Decl* ParseSingleConstDeclaration();

    // functionDeclaration
    //    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
    //    ;
    ast::Decl* ParseFunctionDeclaration();

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
    ast::Decl* ParseClassDeclaration();
    ast::Decl* ParseInterfaceDeclaration();

    ast::Decl* ParseClassBodyDeclaration();
    ast::Decl* ParseMemberDeclaration();
    ast::Decl* ParseMethodDeclaration();
    Node* ParseArrayInitializer();
    Node* ParseMapInitializer();

    // Expr
    Node* ParseExpr() { return nullptr; }
    Node* ParseConstExpr() { return nullptr; }
    
    // Scoping support
    void OpenScope();
    void CloseScope();
    void OpenLabelScope();
    void CloseLabelScope();
    void Declare();
    // If x is an identifier, tryResolve attempts to resolve x by looking up
    // the object it denotes. If no object is found and collectUnresolved is
    // set, x is marked as unresolved and collected in the list of unresolved
    // identifiers.
    //
    void TryResolve(ast::Identifier* id, bool collectedUnresolved = true);
    void Resolve(ast::Identifier* id, bool collectedUnresolved = true);
private:
    Lexer& lexer_;
    ProgramHandler& programHandler_;
    ErrorHandler& errorHandler_;
    int syncPos_;
    int syncCount_;
    ast::Scope* pkgScope_;
    ast::Scope* topScope_;
    ast::Scope* labelScope_;
    std::vector<ast::Identifier*> unresolved_;
    std::vector<ast::ImportDecl*> imports_;
};


} // namespace zl
