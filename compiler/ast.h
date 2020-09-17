#pragma once
#include <string>
#include <vector>
#include <map>
#include "token.h"
#include "location.h"

namespace zl {
namespace ast {

class Node;
class Visitor {
public:
    virtual void Visit(const Node* node);
};

class Node {
public:
    Node(const Location& location): location_(location) {}
    virtual ~Node() {}
    // The first location of the node
    virtual Location Pos() {  return location_; }
    // The last location of the node
    virtual Location End() { return Location(); }
    virtual void Visit(Visitor& v) { v.Visit(this); }
protected:
    Location location_;
};

class Decl : public Node { 
public:
    Decl(const Location& location): Node(location) { publicity_ = true; }
    void SetPublic(bool publicity) { publicity_ = publicity; }
    bool IsPublic(void) { return publicity_ == true; }
protected:
    bool publicity_;
};

class Stmt : public Node { 
public:
    Stmt(const Location& location): Node(location) {}
    virtual ~Stmt() {}
};

class Expr : public Node {
public:
    Expr(const Location& location): Node(location) {}
    virtual ~Expr() {}
};

class BadExpr : public Expr {
public:
    BadExpr(const Location& location): Expr(location) {}
    virtual ~BadExpr() {}
};

// Common declaration 
class Identifier : public Node {
public:
    Identifier() = delete;
    explicit Identifier(const Token& token):Node(token.location_), name_(token.assic_) {}
    explicit Identifier(const Location& location, const std::string& name)
        :Node(location), name_(name) {}
    virtual ~Identifier() {}
    std::string name_;
};


class QualifiedName : public Node {
public:
    QualifiedName() = delete;
    explicit QualifiedName(const Location& location, std::vector<std::string>& names)
        : Node(location), names_(names) {}
    virtual ~QualifiedName() {} 
    std::vector<std::string> names_;
};

class QualifiedNameList : public Node {
public:
    QualifiedNameList() = delete;
    explicit QualifiedNameList(const Location& location, std::vector<QualifiedName*>& nameList)
        :Node(location), names_(nameList) {}
    ~QualifiedNameList() {
        for (auto qualifiedName : names_) delete qualifiedName;
    }
    std::vector<QualifiedName*> names_;
};

class Comment : public Node {
public:
    Comment() = delete;
    explicit Comment(const Location& location, const std::string& text): Node(location), text_(text) {}
    ~Comment() {}
    std::string text_;
};

//
// Type 
//
class Type : public Node { 
public:
    Type(const Location& location):Node(location) {}
    virtual ~Type() {}
};

class NullType : public Node { 
public:
    NullType(const Location& location):Node(location) {}
    virtual ~NullType() {}
};

class PrimitiveType : public Type {
public:
    PrimitiveType() = delete;
    explicit PrimitiveType(const Location& location, const std::string& name)
        :Type(location), name_(name) {}
    ~PrimitiveType() {}
    std::string name_;
};

class NonPrimitiveType : public Type {
public:
    NonPrimitiveType() = delete;
    explicit NonPrimitiveType(const Location& location, Identifier* name)
        :Type(location), name_(name) {}
    virtual ~NonPrimitiveType() {
        if (name_) delete name_;
    }
    Identifier* name_;
};

class MapType : public Type {
public:
    MapType() = delete;
    explicit MapType(const Location& location, Type* leftType, Type* rightType)
        :Type(location), leftType_(leftType), rightType_(rightType) {}
    ~MapType() {
        if (leftType_) delete leftType_;
        if (rightType_) delete rightType_;
    }
    Type* leftType_;
    Type* rightType_;
};

class ArrayType : public Type {
public:
    ArrayType() = delete;
    explicit ArrayType(const Location& location, Type* type)
        :Type(location), type_(type) {}
    ~ArrayType() {
        if (type_) delete type_;
    }
    Type* type_;
};


//
// Declarations
//
class Expr;
class Identifier;

struct PackageDecl : public Decl {
public:
    PackageDecl() = delete;
    explicit PackageDecl(const Location& location, Identifier* identifier)
        :Decl(location), name_(identifier) {}
    ~PackageDecl() { delete name_; }
    Identifier* name_;
};

class ImportDecl : public Decl {
public:
    ImportDecl() = delete;
    explicit ImportDecl(const Location& location, QualifiedName* name)
        :Decl(location), name_(name) {}
    virtual ~ImportDecl() {
        if (name_) delete name_;
    }
    QualifiedName* name_;
};

class UsingDecl : public Decl {
public:
    UsingDecl() = delete;
    explicit UsingDecl(const Location& location, QualifiedName* qualifiedName, Identifier* aliasName)
        :Decl(location), qualifiedName_(qualifiedName), aliasName_(aliasName) {}
    virtual ~UsingDecl() { 
        delete qualifiedName_; 
        delete aliasName_;
    }
    QualifiedName* qualifiedName_;
    Identifier* aliasName_;
};

class VarInitializer;
class Expr;

class VarInitializer : public Node {
public:
    VarInitializer() = delete;
    explicit VarInitializer(const Location& location, Expr* expr)
        :Node(location), expr_(expr) {}
    ~VarInitializer() { 
        if (expr_) delete expr_; 
    }
    Expr* expr_;
};

class VariableDecl : public Decl {
public:
    VariableDecl() = delete;
    explicit VariableDecl(const Location& location, Identifier* name, Type* type, 
            VarInitializer* varInitializer)
        :Decl(location), name_(name), type_(type), varInitializer_(varInitializer) {}
    ~VariableDecl() { 
        if (name_) delete name_; 
        if (type_) delete type_;
        if (varInitializer_) delete varInitializer_;
    }
    Identifier* name_;
    Type* type_;
    VarInitializer* varInitializer_;
};

class VariableBlockDecl: public Decl {
public:
    VariableBlockDecl() = delete;
    explicit VariableBlockDecl(const Location& location, std::vector<VariableDecl*> variables)
        :Decl(location), variables_(variables) {}
    ~VariableBlockDecl() {
        for (auto p : variables_) delete p;
    }
    std::vector<VariableDecl*> variables_;
};

// singleConstDeclaration
//    : IDENTIFIER (':' type)? ('=' constExpression)?
//    ;
class ConstDecl: public Decl {
public:
    ConstDecl() = delete;
    explicit ConstDecl(const Location& location, Identifier* name, Type* type, VarInitializer* varInitializer)
        :Decl(location), name_(name), type_(type), varInitializer_(varInitializer) {}
    virtual ~ConstDecl() {
        if (name_) delete name_;
        if (type_) delete type_;
        if (varInitializer_) delete varInitializer_;
    }
    Identifier* name_;
    Type* type_;
    VarInitializer* varInitializer_;
};

// constBlockDeclaration
//    : '(' singleConstDeclaration* ')'
//    ;
class ConstBlockDecl: public Decl {
public:
    ConstBlockDecl() = delete;
    explicit ConstBlockDecl(const Location& location, std::vector<ConstDecl*> fields)
        :Decl(location), fields_(fields) {}
    ~ConstBlockDecl() {
        for (auto field : fields_) delete field;
    }
    std::vector<ConstDecl*> fields_;
};

class FormalParameterList;
class ReturnParameterList;
class FunctionBlockDecl;

// functionDeclaration
//    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBlockDeclaration
//    ;
class FunctionDecl: public Decl {
public:
    FunctionDecl() = delete;
    explicit FunctionDecl(const Location& location, Identifier* id, FormalParameterList* formalParameterList,
            ReturnParameterList* returnParameterList, FunctionBlockDecl* functionBlockDecl)
        :Decl(location), name_(id), formalParameterList_(formalParameterList),
        returnParameterList_(returnParameterList), functionBlockDecl_(functionBlockDecl) {}

