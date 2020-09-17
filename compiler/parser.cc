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
            decl = ParseInterfaceDecl();
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
    if (Match(Token::LPAREN))
        return ParseConstBlockDeclaration();
    else 
        return ParseSingleVarDeclaration();
}

// constBlockDeclaration
//    : '(' singleConstDeclaration* ')'
//    ;
ast::ConstBlockDecl* Parser::ParseConstBlockDeclaration() {
    std::vector<ast::ConstDecl*> declarations;

    auto location = Expect(Token::LPAREN);
    while (!Match(Token::RPAREN)) {
        if (auto decl  = ParseSingleConstDeclaration(); decl)
            declarations.push_back(decl);
    }
    Expect(Token::RPAREN);
    return new ast::ConstBlockDecl(location, declarations);
}

// singleConstDeclaration
//    : IDENTIFIER (':' type)? ('=' variablieInitialization)?
//    ;
ast::ConstDecl* Parser::ParseSingleConstDeclaration() {
    auto location = location_;
    ast::Type* type = nullptr;
    ast::VarInitializer* varInitializer = nullptr;
    auto nameId = ParseIdentifier();

    if (Match(Token::COLON)) {
        Next();
        type = ParseType();
    }
    if (Match(Token::ASSIGN)) {
        Next();
        varInitializer = ParseVariableInitializer();
    }
    return new ast::ConstDecl(location, nameId, type, varInitializer); 
}

// functionDeclaration
//    : 'func' IDENTIFIER formalParameters (':' functionReturnParameters)?  functionBodyDeclaration
//    ;
ast::FunctionDecl* Parser::ParseFunctionDeclaration() {
    auto location = location_;  
    auto nameId = ParseIdentifier();
    auto formalParameters = ParseFormalParameters();
    ast::ReturnParameterList* returnParams = nullptr;

    if (Match(Token::COLON)) {
        Next();
        returnParams = ParseFunctionReturnParameters();
    }
    auto functionBlockDecl = ParseFunctionBlockDeclaration();

    return new ast::FunctionDecl(location, nameId, formalParameters, 
            returnParams, functionBlockDecl);
}

// formalParameters
//    : '(' formalParameterList ? ')'
//    ;
ast::FormalParameterList* Parser::ParseFormalParameters() {
    Expect(Token::LPAREN);
    auto formalParameterList = ParseFormalParameterList();
    Expect(Token::RPAREN);
    return formalParameterList;
}

// formalParameterList
// : formalParameter (',' formalParameter)*
// ;
ast::FormalParameterList* Parser::ParseFormalParameterList() {
    auto location = location_;
    std::vector<ast::FormalParameter*> parameterList;
    parameterList.push_back(ParseFormalParameter());

    while (Match(Token::COMMA)) {
        Next();
        parameterList.push_back(ParseFormalParameter());
    }
    return new ast::FormalParameterList(location, parameterList); 
}

// formalParameter
// : IDENTIFIER ':' type
// ;
ast::FormalParameter* Parser::ParseFormalParameter() {
    auto location = location_;
    auto identifier = ParseIdentifier();
    Expect(Token::COLON);
    auto type = ParseType();
    return new ast::FormalParameter(location, identifier, type);
}

// functionReturnParameters
//    : ('void' | type) | ('(' typeList ')')
//    ;
ast::ReturnParameterList* Parser::ParseFunctionReturnParameters() {
    auto location = location_;
    std::vector<ast::Type*> types;
    
    if (Match(Token::LPAREN) ) {
        Next();
        types = ParseTypeList();
        Expect(Token::RPAREN);
    } else if (Match(Token::VOID)) {
        // Do nothing for void type
    } else if (Match(Token::LBRACE)) {
        // Do nothing for void type
    } else {
        ast::Type* type = ParseType();
        types.push_back(type);
    }

    return new ast::ReturnParameterList(location, types);
}

// functionBlockDeclaration
//    : block
//    ;
ast::FunctionBlockDecl* Parser::ParseFunctionBlockDeclaration() {
    auto location = location_;
    std::vector<ast::Node*> nodes;

    Expect(Token::LBRACE);
    while (!Match(Token::RBRACE)) {
        Next();
        if (token_.type_ == Token::VAR) 
            nodes.push_back(ParseVarDeclaration());
        else 
            nodes.push_back(ParseStmt());
    }
    Expect(Token::RBRACE);
    return new ast::FunctionBlockDecl(location);
}

// qualifiedName
//    : IDENTIFIER ('.' IDENTIFIER)*
//    ;
ast::QualifiedName* Parser::ParseQualifiedName() {
    std::vector<std::string> names;
    
    auto location = Expect(Token::ID);
    names.push_back(literal_);
    while (Match(Token::PERIOD)) {
        Next();
        Expect(Token::ID);
        names.push_back(literal_);
    }
    return new ast::QualifiedName(location, names);
}

