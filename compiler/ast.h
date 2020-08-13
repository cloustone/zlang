#pragma once
#include "location.h"
#include <string>
#include <vector>
#include <map>

namespace zl {
namespace ast {

class Visitor {
public:
    virtual void Visit(const Node* node);
};

class Node {
public:
    virtual const Location Pos(); // The first location of the node
    virtual const Location End(); // The last location of the node
    virtual void Visit(Visitor& v) { v.Visit(this); }
};

class Decl : public Node { };
class Stmt : public Node { };
class Expr : public Node { };

// Common declaration 
class QualifiedName : public Node {
public:
    QualifiedName() = delete;
    explicit QualifiedName(const Location& location, std::vector<std::string>& names);
    ~QualifiedName();
    const Location Pos(); 
    const Location End(); 
    std::vector<std::string> names_;
};

class QualifiedNameList : public Node {
public:
    QualifiedNameList() = delete;
    explicit QualifiedNameList(const Location& location, std::vector<QualifiedName>& nameList)
        :location_(location), names_(nameList) {}
    ~QualifiedNameList() {
        for (auto qualifiedName : names_)
            delete qualifiedName;
    }
    const Location Pos(); 
    const Location End(); 
    std::vector<QualifiedName*> names_;
};

class Comment : public Node {
public:
    Comment() = delete;
    explicit Comment(const Location& location, const std::string& text): location_(location), text_(text) {}
    ~Comment() {}
    const Location Pos(); 
    const Location End(); 
pubic:
    Location location_;
    std::string text_;
};

//
// Type 
//
class Type : public Node { };
class PrimitiveType : public Type {
public:
    PrimitiveType() = delete;
    explicit PrimitiveType(const Location& location, const std::string& name)
        :location_(location), name_(name) {}
    ~PrimitiveType() {}
    const Location Pos(); 
    const Location End(); 
    Location location_;
    std::string name_;
};

class NonPrimitiveType : public Type {
public:
    NonPrimitiveType() = delete;
    explicit NonPrimitiveType(const location& location, Identifier* name)
        :location_(location), name_(name) {}
    virtual ~NonPrimitiveType() {
        if (name_) delete name_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Identifier* name_;
};

class MapType : public Type {
public:
    MapType() = delete;
    explicit MapType(const location& location, Type* leftType, Type* rightType)
        :location_(location), leftType_(leftType), rightType_(rightType) {}
    ~MapType() {
        if (leftType_) delete leftType_;
        if (rightType_) delete rightType_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Type* leftType_;
    Type* rightType_;
};

class ArrayType : public Type {
public:
    ArrayType() = delete;
    explicit ArrayType(const location& location, Type* type)
        :location_(location), type_(type) {}
    ~ArrayType() {
        if (type_) delete type_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
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
        :location_(location), identifier_(identifier) {}
    ~PackageDecl() { delete name_; }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Identifier* name_;
};

class ImportDecl : public Decl {
public:
    ImportDecl() = delete;
    explicit ImportDecl(const Location& location, Identifier* identifier, QualifiedName* path);
    ~ImportDecl();
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Identifier* name_;
    QualifiedName* path_;
};

class UsingDecl : public Decl {
public:
    UsingDecl() = delete;
    explicit UsingDecl(const Location& location, QualifiedNam* qualifiedName, Identifier* identifier)
        :location_(location), qualifiedName_(qualifiedName), identifier_(identifier) {}
    ~PackageDecl() { 
        delete name_; 
        delete identifier_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    QualifiedName* qualifiedName_;
    Identifier* identifier_;
};


class VariableDecl : public Decl {
public:
    VariableDecl() = delete;
    explicit VariableDecl(const Location& location, Identifier* name, TypeDecl* typeDecl, 
            Expr* varInitializer)
        :location_(location), name_(name), typeDecl_(typeDecl), varInitializer_(varInitializer) {}
    ~VariableDecl() { 
        if (name_) delete name_; 
        if (typeDecl_) delete typeDecl_;
        if (varInitializer_) delete varInitializer_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Identifier* name_;
    TypeDecl* typeDecl_;
    Expr* varInitializer_;
};

class VariableBlockDecl: public Decl {
public:
    VariableBlockDecl() = delete;
    explicit VariableBlockDecl(const Location& location, std::vector<VariableDecl*> variables)
        :location_(location), variables_(variables) {}
    ~VariableBlockDecl() {
        for (auto p : variables_)
            delete p;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    std::vector<VariableDecl*> variables_;
};

// singleConstDeclaration
//    : IDENTIFIER (':' type)? ('=' constExpression)?
//    ;
class ConstDecl: public Decl {
public:
    ConstDecl() = delete;
    explicit ConstDecl(const Location& location, Identifier* name, TypeDecl* typeDecl, Expr* initializer)
        :location_(location), name_(name), type_(typeDecl), initializer_(initializer_) {}
    ~ConstBlockDecl() {
        if (name_) delete name_;
        if (typeDecl_) delete typeDecl_;
        if (initializer_) delete initializer_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    TypeDecl* type_;
    Expr* initializer_;
};

// constBlockDeclaration
//    : '(' singleConstDeclaration* ')'
//    ;
class ConstBlockDecl: public Decl {
public:
    ConstBlockDecl() = delete;
    explicit ConstBlockDecl(const Location& location, std::vector<ConstDecl*> fields)
        :location_(location), fields(fields) {}
    ~ConstBlockDecl() {
        for (auto field : fields_)
            delete field;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
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
        :location_(location), formalParameterList_(formalParameterList),
        returnParameterList_(returnParameterList), functionBodyDecl_(functionBodyDecl) {}

    ~FunctionDecl() {
        if (formalParameterList_) delete formalParameterList_;
        if (returnParameterList_) delete returnParameterList_;
        if (functionBodyDecl_) delete functionBodyDecl_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    FormalParameterList* formalParameterList_;
    ReturnParameterList* returnParameterList_;
    FunctionBodyDecl* functionBodyDecl_;
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
        :location(location_), formalParameters_(params) {}
    ~FormalParameterList() {
        for (p : formalParameters_) delete p;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    std::vector<FormalParameter*> formalParameters_;
};

// formalParameter
// : IDENTIFIER ':' type
// ;
class FormalParameter : public Decl {
public:
    FormalParameter() = delete;
    explicit FormalParameter(const Location& location, Identifier* name, TypeDecl* typeDecl)
        :location_(location), name_(name), type_(typeDecl) {}
    ~FormalParameter() {
        if (name_) delete name_;
        if (type_) delete type_;
    }
    const Location Pos(); 
    const Location End(); 
    Location location_;
    Identifier* name_;
    TypeDecl* type_;
};













} // namespacde ast
} // namespace zl