    virtual ~FunctionDecl() {
        if (formalParameterList_) delete formalParameterList_;
        if (returnParameterList_) delete returnParameterList_;
        if (functionBlockDecl_) delete functionBlockDecl_;
    }
    Identifier* name_;
    FormalParameterList* formalParameterList_;
    ReturnParameterList* returnParameterList_;
    FunctionBlockDecl* functionBlockDecl_;
};

class FunctionBlockDecl: public Node {
public:
    FunctionBlockDecl() = delete;
    explicit FunctionBlockDecl(const Location& location) :Node(location) {}
    virtual ~FunctionBlockDecl() {
        for (auto p : nodes_)
            delete p;
    }
    void Add(Node* n);
    std::vector<Node*> nodes_;
};


// formalParameter
// : IDENTIFIER ':' type
// ;
class FormalParameter : public Decl {
public:
    FormalParameter() = delete;
    explicit FormalParameter(const Location& location, Identifier* name, Type* type)
        :Decl(location), name_(name), type_(type) {}
    virtual ~FormalParameter() {
        if (name_) delete name_;
        if (type_) delete type_;
    }
    Identifier* name_;
    Type* type_;
};


// formalParameters
//    : '(' formalParameterList ? ')'
//    ;
// formalParameterList
// : formalParameter (',' formalParameter)*
// ;
class FormalParameterList : public Decl {
public:
    FormalParameterList() = delete;
    explicit FormalParameterList(const Location& location, const std::vector<FormalParameter*>& params)
        :Decl(location), formalParameters_(params) {}
    virtual ~FormalParameterList() {
        for (auto p : formalParameters_) delete p;
    }
    std::vector<FormalParameter*> formalParameters_;
};

// functionReturnParameters
//    : ('void' | type) | ('(' typeList ')')
//    ;
class ReturnParameterList : public Node {
public:
    ReturnParameterList() = delete;
    explicit ReturnParameterList(const Location& location, const std::vector<Type*>& params)
        :Node(location), types_(params) {}
    virtual ~ReturnParameterList() {
        for (auto p : types_) delete p;
    }
    std::vector<Type*> types_;
};

// interfaceMethodDecl
//    : IDENTIFIER formalParameters (':' (type | 'void'))? ('throw' qualifiedNameList)?
//    ;
class InterfaceMethodDecl: public Node {
public:
    InterfaceMethodDecl() = delete;
    explicit InterfaceMethodDecl(const Location& location, Identifier* id, FormalParameterList* formalParameterList,
            ReturnParameterList* returnParameterList)
        :Node(location), name_(id), formalParameterList_(formalParameterList),
        returnParameterList_(returnParameterList) {}

