#pragma once

#include <string>
#include <fstream>
#include "token.h"

using namespace std;

namespace zl {

class Lexer {
public:
    virtual Token* Get() = 0;
    virtual Token* Peek() = 0;
    virtual bool Match(int tokenType, Token** token) = 0;
};


// SimpleLexer
class SimpleLexer : public Lexer {
public:
    explicit SimpleLexer(const string& path, const string& file);
    ~SimpleLexer();
    Token* Get();
    Token* Peek();
    bool Match(int tokenType, Token** token);
private:
    SimpleLexer();
    char GetChar();
    void PutChar(char ch);
    void PushToken(Token* token);
    string GetAtomString(char ch);
    Token* ParseDigitLiteral(char ch);
    Token* ParseKeyWord(const string& name);
private:
    ifstream ifs_;
    string file_;
    string path_;
    string fullFileName_;
};

} // namespace zl


