#include "parser.h"

namespace zl {

enum {
    ROOT = 0,
    PACKAGE,
    IMPORT,
    USING,
    VAR_BLOCK,
    VAR,
    ID,
    EXPR,
    CONST_EXPR,
    CONST,
    CONST_BLOCK,
    FUNC,
    FUNC_PARAM,
    FUNC_BLOCK,
    FUNC_RET_PARAM,
    MAX,
};
#define MAX_FOLLOWS 20
const int FollowSets[][MAX_FOLLOWS] = {
    // ROOT,
    // PACKAGE,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // IMPORT,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // USING,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // VAR_BLOCK,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // VAR,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // ID,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // EXPR,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // CONST_EXPR,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // CONST,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // CONST_BLOCK,
    {Token::IMPORT, Token::USING, Token::PACKAGE, Token::CLASS, Token::INTERFACE, Token::FUNCTION,},
    // MAX,
    {}
};

void  Parser::Build(std::vector<Node*>& decls) {
    ParseCompilationUnit(decls);
}

// The function just output systax error messge to ErrorHandler
void Parser::SyntaxErrorAt(const Token& token, const std::string& msg) {}
void Parser::SyntaxErrorAt(const Location& location, const std::string& msg) {}
void Parser::SyntaxError(const std::string& msg) {}

// The function will advance token until one of followset found
// The followset is the list of valid tokens that can follow a production,
// if it is empty, exactly one (non-EOF) token is consumed to ensure progress.
void Parser::Advance(int nonterminal) { 
    if (nonterminal < (int)MAX) {
        auto follows = FollowSets[(int)nonterminal];
        while (!lexer_.Eof()) {
            // Peek a token and check wethere the token is in followset
            Token token = lexer_.Peek();
            for (int index = 0; index < MAX_FOLLOWS; index++) {
                if (follows[index] == token.type_)
                    return;
            }
            lexer_.Next();
        }
    }
}


// ParseCompilationUnit will iterate all tokens to match declarations
// compilationUnit
//    : scopeModifier? declaration* EOF
//    ;
void Parser::ParseCompilationUnit(std::vector<Node*>& decls) { 
    while (!lexer_.Eof()) {
        bool publicity = true;
        if (lexer_.Match(Token::PRIVATE) || lexer_.Match(Token::PUBLIC))  {
            publicity = lexer_.Match(Token::PUBLIC);
            lexer_.Next();
        }
        ast::Decl* decl= ParseDeclaration();
        if (decl) {
            decl->SetPublic(publicity);
            decls.push_back(decl);
        }
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
ast::Decl* Parser::ParseDeclaration() { 
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
    else if (lexer_.Match(Token::CONST))
        return ParseConstDeclaration();
    else if (lexer_.Match(Token::VAR))
        return ParseVarDeclaration();
    else 
        return ParseFunctionDeclaration();
}

// packageDeclaration
//    : 'package' IDENTIFIER 
ast::Decl* Parser::ParsePackageDeclaration() { 
    auto location = lexer_.Next().location_;
    Token token; 

    if (!lexer_.Match(Token::ID, &token)) {
        SyntaxError("unknown package name");
        Advance(PACKAGE);
        return nullptr;
    }
    return new ast::PackageDecl(location, new ast::Identifier(token));
}

// importDeclaration
//    : 'import' qualifiedName 
//    ;
ast::Decl* Parser::ParseImportDeclaration() { 
    auto location = lexer_.Next().location_; // skip 'package' token

    QualifiedName* qualifiedName = (QualifiedName*)ParseQualifiedName();
    if (!qualifiedName) {
        SyntaxError("formal qualified name expected");
        Advance(IMPORT);
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
    auto location = lexer_.Next().location_; // skip 'using' token

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
    Advance(USING);
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
