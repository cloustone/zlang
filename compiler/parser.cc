#include "parser.h"

namespace zl {

Node* Parser::Build() {
    return ParseCompilationUnit();
}

// ParseCompilationUnit will iterate all tokens to match declarations
Node* Parser::ParseCompilationUnit() { 
    Node* root = new Node();
    Node* child;

    while ((child = ParseDeclaration()) != nullptr)
        root->AddChild(child);
    return root; 
}

Node* Parser::ParseDeclaration() { 
    if (lexer_.Match(Token::PACKAGE))
        return ParsePackageDeclaration();
    else if (lexer_.Match(Token::IMPORT))
        return ParseImportDeclaration();
    else 
        return ParseTypeDeclaration();
}

// packageDeclaration
//    : 'package' qualifiedName  
Node* Parser::ParsePackageDeclaration() { 
    lexer_.Next(); // skip 'package' token
    Node* child = ParseQualifiedName();
    if (child) {
        Node* node = new PackageDeclNode();
        node->AddChild(child);
        return node;
    }
    return nullptr; 
}

Node* Parser::ParseImportDeclaration() { 
    return nullptr; 
}

Node* Parser::ParseTypeDeclaration() { 
    return nullptr; 
}

Node* Parser::ParseClassDeclaration(){ return nullptr; }

Node* Parser::ParseEnumDeclaration(){ return nullptr; }

Node* Parser::ParseConstDeclaration(){ return nullptr; }

Node* Parser::ParseModifier(){ return nullptr; }

Node* Parser::ParseClassBodyDeclaration(){ return nullptr; }

Node* Parser::ParseMemberDeclaration(){ return nullptr; }

Node* Parser::ParseMethodDeclaration(){ return nullptr; }

Node* Parser::ParseVariableDeclaration(){ return nullptr; }

Node* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }
Node* Parser::ParseQualifiedName(){ return nullptr; }







} // namespace zl