// qualifiedNameList
//    : qualifiedName (',' qualifiedName)*
//    ;
ast::QualifiedNameList* Parser::ParseQualifiedNameList() {
    auto location = location_;
    std::vector<ast::QualifiedName*> qualifiedNames;
    qualifiedNames.push_back(ParseQualifiedName());

    while (Match(Token::COMMA)) {
        Next();
        qualifiedNames.push_back(ParseQualifiedName());
    }
    return new ast::QualifiedNameList(location, qualifiedNames);
}

// interfaceMethodDecl
//    : IDENTIFIER formalParameters (':' (type | 'void'))? ('throw' qualifiedNameList)?
// 
ast::InterfaceMethodDecl* Parser::ParseInterfaceMethod() {
    auto location = location_;  
    auto nameId = ParseIdentifier();
    auto formalParameters = ParseFormalParameters();
    ast::ReturnParameterList* returnParams = nullptr;

    if (Match(Token::COLON)) {
        Next();
        returnParams = ParseFunctionReturnParameters();
    }
    return new ast::InterfaceMethodDecl(location, nameId, formalParameters, returnParams);
}

// interfaceDeclaration
//    : 'interface' '{' interfaceMethodDecl* '}'
//    ;
ast::InterfaceDecl* Parser::ParseInterfaceDecl() {
    auto location = location_;
    auto methodName = ParseIdentifier();
    std::vector<ast::InterfaceMethodDecl*> methods;

    Expect(Token::LBRACE);
    while (!Match(Token::RBRACE)) {
        if (auto method  = ParseInterfaceMethod(); method)
            methods.push_back(method);
    }
    Expect(Token::RBRACE);
    return new ast::InterfaceDecl(location, methodName, methods);
}

// classDeclaration
//    : 'class' IDENTIFIER ('implements' qualifiedNameList)? 
//     '{' classBodyDeclaration* '}'
//    ;
ast::ClassDecl* Parser::ParseClassDeclaration() {
    auto location = location_;
    auto className = ParseIdentifier();
    QualifiedNameList* interfaceList = nullptr;
    
    if (Match(Token::IMPLEMENTS)) {
        Next();
        interfaceList = ParseQualifiedNameList();
    }
    Expect(Token::LBRACE);
    auto classBodyDecl = ParseClassBody();
    Expect(Token::RBRACE);
    return new ast::ClassDecl(location, className, interfaceList, classBodyDecl);
}

// classBodyDeclaration
//    : (classSectionSpecifier)? classMethodDeclaration |classVariableDeclaration 
//    ;
ast::ClassBodyDecl* Parser::ParseClassBody() {
    auto location = location_;
    std::vector<ast::VariableDecl*> variables;
    std::vector<ast::FunctionDecl*> functions;
    bool publicity = false;

    while (!Match(Token::RBRACE)) {
        if (token_.type_ == Token::PRIVATE || token_.type_ == Token::PUBLIC) {
            publicity = (token_.type_ == Token::PUBLIC);
            Next();
            Expect(Token::COLON);
            continue;
        }
        // method and varaible declaration both begin with identifier
        Expect(Token::ID);
        if (Match(Token::LPAREN)) { // method declaration
            Back();
            functions.push_back(ParseFunctionDeclaration());
        } else { // variable declaration 
            Back();
            variables.push_back((ast::VariableDecl*)ParseVarDeclaration());
        }
    }
    Expect(Token::RBRACE);
    return new ast::ClassBodyDecl(location, variables, functions);
}

// statements
//    : statement*
//    ;
std::vector<ast::Stmt*> Parser::ParseStatements() {
    std::vector<ast::Stmt*> stmts;
    while (ast::Stmt* stmt = ParseStatement())
        stmts.push_back(stmt);
    return stmts;
}

// statement
//     : ifStatement
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
//     | localVariableDeclarationStatement
//     ;
ast::Stmt* Parser::ParseStatement() {
    Next();
    switch (token_.type_) {
        case Token::IF:
            return ParseIfStatement();
        case Token::FOR:
            return ParseForStatement();
        case Token::FOREACH:
            return ParseForeachStatement();
        case Token::ID:
            if (Match(Token::COLON))
                return ParseLabelStatement();
            else 
                return ParseExprStatement();
        default:
            break;
    }
    SyntaxError("unkown statement");
    SyncStmt();
    return new UnknownStmt(location_);
}

// labelStatement
//    : IDENTIFIER ':' 
//    ;
ast::Stmt* Parser::ParseLabelStatement() {
    auto location = location_;
    const std::string label = token_.assic_;
    Expect(Token::COLON);
    return new ast::LabelStmt(location, label);
}

