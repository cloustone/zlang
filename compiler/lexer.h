#pragma once

#include <string>
#include <fstream>
#include <exception>
#include "token.h"

using namespace std;

namespace zl {

class Lexer {
public:
    // Load source code from specifed fullpath file
    Lexer(const string& fullpath) throw(std::invalid_argument);

    // Load source code from specified string buffer
    Lexer(const char* codes) throw(std::invalid_argument);
    ~Lexer();

    // Return the next token in lexer
    const Token Next() { return NextToken(); }

    // Peek the next token
    const Token Peek() { return NextToken(true); }

    // Check wetther the next token is specified token, return the mached token
    // if matched
    bool Match(int tokenType, Token** token);

private:
    Token NextToken(bool mark = false);
    Token ParseStringLiteral(char ch);
    Token ParseDigitalLiteral(char ch);
    Token ParseIdentifier(char ch);
    Token ParseAlphaToken(char ch);
    std::string GetAtomString(char ch);

    char  NextChar() {
        if (index_ == bufSize_)
            return EOF;
        char ch = *(buf_ + index_); 
        index_++;
        return ch;
    }

private:
    string fullFileName_;
    int fd_ = -1;
    const char* buf_;
    size_t bufSize_;
    int mark_ = 0;
    int index_ = 0;
    int lineno_ = 0;
};

} // namespace zl


