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

    // The function check wether the next token is matched with specified
    // token, report errors if not matched
    Location Expect(Token::TokenType token);

    // The function advance next token and update token variable
    void Next();
    // The function lookhead to check wether next token match specified token
    bool Match(Token::TokenType type);

    // Back will go back one token
    void Back() {}

    // The function just output systax error messge to ErrorHandler
    void SyntaxErrorAt(const Token& token, const std::string& msg);
    void SyntaxErrorAt(const Location& location, const std::string& msg);
    void SyntaxError(const std::string& msg);
    void ErrorExpected(const Location& location, const std::string& msg);

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
    ast::VariableDecl* ParseSingleVarDeclaration();

    // constDeclaraton
    //    : 'const' constBlockDeclaration | singleConstDeclaration
    //    ;
    ast::Decl* ParseConstDeclaration();

    // constBlockDeclaration
    //    : '(' singleConstDeclaration* ')'
    //    ;
    ast::ConstBlockDecl* ParseConstBlockDeclaration();

    // singleConstDeclaration
    //    : IDENTIFIER (':' IDENTIFIER)? ('=' constExpression)?
    //    ;
    ast::ConstDecl* ParseSingleConstDeclaration();

    // functionDeclaration
    //    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
    //    ;
    ast::FunctionDecl* ParseFunctionDeclaration();

    // formalParameters
    //    : '(' formalParameterList ? ')'
    //    ;
    ast::FormalParameterList* ParseFormalParameters();

    // formalParameterList
    // : formalParameter (',' formalParameter)*
    // ;
    ast::FormalParameterList* ParseFormalParameterList();
    // formalParameter
    // : IDENTIFIER ':' type
    // ;
    ast::FormalParameter* ParseFormalParameter();

    // functionReturnParameters
    //    : ('void' | type) | ('(' typeList ')')
    //    ;
    ast::ReturnParameterList* ParseFunctionReturnParameters();

    // functionBodyDeclaration
    //    : block
    //    ;
    ast::FunctionBlockDecl* ParseFunctionBlockDeclaration();

    // qualifiedName
    //    : IDENTIFIER ('.' IDENTIFIER)*
    //    ;
    ast::QualifiedName* ParseQualifiedName();

    // qualifiedNameList
    //    : qualifiedName (',' qualifiedName)*
    //    ;
    ast::QualifiedNameList* ParseQualifiedNameList();

    // interfaceMethodDecl
    //    : IDENTIFIER formalParameters (':' (type | 'void'))? ('throw' qualifiedNameList)?
    // 
    ast::InterfaceMethodDecl* ParseInterfaceMethod();

    // interfaceDeclaration
    //    : 'interface' '{' interfaceMethodDecl* '}'
    //    ;
    ast::InterfaceDecl* ParseInterfaceDecl();

    // classDeclaration
    //    : 'class' IDENTIFIER ('extends' qualifiedName)?
    //     '{' classBodyDeclaration* '}'
    //    ;
    ast::ClassDecl* ParseClassDeclaration();

    // classBodyDeclaration
    //    : (classSectionSpecifier)? classMethodDeclaration |classVariableDeclaration 
    //    ;
    ast::ClassBodyDecl* ParseClassBody();


    // type
    //    : primitiveType ('[' ']')*
    //    | classType ('[' ']')*
    //    | mapType
    //    ;
    ast::Type* ParseType();

    // typeList
    //    : type (',' type)*
    //    ;
    std::vector<ast::Type*> ParseTypeList();

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

    ast::VarInitializer* ParseVariableInitializer();

    //
    // statements
    //    : statement*
    //    ;
    std::vector<ast::Stmt*> ParseStatements();

    // statement
    //     : localVariableDeclarationStatement
    //     | ifStatement
    //     | forStatement
    //     | foreachStatement
    //     | doStatement
    //     | whileStatement
    //     | returnStatement
    //     | tryStatement
    //     | throwStatement
    //     | breakStatement
    //     | continueStatement
    //     | assertStatement
    //     | expressionStatement
    //     | labelStatement
    //     | blockStatement
    //     ;
    ast::Stmt* ParseStatement();

    // statementBlock 
    //     : '{' statements '}'
    //     ;
    ast::Node* ParseStatementBlock();

    // local variable declaration statement
    // localVariableDeclarationStatement
    //    : variableDeclaration 
    //    ;
    ast::Decl* ParseLocalVaraible();


    // labelStatement
    //    : IDENTIFIER ':' 
    //    ;
    ast::Stmt* ParseLabelStatement();

    // blockStatement
    //    : '{' statements '}'
    //    ;
    ast::Stmt* ParseBlockStatement();

    // ifStatement
    //    : 'if' expression  statementBlock ('elif' statementBlock)* ('else' statementBlock)?
    //    ;
    ast::Stmt* ParseIfStatement();

    ast::ExprStmt* ParseExprStatement();
    ast::ExprStmts* ParseExprStatements();

    // forStatement
    //    : 'for' 
    //      forInitializer?  ';'expression? ';' expressionList?   
    //      statement
    //   ;
    ast::Stmt* ParseForStatement();

    // forInitializer
    //    : variableDeclaration
    //    | expressionList
    //    ;
    ast::Stmt* ParseForInitializer();

    // foreachStatement
    //    : 'foreach' '(' foreachVariable (',' foreachVariable)? 'in' iterableObject ')' statement
    //    ;
    ast::Stmt* ParseForeachStatement();

    // foreachVariable
    //     : IDENTIFIER (':' type)?
    //     ;
    ast::Node* ParseForeachVariable();

    // iterableObject
    //    : IDENTIFIER
    //    | mapInitializer
    //    | arrayInitializer
    //    ;
    ast::Node* ParseIterableObject();

    // whileStatement
    //    : 'while' '(' expression ')' statement
    //    ;
    ast::Stmt* ParseWhileStatement();

    // doStatement
    //    : 'do' statement 'while' '(' expression ')'
    //    ;
    ast::Stmt* ParseDoStatement();

    // switchStatement
    //    : 'switch' '(' expression ')' '{' switchCase*defaultCase? '}'
    //    ;
    ast::Stmt* ParseSwitchStatement();

    // switchCase
    //    : ('case' expression ':')+ statement
    //    ;
    ast::Node* ParseSwitchCase();

    // defaultCase
    //    : 'default' ':' statement
    //    ;
    ast::Node* ParseSwitchDefault();

    // returnStatement
    //    : 'return' expression? ';'
    //    ;
    ast::Stmt* ParseReturnStatement();

    // breakStatement
    //    : 'break' ';'
    //    ;
    ast::Stmt* ParseBreakStatement();

    // continueStatement
    // continueStatement
    //    : 'continue' IDENTIFIER? ';'
    //    ;
    ast::Stmt* ParseContinueStatement();

    // assertStatement
    //    : 'assert' '(' expression ')' ';'
    //    ;
    ast::Stmt* ParseAssertStatement();

    // throwStatement
    //    : 'throw' expression ';'
    //    ;
    ast::Stmt* ParseThrowStatement();

    // tryStatement
    //    : 'try' block catchParts? finallyPart?
    //    ;
    ast::Stmt* ParseTryStatement();

    // catchParts
    //    : catchPart*
    //    ;
    ast::Stmt* ParseCatchParts();

    // catchPart
    //    : 'catch' '('catchType IDENTIFIER ')' block 
    //    ;
    ast::Stmt* ParseCatchPart();

    // catchType
    //    : qualifiedName ( '|' qualifiedName)*
    //    ;
    ast::Node* ParseCatchType();

    // finallyPart
    //    : 'finally' block
    //    ;
    ast::Stmt* ParseFinallyPart();


    Node* ParseArrayInitializer();
    Node* ParseMapInitializer();

    // Expr
    Expr* ParseExpr() { return nullptr; }
    Expr* ParseConstExpr() { return nullptr; }

    // Statement
    Stmt* ParseStmt() { return nullptr; }

    // Identifier
    ast::Identifier* ParseIdentifier() { return nullptr; }
    
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

    // Next token look ahead
    Token token_;
    std::string literal_;
    Location location_;
    bool trace_;
};


} // namespace zl
