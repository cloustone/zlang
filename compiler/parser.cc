#include "parser.h"

namespace zl {

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
void Parser::Advance(Node::Type nonterminal) { 
    if (nonterminal < Node::Type::MAX) {
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


/* ParseCompilationUnit will iterate all tokens to match declarations
compilationUnit
    : scopeModifier? declaration* EOF
    ;
*/
Node* Parser::ParseCompilationUnit() { 
    Node* root = new Node(Location());

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
    else if (lexer_.Match(Token::CONST))
        return ParseConstDeclaration();
    else if (lexer_.Match(Token::VAR))
        return ParseVarDeclaration();
    else 
        return ParseFunctionDeclaration();
}

// packageDeclaration
//    : 'package' qualifiedName  
Node* Parser::ParsePackageDeclaration() { 
    auto location = lexer_.Next().location_;

    if (Node* child = ParseQualifiedName(); child) 
        return new Node(Node::Type::PACKAGE, location, child);

    SyntaxErrorAt(location, "formal qualified name expected");
    Advance(Node::Type::PACKAGE);
    return nullptr; 
}

// importDeclaration
//    : 'import' qualifiedName 
//    ;
Node* Parser::ParseImportDeclaration() { 
    auto location = lexer_.Next().location_; // skip 'package' token

    if (Node* child = ParseQualifiedName(); child) 
        return new Node(Node::Type::IMPORT, location, child);

    SyntaxErrorAt(location, "formal qualified name expected");
    Advance(Node::Type::IMPORT);
    return nullptr; 
}

// usingDeclaration
//    : 'using' qualifiedName '=' IDENTIFIER
//    ;
Node* Parser::ParseUsingDeclaration() { 
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
    return new Node(Node::Type::USING, location, qualifiedName);

failed:
    Advance(Node::Type::USING);
    if (qualifiedName)
        delete qualifiedName;
    return nullptr;
}


// varDeclaration
//        : 'var' varBlockDeclaration | singleVarDeclaration
//        ;
Node* Parser::ParseVarDeclaration() {
    if (lexer_.Match('('))
        return ParseVarBlockDeclaration(); 
    else
        return ParseSingleVarDeclaration();
}

// varBlockDeclaration
//    : '(' singleVarDeclaration* ')'
//    ;
Node* Parser::ParseVarBlockDeclaration() {
    auto location = lexer_.Next().location_; // skip '(' token
    std::vector<Node*> nodes;

    while (!lexer_.Match(')')) {
        if (Node* node = ParseSingleVarDeclaration(); node)
            nodes.push_back(node);
        else {
            SyntaxError("illegal variable declaration");
            for (auto node : nodes) delete node;
            Advance(Node::Type::VAR_BLOCK);
            return nullptr;
        }
    }
    auto varBlockDeclNode = new Node(Node::Type::VAR_BLOCK, location);
    for (auto node : nodes) 
        varBlockDeclNode->AddChild(node);
    return varBlockDeclNode;
    
}

// singleSingleVarDeclaration:
//    : IDENTIFIER ':' IDENTIFIER ('=' expression)?
//    ;
Node* Parser::ParseSingleVarDeclaration() {
    auto location = lexer_.Next().location_; 
    Token varNameToken;

    if (!lexer_.Match(Token::ID, &varNameToken)) {
        SyntaxError("variable ename unexpected");
        Advance(Node::Type::VAR);
        return nullptr;
    }
    if (!lexer_.Match(Token::ASSIGN)) {
        SyntaxError("variable declaration miss '=' symbol");
        Advance(Node::Type::VAR);
        return nullptr;
    }
    
    Node* initializationExpr = ParseExpr();
    if (!initializationExpr) {
        Advance(Node::Type::VAR);
        return nullptr;
    }
    return new Node(Node::Type::VAR, location,
            new Node(Node::Type::ID, varNameToken),
            new Node(Node::Type::EXPR, location, initializationExpr));
}

// constDeclaraton
//    : 'const' constBlockDeclaration | singleConstDeclaration
//    ;
Node* Parser::ParseConstDeclaration() {
    lexer_.Next(); 

    if (lexer_.Match('('))
        return ParseConstBlockDeclaration();
    else 
        return ParseSingleVarDeclaration();
}

// constBlockDeclaration
//    : '(' singleConstDeclaration* ')'
//    ;
Node* Parser::ParseConstBlockDeclaration() {
    auto location = lexer_.Next().location_;  // skip '('
    std::vector<Node*> declarations;

    while (!lexer_.Match(')')) {
        if (Node* node = ParseSingleConstDeclaration(); node)
            declarations.push_back(node);
        else  {
            Advance(Node::Type::CONST_BLOCK);
            break;
        }
    }
    if (!declarations.empty()) {
        auto constBlockDeclNode = new Node(Node::Type::CONST_BLOCK, location);
        for (auto decl: declarations) 
            constBlockDeclNode->AddChild(decl);
        return constBlockDeclNode;
    }
    SyntaxErrorAt(location, "no valid const declaration found");
    return nullptr;
}

// singleConstDeclaration
//    : IDENTIFIER (':' IDENTIFIER)? ('=' constExpression)?
//    ;
Node* Parser::ParseSingleConstDeclaration() {
    Token nameToken, typeToken;
    auto location = lexer_.GetLocation();

    if (!lexer_.Match(Token::ID, &nameToken)) {
        SyntaxError("expected const identifier token missed");
        Advance(Node::Type::VAR);
        return nullptr;
    }

    // Const identifier have a type constraint
    if (lexer_.Match(':')) {
        lexer_.Next();
         if (!lexer_.Match(Token::ID, &typeToken)) {
            SyntaxError("no type identifier found in const declaration");
            Advance(Node::Type::VAR);
            return nullptr;
        }
    }

    Node* constExpr = nullptr;
    if (lexer_.Match(Token::ASSIGN)) {
        lexer_.Next();
        if (constExpr = ParseConstExpr(); !constExpr) {
            SyntaxError("wrong const expression found");
            Advance(Node::Type::VAR);
            return nullptr;
        }
    }
    Node* varDeclaration = new Node(Node::Type::VAR, location, nameToken); 
    if (nameToken.Valid())
        varDeclaration->AddChild(nameToken);
    if (typeToken.Valid())
        varDeclaration->AddChild(typeToken);
    if (constExpr)
        varDeclaration->AddChild(constExpr);

    return varDeclaration;
}



Node* Parser::ParseFunctionDeclaration(){ return nullptr; }
Node* Parser::ParseInterfaceDeclaration(){ return nullptr; }
Node* Parser::ParseClassDeclaration(){ return nullptr; }

Node* Parser::ParseClassBodyDeclaration(){ return nullptr; }

Node* Parser::ParseMemberDeclaration(){ return nullptr; }

Node* Parser::ParseMethodDeclaration(){ return nullptr; }

Node* Parser::ParseVariableDeclaration(){ return nullptr; }

Node* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }
Node* Parser::ParseQualifiedName(){ return nullptr; }







} // namespace zl
