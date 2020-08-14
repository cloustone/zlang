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
    explicit QualifiedName(const Location& location, std::vector<std::string>& names);
    virtual ~QualifiedName();
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

class VariableDecl : public Decl {
public:
    VariableDecl() = delete;
    explicit VariableDecl(const Location& location, Identifier* name, Type* type, 
            Expr* varInitializer)
        :Decl(location), name_(name), type_(type), varInitializer_(varInitializer) {}
    ~VariableDecl() { 
        if (name_) delete name_; 
        if (type_) delete type_;
        if (varInitializer_) delete varInitializer_;
    }
    Identifier* name_;
    Type* type_;
    Expr* varInitializer_;
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
    explicit ConstDecl(const Location& location, Identifier* name, Type* type, Expr* initializer)
        :Decl(location), name_(name), type_(type), initializer_(initializer) {}
    virtual ~ConstDecl() {
        if (name_) delete name_;
        if (type_) delete type_;
        if (initializer_) delete initializer_;
    }
    Identifier* name_;
    Type* type_;
    Expr* initializer_;
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
class FunctionBodyDecl;

// functionDeclaration
//    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
//    ;
class FunctionDecl: public Decl {
public:
    FunctionDecl() = delete;
    explicit FunctionDecl(const Location& location, FormalParameterList* formalParameterList,
            ReturnParameterList* returnParameterList, FunctionBodyDecl* functionBodyDecl)
        :Decl(location), formalParameterList_(formalParameterList),
        returnParameterList_(returnParameterList), functionBodyDecl_(functionBodyDecl) {}

    virtual ~FunctionDecl() {
        if (formalParameterList_) delete formalParameterList_;
        if (returnParameterList_) delete returnParameterList_;
        if (functionBodyDecl_) delete functionBodyDecl_;
    }
    FormalParameterList* formalParameterList_;
    ReturnParameterList* returnParameterList_;
    FunctionBodyDecl* functionBodyDecl_;
};

class FunctionBodyDecl: public Decl {
public:
    FunctionBodyDecl() = delete;
    explicit FunctionBodyDecl(const Location& location, Decl* block)
        :Decl(location), block_(block) {}
    virtual ~FunctionBodyDecl() {
        if (block_) delete block_;
    }
    Decl* block_;
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










} // namespacde ast
} // namespace zl
