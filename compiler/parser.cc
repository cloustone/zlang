#include "parser.h"

namespace zl {

Node* Parser::Build() {
    return ParseCompilationUnit();
}

// The function just output systax error messge to ErrorHandler
void Parser::SyntaxErrorAt(const Token& token, const std::string& msg) {}
void Parser::SyntaxErrorAt(const Location& location, const std::string& msg) {}
void Parser::SyntaxError(const std::string& msg) {}

// The function will advance token until one of followset found
// The followset is the list of valid tokens that can follow a production,
// if it is empty, exactly one (non-EOF) token is consumed to ensure progress.
void Parser::Advance(int followset[]) {}
void Parser::Advance(const std::vector<int>& followset) {}


/* ParseCompilationUnit will iterate all tokens to match declarations
compilationUnit
    : scopeModifier? declaration* EOF
    ;
*/
Node* Parser::ParseCompilationUnit() { 
    Node* root = new Node();

    while (!lexer_.Eof()) {
        bool publicity = true;
        if (lexer_.Match(Token::PRIVATE) || lexer_.Match(Token::PUBLIC))  {
            publicity = lexer_.Match(Token::PUBLIC);
            lexer_.Next();
        }
        Node* node= ParseDeclaration();
        if (node) {
            root->AddChild(node);
            node->SetPublic(publicity);
        }
    }
    return root; 
}
/* Declaration nonterminal parser function 
declaration
    : packageDeclaration 
    | importDeclaration 
    | usingDeclaration
    | constDeclaration
    | varDeclaration
    | functionDeclaration
    | classDeclaration
    | interfaceDeclaration
    ;
*/
Node* Parser::ParseDeclaration() { 
    if (lexer_.Match(Token::PACKAGE))
        return ParsePackageDeclaration();
    else if (lexer_.Match(Token::IMPORT))
        return ParseImportDeclaration();
    else if (lexer_.Match(Token::USING))
        return ParseUsingDeclaration();
    else if (lexer_.Match(Token::CLASS))
        return ParseClassDeclaration();
    else if (lexer_.Match(Token::INTERFACE))
        return ParseInterfaceDeclaration();
    else 
        return ParseFunctionDeclaration();
}

// packageDeclaration
//    : 'package' qualifiedName  
Node* Parser::ParsePackageDeclaration() { 
    static int followSet[] = {
        Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,
    };
    auto location = lexer_.Next().location_;

    if (Node* child = ParseQualifiedName(); child) 
        return new PackageDeclNode(location, child);

    SyntaxErrorAt(location, "formal qualified name expected");
    Advance(followSet);
    return nullptr; 
}

/*
importDeclaration
    : 'import' qualifiedName 
    ;
*/
Node* Parser::ParseImportDeclaration() { 
    static int followSet[] = {
        Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,
    };
    auto location = lexer_.Next().location_; // skip 'package' token

    if (Node* child = ParseQualifiedName(); child) 
        return new PackageDeclNode(location, child);

    SyntaxErrorAt(location, "formal qualified name expected");
    Advance(followSet);
    return nullptr; 
}

Node* Parser::ParseUsingDeclaration() { 
    static int followSet[] = {
        Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,
    };
    auto location = lexer_.Next().location_; // skip 'using' token
    Node* qualifiedName;
    Token identifier;

    if (qualifiedName = ParseQualifiedName(); !qualifiedName)  {
        SyntaxError("formal qualifed named unexpected");
        goto failed;
    }
    if (!lexer_.Match(Token::ASSIGN)) {
        SyntaxError("missing '=' in using statement");
        goto failed;
    }
    if (!lexer_.Match(Token::ID, &identifier)) {
        SyntaxError("missing target name in using statement");
        goto failed;
    }
    return new UsingDeclNode(location, qualifiedName, identifier);

failed:
    Advance(followSet);
    if (qualifiedName)
        delete qualifiedName;
    return nullptr;
}


// varDeclaration
//        : 'var' varBlockDeclaration | singleVarDeclaration
//        ;
Node* Parser::ParseVarDeclarartion() {
    if (lexer_.Match('('))
        return ParseVarBlockDeclaration(); 
    else
        return ParseSingleVarDeclaration();
}

// varBlockDeclaration
//    : '(' singleVarDeclaration* ')'
//    ;
Node* Parser::ParseVarBlockDeclaration() {
    static int followSet[] = {
        Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,
    };
    auto location = lexer_.Next().location_; // skip '(' token
    std::vector<Node*> nodes;

    while (!lexer_.Match(')')) {
        if (Node* node = ParseSingleVarDeclaration(); node)
            nodes.push_back(node);
        else {
            SyntaxError("illegal variable declaration");
            for (auto node : nodes) delete node;
            Advance(followSet);
            return nullptr;
        }
    }
    return new VarBlockDeclNode(location, nodes);
}

// singleSingleVarDeclaration:
//    : IDENTIFIER ':' IDENTIFIER ('=' expression)?
//    ;
Node* Parser::ParseSingleVarDeclaration() {
    return nullptr;
}



Node* Parser::ParseFunctionDeclaration(){ return nullptr; }
Node* Parser::ParseInterfaceDeclaration(){ return nullptr; }
Node* Parser::ParseClassDeclaration(){ return nullptr; }

Node* Parser::ParseConstDeclaration(){ return nullptr; }

Node* Parser::ParseClassBodyDeclaration(){ return nullptr; }

Node* Parser::ParseMemberDeclaration(){ return nullptr; }

Node* Parser::ParseMethodDeclaration(){ return nullptr; }

Node* Parser::ParseVariableDeclaration(){ return nullptr; }

Node* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }
Node* Parser::ParseQualifiedName(){ return nullptr; }







} // namespace zl