    virtual ~InterfaceMethodDecl() {
        if (name_) delete name_;
        if (formalParameterList_) delete formalParameterList_;
        if (returnParameterList_) delete returnParameterList_;
    }
    Identifier* name_;
    FormalParameterList* formalParameterList_;
    ReturnParameterList* returnParameterList_;
};


// interfaceDeclaration
//    : 'interface' '{' interfaceMethodDecl* '}'
//    ;
class InterfaceDecl : public Decl {
public:
    InterfaceDecl() = delete;
    explicit InterfaceDecl(const Location& location, Identifier* name,
            const std::vector<InterfaceMethodDecl*>& methods)
        :Decl(location), name_(name), methods_(methods) {}
    virtual ~InterfaceDecl() {
        if (name_) delete name_;
        for (auto p : methods_) delete p;
    }
    Identifier* name_;
    std::vector<InterfaceMethodDecl*> methods_;
};

// classBodyDeclaration
//    : (classSectionSpecifier)? classMethodDeclaration |classVariableDeclaration 
//    ;
class ClassBodyDecl : public Decl {
public:
    ClassBodyDecl() = delete;
    explicit ClassBodyDecl(const Location& location, const std::vector<VariableDecl*>& variables,
            const std::vector<FunctionDecl*>& functions)
        : Decl(location), variables_(variables), functions_(functions) {} 
    virtual ~ClassBodyDecl() {
        for (auto p : variables_) delete p;
        for (auto p : functions_) delete p;
    }
    std::vector<VariableDecl*> variables_;
    std::vector<FunctionDecl*> functions_;
};

// classDeclaration
//    : 'class' IDENTIFIER ('extends' qualifiedName)?
//      ('implements' qualifiedNameList)? 
//     '{' classBodyDeclaration* '}'
//    ;
class ClassDecl : public Decl {
public:
    ClassDecl() = delete;
    explicit ClassDecl(const Location& location, Identifier* name,
           QualifiedNameList* interfaceList, ClassBodyDecl* classBody)
        :Decl(location), name_(name), interfaceList_(interfaceList), classBody_(classBody) {}
    virtual ~ClassDecl() {
        if (name_) delete name_;
        if (interfaceList_) delete interfaceList_;
        if (classBody_) delete classBody_;
    }
    Identifier* name_;
    QualifiedNameList* interfaceList_;
    ClassBodyDecl* classBody_;
};

class UnknownStmt : public Stmt {
public:
    UnknownStmt() = delete;
    explicit UnknownStmt(const Location& location) : Stmt(location) {}
};


// labelStatement
//    : IDENTIFIER ':' statement
//    ;
class LabelStmt : public Stmt {
public:
    LabelStmt() = delete;
    explicit LabelStmt(const Location& location, const std::string& name) :Stmt(location), labelName_(name) {}
    const std::string labelName_;
};

// ifStatement
//    : 'if' '(' expression ')' statementBlock ('elif' statementBlock)* ('else' statementBlock)?
//    ;
class IfStmt : public Stmt {
public:
    IfStmt() = delete;
    explicit IfStmt(const Location& location, Expr* conditionExpr, Stmt* ifBlockStmt,
            const std::vector<Stmt*>& elifBlockStmts, Stmt* finalStmt) : Stmt(location),
            conditionExpr_(conditionExpr), ifBlockStmt_(ifBlockStmt), elifBlockStmts_(elifBlockStmts), finalStmt_(finalStmt) {}
    Expr* conditionExpr_;
    Stmt* ifBlockStmt_;
    std::vector<Stmt*> elifBlockStmts_;
    Stmt* finalStmt_;
};


// forStatement
//    : 'for' 
//      '('forInitializer?  ';'expression? ';' expressionList? ')'  
//      statement
//   ;
class ForStmt : public Stmt {
public:
};

// iterableObject
//    : IDENTIFIER
//    | mapInitializer
//    | arrayInitializer
//    ;
class IterableObject : public Node {
public:
};


// whileStatement
//    : 'while' '(' expression ')' statement
//    ;
class WhileStmt : public Stmt {
public:
};

// doStatement
//    : 'do' statement 'while' '(' expression ')'
//    ;
class DoStmt : public Stmt {
public:
};

// switchStatement
//    : 'switch' '(' expression ')' '{' switchCase*defaultCase? '}'
//    ;
class SwitchStmt : public Stmt {
public:
};

// returnStatement
//    : 'return' expression? ';'
//    ;
class ReturnStmt : public Stmt {
public:
};

// breakStatement
//    : 'break' ';'
//    ;
class BreakStmt : public Stmt {
public:
};

// continueStatement
// continueStatement
//    : 'continue' IDENTIFIER? ';'
//    ;
class ContinueStmt : public Stmt {
public:
};

// assertStatement
//    : 'assert' '(' expression ')' ';'
//    ;
class AssertStmt : public Stmt {
public:
};

// throwStatement
//    : 'throw' expression ';'
//    ;
class ThrowStmt : public Stmt {
public:
};


// tryStatement
//    : 'try' block catchParts? finallyPart?
//    ;
class TryStmt : public Stmt {
public:
};

// catchPart
//    : 'catch' '('catchType IDENTIFIER ')' block 
//    ;
class CatchStmt : public Stmt {
public:
};

// finallyPart
//    : 'finally' block
//    ;
class FinallyStmt : public Stmt {
public:
};

// expressionStatement
//    : expression ';'
//    ;
class ExprStmt : public Stmt {
public:
};





















} // namespacde ast
} // namespace zl
