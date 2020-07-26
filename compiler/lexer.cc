#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <error.h>
#include "lexer.h"

namespace zl {

// Load source code from specifed fullpath file
Lexer::Lexer(const string& fullpath) throw (std::invalid_argument) {
    struct stat sb;
    if ((fd_ = open(fullpath.c_str(), O_RDWR)) < 0) 
        throw std::invalid_argument("file no exist");
    if (fstat(fd_, &sb) < 0) 
        throw std::invalid_argument("invalid file state");
    if ((buf_ = (char*)mmap(nullptr, sb.st_size, PROT_READ, MAP_SHARED, fd_, 0)) == nullptr) 
        throw std::invalid_argument("file map failed");

    bufSize_ = sb.st_size;
    fullFileName_ = fullpath;
    mark_ = 0;
}

// Load source code from specified string buffer
Lexer::Lexer(const char* codes) throw(std::invalid_argument) {
    if (codes == nullptr) 
        throw std::invalid_argument("invalid code buffer");
    fd_ = -1;
    buf_ = codes;
    mark_ = 0;
}


Lexer::~Lexer() {
    if (fd_ > 0) {
        munmap((void*)buf_, bufSize_);
        close(fd_);
    }
    fd_ = -1;
    buf_ = nullptr;
}

// The function retrieve a while string from current index
std::string Lexer::GetAtomString(char ch) {
    return std::string("");
}

Token Lexer::ParseStringLiteral(char ch) {
    return Token();
}

Token Lexer::ParseDigitalLiteral(char ch) {
    return Token();
}
Token Lexer::ParseIdentifier(char ch) {
    return Token();
}

Token Lexer::ParseAlphaToken(char ch) {
    return Token();
}

// The function return next token internal
Token Lexer::NextToken(bool mark) {
    char ch;
    
    while ((ch = NextChar()) != EOF) {
        switch (ch) {
            case '/':
                // consume comments
                if (((ch = NextChar()) != EOF) && (ch = '/')) {
                    while (((ch = NextChar()) != EOF)) {
                        if (ch == '\r' || ch == '\n') {
                            lineno_++;
                            break; 
                        }
                    }
                }
                break;
            case '+':
            case '-':
            case '*':
            case ':':
            case '.':
            case '&':
            case '!':
            case '=':
            case '>':
            case '<':
            case ';':
            case '%':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '?':
            case ',':
            case '#':
            case '~':
            case '`':
                return Token(ch, Token::OP, lineno_);
            case '\r':
                continue;
                break;
            case '\n':
                lineno_ ++;
                break;
            case ' ':
                continue;
                break;

            case '"':
                return ParseStringLiteral(ch);
            default:
                if (isdigit(ch))
                    return ParseDigitalLiteral(ch);
                if (isalpha(ch))
                    return ParseAlphaToken(ch);
                return Token(GetAtomString(ch), Token::UNKNOWN, lineno_++);
                break;
        }
    }
    return Token();
}


} // namespace zl