// blockStatement
//    : '{' statements '}'
//    ;
ast::Stmt* Parser::ParseBlockStatement() {
    return nullptr;
}

// ifStatement
//    : 'if' '(' expression ')' statementBlock ('elif' statementBlock)* ('else' statementBlock)?
//    ;
ast::Stmt* Parser::ParseIfStatement() {
    auto location = location_;

    Match(Token::LBRACE);
    auto conditionExpr = ParseExpr();
    auto ifBlockStmt = ParseBlockStatement();

    std::vector<ast::Stmt*> elifStmts;
    while (Match(Token::ELIF)) {
        Next();
        auto stmt = ParseBlockStatement();
        elifStmts.push_back(stmt);
    }
    ast::Stmt* finalStmt = nullptr;
    if (Match(Token::ELSE)) {
        Next();
        finalStmt = ParseBlockStatement();
    }
    return new ast::IfStmt(location, conditionExpr, ifBlockStmt, elifStmts, finalStmt);
}

// forStatement
//    : 'for' 
//      '('forInitializer?  ';'expression? ';' expressionList? ')'  
//      statement
//   ;
ast::Stmt* Parser::ParseForStatement() {
    return nullptr;
}

// forInitializer
//    : variableDeclaration
//    | expressionList
//    ;
ast::Stmt* Parser::ParseForInitializer() {
    return nullptr;
}

// foreachStatement
//    : 'foreach' '(' foreachVariable (',' foreachVariable)? 'in' iterableObject ')' statement
//    ;
ast::Stmt* Parser::ParseForeachStatement() {
    return nullptr;
}

// foreachVariable
//     : IDENTIFIER (':' type)?
//     ;
ast::Node* Parser::ParseForeachVariable() {
    return nullptr;
}

// iterableObject
//    : IDENTIFIER
//    | mapInitializer
//    | arrayInitializer
//    ;
ast::Node* Parser::ParseIterableObject() {
    return nullptr;
}



// whileStatement
//    : 'while' '(' expression ')' statement
//    ;
ast::Stmt* Parser::ParseWhileStatement() {
    return nullptr;
}

// doStatement
//    : 'do' statement 'while' '(' expression ')'
//    ;
ast::Stmt* Parser::ParseDoStatement() {
    return nullptr;
}

// switchStatement
//    : 'switch' '(' expression ')' '{' switchCase*defaultCase? '}'
//    ;
ast::Stmt* Parser::ParseSwitchStatement() {
    return nullptr;
}

// switchCase
//    : ('case' expression ':')+ statement
//    ;
ast::Node* Parser::ParseSwitchCase() {
    return nullptr;
}

// defaultCase
//    : 'default' ':' statement
//    ;
ast::Node* Parser::ParseSwitchDefault() {
    return nullptr;
}

// returnStatement
//    : 'return' expression? ';'
//    ;
ast::Stmt* Parser::ParseReturnStatement() {
    return nullptr;
}

// breakStatement
//    : 'break' ';'
//    ;
ast::Stmt* Parser::ParseBreakStatement() {
    return nullptr;
}

// continueStatement
// continueStatement
//    : 'continue' IDENTIFIER? ';'
//    ;
ast::Stmt* Parser::ParseContinueStatement() {
    return nullptr;
}

// assertStatement
//    : 'assert' '(' expression ')' ';'
//    ;
ast::Stmt* Parser::ParseAssertStatement() {
    return nullptr;
}


// throwStatement
//    : 'throw' expression ';'
//    ;
ast::Stmt* Parser::ParseThrowStatement() {
    return nullptr;
}

// tryStatement
//    : 'try' block catchParts? finallyPart?
//    ;
ast::Stmt* Parser::ParseTryStatement() {
    return nullptr;
}

// catchParts
//    : catchPart*
//    ;
ast::Stmt* Parser::ParseCatchParts() {
    return nullptr;
}

// catchPart
//    : 'catch' '('catchType IDENTIFIER ')' block 
//    ;
ast::Stmt* Parser::ParseCatchPart() {
    return nullptr;
}

// catchType
//    : qualifiedName ( '|' qualifiedName)*
//    ;
ast::Node* Parser::ParseCatchType() {
    return nullptr;
}

// finallyPart
//    : 'finally' block
//    ;
ast::Stmt* Parser::ParseFinallyPart() {
    return nullptr;
}

// expressionStatement
//    : expression ';'
//    ;
ast::Stmt* Parser::ParseExprStatement() {
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
std::vector<ast::Type*> Parser::ParseTypeList() {
    std::vector<ast::Type*> types;
    return types;
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

VarInitializer* Parser::ParseVariableInitializer(){ return nullptr; }

Node* Parser::ParseArrayInitializer(){ return nullptr; }

Node* Parser::ParseMapInitializer(){ return nullptr; }

// Statement parser functions







} // namespace zl
