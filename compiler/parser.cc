#include "parser.h"

namespace zl {

void  Parser::Build(std::vector<Node*>& decls) {
    ParseCompilationUnit(decls);
}

// The function just output systax error messge to ErrorHandler
void Parser::SyntaxErrorAt(const Token& token, const std::string& msg) {}
void Parser::SyntaxErrorAt(const Location& location, const std::string& msg) {}
void Parser::SyntaxError(const std::string& msg) {}

// SyncStmt advances to the next statement, Used for synchronization after an error.
void Parser::SyncStmt() {
}
// SyncDecl advances to the next declaration. Used for synchronization after an error.
void Parser::SyncDecl() {
}

// ParseCompilationUnit will iterate all tokens to match declarations
// compilationUnit
//    : scopeModifier? declaration* EOF
//    ;
void Parser::ParseCompilationUnit(std::vector<Node*>& decls) { 
    while (!lexer_.Eof()) {
        Token token;
        if (lexer_.Match(Token::PRIVATE) || lexer_.Match(Token::PUBLIC))  {
            token = lexer_.Next();
        }
        ast::Decl* decl = ParseDeclaration(token);
        if (decl) 
            decls.push_back(decl);
    }
}

// Declaration nonterminal parser function 
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
ast::Decl* Parser::ParseDeclaration(const Token& publicityToken) { 
    Token token = lexer_.Next(); 

    switch (token.type_) {
        case Token::PACKAGE:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            return ParsePackageDeclaration();

        case Token::IMPORT:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            return ParseImportDeclaration();

        case Token::USING:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            return ParseUsingDeclaration();

        case Token::CLASS:
            return ParseClassDeclaration();
        case Token::INTERFACE:
            return ParseInterfaceDeclaration();
        case Token::CONST:
            return ParseConstDeclaration();
        case Token::VAR:
            return ParseVarDeclaration();
        case Token::FUNC: 
            return ParseFunctionDeclaration();
        default:
            SyntaxError("unknown declaration");
            return nullptr;
    }
}

// packageDeclaration
//    : 'package' IDENTIFIER 
ast::Decl* Parser::ParsePackageDeclaration() { 
    auto location = lexer_.GetLocation();
    Token token; 

    if (!lexer_.Match(Token::ID, &token)) {
        SyntaxError("unknown package name");
        SyncDecl();
        return nullptr;
    }
    return new ast::PackageDecl(location, new ast::Identifier(token));
}

// importDeclaration
//    : 'import' qualifiedName 
//    ;
ast::Decl* Parser::ParseImportDeclaration() { 
    auto location = lexer_.GetLocation(); 

    QualifiedName* qualifiedName = (QualifiedName*)ParseQualifiedName();
    if (!qualifiedName) {
        SyntaxError("formal qualified name expected");
        SyncDecl();
        return nullptr; 
    }
    return new ast::ImportDecl(location, qualifiedName);
}

// usingDeclaration
//    : 'using' qualifiedName '=' IDENTIFIER
//    ;
ast::Decl* Parser::ParseUsingDeclaration() { 
    ast::QualifiedName* qualifiedName;
    Token token;
    auto location = lexer_.GetLocation();

    if (qualifiedName = (ast::QualifiedName*)ParseQualifiedName(); !qualifiedName)  {
        SyntaxError("formal qualifed named unexpected");
        goto failed;
    }
    if (!lexer_.Match(Token::ASSIGN)) {
        SyntaxError("missing '=' in using statement");
        goto failed;
    }
    if (!lexer_.Match(Token::ID, &token)) {
        SyntaxError("missing target name in using statement");
        goto failed;
    }
    return new ast::UsingDecl(location, qualifiedName, new ast::Identifier(token));

failed:
    SyncDecl();
    if (qualifiedName)
        delete qualifiedName;
    return nullptr;
}


// varDeclaration
//        : 'var' varBlockDeclaration | singleVarDeclaration
//        ;
ast::Decl* Parser::ParseVarDeclaration() {
    if (lexer_.Match('('))
        return ParseVarBlockDeclaration(); 
    else
        return ParseSingleVarDeclaration();
}
// varBlockDeclaration
//    : '(' singleVarDeclaration* ')'
//    ;
ast::Decl* Parser::ParseVarBlockDeclaration() {
/*
    auto location = lexer_.Next().location_; // skip '(' token
    std::vector<Node*> nodes;

    while (!lexer_.Match(')')) {
        if (Node* node = ParseSingleVarDeclaration(); node)
            nodes.push_back(node);
        else {
            SyntaxError("illegal variable declaration");
            for (auto node : nodes) delete node;
            Advance(VAR_BLOCK);
            return nullptr;
        }
    }
    return  new Node(VAR_BLOCK, location, nodes);
*/
    return nullptr;
}

// singleSingleVarDeclaration:
//    : IDENTIFIER ':' IDENTIFIER ('=' expression)?
//    ;
ast::Decl* Parser::ParseSingleVarDeclaration() {
/*
    auto location = lexer_.Next().location_; 
    Token varNameToken;

    if (!lexer_.Match(Token::ID, &varNameToken)) {
        SyntaxError("variable ename unexpected");
        Advance(VAR);
        return nullptr;
    }
    if (!lexer_.Match(Token::ASSIGN)) {
        SyntaxError("variable declaration miss '=' symbol");
        Advance(VAR);
        return nullptr;
    }
    
    Node* initializationExpr = ParseExpr();
    if (!initializationExpr) {
        Advance(VAR);
        return nullptr;
    }
    return new Node(VAR, location,
            new Node(ID, varNameToken),
            new Node(EXPR, location, initializationExpr));
*/
    return nullptr;
}


// constDeclaraton
//    : 'const' constBlockDeclaration | singleConstDeclaration
//    ;
ast::Decl* Parser::ParseConstDeclaration() {
    lexer_.Next(); 

    if (lexer_.Match('('))
        return ParseConstBlockDeclaration();
    else 
        return ParseSingleVarDeclaration();
}

// constBlockDeclaration
//    : '(' singleConstDeclaration* ')'
//    ;
ast::Decl* Parser::ParseConstBlockDeclaration() {
/*
    auto location = lexer_.Next().location_;  // skip '('
    std::vector<Node*> declarations;

    while (!lexer_.Match(')')) {
        if (Node* node = ParseSingleConstDeclaration(); node)
            declarations.push_back(node);
        else  {
            Advance(CONST_BLOCK);
            break;
        }
    }
    if (!declarations.empty()) 
        return  new Node(CONST_BLOCK, location, declarations);
    SyntaxErrorAt(location, "no valid const declaration found");
*/
    return nullptr;
}

// singleConstDeclaration
//    : IDENTIFIER (':' IDENTIFIER)? ('=' variablieInitialization)?
//    ;
ast::Decl* Parser::ParseSingleConstDeclaration() {
    /*
    Token nameToken, typeToken;
    auto location = lexer_.GetLocation();

    if (!lexer_.Match(Token::ID, &nameToken)) {
        SyntaxError("expected const identifier token missed");
        Advance(VAR);
    }

    // Const identifier have a type constraint
    if (lexer_.Match(':')) {
        lexer_.Next();
         if (!lexer_.Match(Token::ID, &typeToken)) {
            Advance(VAR);
        }
    }

    Node* variableInitializer = nullptr;
    if (lexer_.Match(Token::ASSIGN)) {
        lexer_.Next();
        if (variableInitializer = ParseVariableInitializer(); !variableInitializer) {
            SyntaxError("wrong const expression found");
        }
    }
    Node* varDeclaration = new Node(VAR, location, nameToken, typeToken); 
    varDeclaration->Add(variableInitializer);

    return varDeclaration;
*/
    return nullptr;
}

// functionDeclaration
//    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
//    ;
ast::Decl* Parser::ParseFunctionDeclaration() {
/*
    auto location = lexer_.Next().location_;  // skip 'func'
    Token funcNameToken;

    if (!lexer_.Match(Token::ID, &funcNameToken)) {
        SyntaxError("function name missed");
        Advance(FUNC);
        // Parser continue to parser the left declaration without return
        // nullptr
    }
    Node* formalParameters = ParseFormalParameters();
    Node* returnParameters = nullptr;
    if (lexer_.Match(':')) {
        lexer_.Next();
        returnParameters = ParseFunctionReturnParameters();
    }
    Node* bodyDeclaration = ParseFunctionBodyDeclaration();
    return new Node(FUNC, location, formalParameters, returnParameters, bodyDeclaration);
*/
    return nullptr;
}

// formalParameters
//    : '(' formalParameterList ? ')'
//    ;
Node* Parser::ParseFormalParameters() {
    return nullptr;
}

// qualifiedNameList
//    : qualifiedName (',' qualifiedName)*
//    ;
Node* Parser::ParseQualifiedNameList() {
    return nullptr;
}

// formalParameterList
// : formalParameter (',' formalParameter)*
// ;
Node* Parser::ParseFormalParameterList() {
    return nullptr;
}

// formalParameter
// : IDENTIFIER ':' type
// ;
Node* Parser::ParseFormalParameter() {
    return nullptr;
}

// functionReturnParameters
//    : ('void' | type) | ('(' typeList ')')
//    ;
Node* Parser::ParseFunctionReturnParameters() {
    return nullptr;
}

// functionBodyDeclaration
//    : block
//    ;
Node* Parser::ParseFunctionBodyDeclaration() {
    return nullptr;
}

// qualifiedName
//    : IDENTIFIER ('.' IDENTIFIER)*
//    ;
Node* Parser::ParseQualifiedName() {
    return nullptr;
}

// type
//    : primitiveType ('[' ']')*
//    | classType ('[' ']')*
//    | mapType
//    ;
Node* Parser::ParseTypeDeclaration() {
    return nullptr;
}

// typeList
//    : type (',' type)*
//    ;
Node* Parser::ParseTypeList() {
    return nullptr;
}

// classType
//   : qualifiedName
//    ;
Node* Parser::ParseClassType() {
    return nullptr;
}

// mapType
//    : 'map' '<' mapItemType ','  mapItemType '>' 
//    ;
Node* Parser::ParseMapType() {
    return nullptr;
}

// mapItemType
//    : primitiveType
//    | classType
//    ;
Node* Parser::ParseMapItemType() {
    return nullptr;
}

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
Node* Parser::ParsePrimitiveType() {
    return nullptr;
}



Decl* Parser::ParseInterfaceDeclaration(){ return nullptr; }
Decl* Parser::ParseClassDeclaration(){ return nullptr; }

Decl* Parser::ParseClassBodyDeclaration(){ return nullptr; }

Decl* Parser::ParseMemberDeclaration(){ return nullptr; }

Decl* Parser::ParseMethodDeclaration(){ return nullptr; }

Node* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }







} // namespace zl
