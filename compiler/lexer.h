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
    Lexer(const string& fullpath);

    // Load source code from specified string buffer
    Lexer(const char* codes);
    ~Lexer();

    // Return the next token in lexer
    const Token Next() { return NextToken(); }

    // Return the previous token position
    void Back();

    // Peek the next token
    const Token Peek() { return NextToken(true); }

    // Check wetther the next token is specified token, return the mached token
    // if matched
    bool Match(int tokenType, Token* token = nullptr) { return false;}
    bool Match(char ch) { return false;}
    bool Eof() const { return  false; }
    Location GetLocation() const { return Location(lineno_); }

private:
    Token NextToken(bool mark = false);
    Token ParseStringLiteral(char ch);
    Token ParseDigitalLiteral(char ch);
    Token ParseAlphaToken(char ch);
    std::string GetAtomString(char ch);
    void UpdateMark() { mark_ = index_; }
    void PutbackChar(){ index_--; }

    char NextChar() {
        if (index_ == bufSize_)
            return EOF;
        char ch = *(buf_ + index_); 
        index_++;
        return ch;
    }

private:
    // the source file full path name
    std::string fullFileName_;
    // file descriptor for source file
    int fd_;
    // buffer pointer to mmaped buffer or user input buffer
    const char* buf_;
    size_t bufSize_;
    // mark indication to current index postion, using in peek or back
    int mark_;
    // current char index
    int index_;
    // current line number
    int lineno_;
};

} // namespace zl


