#include "parser.h"

namespace zl {

void  Parser::Build(std::vector<Node*>& decls) {
    ParseCompilationUnit(decls);
}

// The function check wether the next token is matched with specified
// token, report errors if not matched
Location Parser::Expect(Token::TokenType tokenType) {
    Location location = location_;
    if (token_.type_ != tokenType) {
        ErrorExpected(location, "'" + TokenTypeString(tokenType)+"'");
    }
    Next();
    return location;
}

// The function advance next token and update token variable
// Because of one-token look-ahead, print the previous token
// when tracing as it provides a more readable output. The
// very first token (!p.pos.IsValid()) is not initialized
// (it is token.ILLEGAL), so don't print it .
void Parser::Next() {
    Token token = lexer_.Next();
    location_ = token.location_;
    token_ = token;
    literal_ = token.assic_;
}


// The function lookheader to check wether next token match specified token
bool Parser::Match(Token::TokenType type) {
    return (token_.type_ == type);
}

// The function just output systax error messge to ErrorHandler
void Parser::SyntaxErrorAt(const Token& token, const std::string& msg) {}
void Parser::SyntaxErrorAt(const Location& location, const std::string& msg) {}
void Parser::SyntaxError(const std::string& msg) {}
void Parser::ErrorExpected(const Location& location, const std::string& msg) {
}

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
        if (Match(Token::PRIVATE) || Match(Token::PUBLIC))  {
            token = lexer_.Next();
        }
        if (ast::Decl* decl = ParseDeclaration(token); decl)
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
    bool publicity = (publicityToken.type_ == Token::PUBLIC);
    Token token = lexer_.Peek(); 
    ast::Decl* decl;

    switch (token.type_) {
        case Token::PACKAGE:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            decl = ParsePackageDeclaration();
            break;

        case Token::IMPORT:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            decl = ParseImportDeclaration();
            break;

        case Token::USING:
            if (publicityToken.Valid())
                SyntaxErrorAt(publicityToken.location_, "scope specifier not allowed here");
            decl = ParseUsingDeclaration();
            break;

        case Token::CONST:
            decl = ParseConstDeclaration();
            break;

        case Token::VAR:
            decl = ParseVarDeclaration();
            break;

        case Token::FUNC: 
            decl = ParseFunctionDeclaration();
            break;
 
        case Token::CLASS:
            decl = ParseClassDeclaration();
            break;

        case Token::INTERFACE:
            decl = ParseInterfaceDeclaration();
            break;

       default:
            SyntaxError("unknown declaration");
            decl = nullptr;
            break;
    }
    if (decl)
        decl->SetPublic(publicity);
    return decl;
}

// packageDeclaration
//    : 'package' IDENTIFIER 
ast::Decl* Parser::ParsePackageDeclaration() { 
    auto location = Expect(Token::PACKAGE);
    auto identifier = ParseIdentifier();
    return new ast::PackageDecl(location, identifier);
}

// importDeclaration
//    : 'import' qualifiedName 
//    ;
ast::Decl* Parser::ParseImportDeclaration() { 
    auto location = Expect(Token::PACKAGE);
    auto qualifiedName = ParseQualifiedName();
    return new ast::ImportDecl(location, qualifiedName);
}

// usingDeclaration
//    : 'using' qualifiedName '=' IDENTIFIER
//    ;
ast::Decl* Parser::ParseUsingDeclaration() { 
    auto location = Expect(Token::USING);
    auto qualifiedName = ParseQualifiedName();
    Expect(Token::ASSIGN);
    auto identifier = ParseIdentifier(); 
    return new ast::UsingDecl(location, qualifiedName, identifier);
}


// varDeclaration
//        : 'var' varBlockDeclaration | singleVarDeclaration
//        ;
ast::Decl* Parser::ParseVarDeclaration() {
    if (Match(Token::LPAREN))
        return ParseVarBlockDeclaration(); 
    else
        return ParseSingleVarDeclaration();
}

// varBlockDeclaration
//    : '(' singleVarDeclaration* ')'
//    ;
ast::Decl* Parser::ParseVarBlockDeclaration() {
    std::vector<ast::VariableDecl*> decls;
    auto location = Expect(Token::LPAREN);

    while (!Match(Token::RPAREN)) {
        auto varDecl = ParseSingleVarDeclaration();
        decls.push_back(varDecl);
    }
    Expect(Token::RPAREN);
    return new ast::VariableBlockDecl(location, decls);
}

// singleSingleVarDeclaration:
//    : IDENTIFIER ':' IDENTIFIER ('=' expression)?
//    ;
ast::VariableDecl* Parser::ParseSingleVarDeclaration() {
    auto location = location_;
    auto identifier = ParseIdentifier();
    Match(Token::ASSIGN);
    auto type = ParseType();
    auto expr = ParseVariableInitializer();
    return new ast::VariableDecl(location, identifier, type, expr);
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
ast::QualifiedName* Parser::ParseQualifiedNameList() {
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
ast::QualifiedName* Parser::ParseQualifiedName() {
    return nullptr;
}

// type
//    : primitiveType ('[' ']')*
//    | classType ('[' ']')*
//    | mapType
//    ;
ast::Type* Parser::ParseType() {
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

VarInitializer* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }







} // namespace zl
