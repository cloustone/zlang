#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <error.h>
#include "lexer.h"
#include <map>
#include <string>

namespace zl {

// Map betweend keyword and name
struct {
    std::string name;
    int type;
} keywords_[] = {
    { "package",    Token::PACKAGE },
    { "import",     Token::IMPORT },
    { "class",      Token::CLASS },
    { "interface",  Token::INTERFACE },
    { "extends",    Token::EXTENDS },
    { "final",      Token::FINAL},
    { "implements", Token::IMPLEMENTS },
    { "var",        Token::VAR },
    { "if",         Token::IF },
    { "while",      Token::WHILE },
};
static std::map<std::string, int> keywordMaps_;

static void InitializeKeywords() {
    static bool initialized = false;
    if (!initialized) {
        for (auto &kd :keywords_) {
            keywordMaps_[kd.name] = kd.type;
        }
        initialized = true;
    }
}

static int GetKeyword(const std::string& name) {
    std::map<std::string, int>::iterator iter = keywordMaps_.find(name);
    if (iter != keywordMaps_.end()) 
        return iter->second;
    return -1;
}

// Load source code from specifed fullpath file
Lexer::Lexer(const string& fullpath)  {
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
    InitializeKeywords();
}

// Load source code from specified string buffer
Lexer::Lexer(const char* codes) {
    if (codes == nullptr) 
        throw std::invalid_argument("invalid code buffer");
    fd_ = -1;
    buf_ = codes;
    mark_ = 0;
    InitializeKeywords();
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
    std::string atom;
    atom = ch;

    while (((ch = NextChar()) != EOF)) {
        if (isalpha(ch)) {
            atom += ch;
        } else {
            PutbackChar();
            break;
        }
    }
    return atom;
}

Token Lexer::ParseStringLiteral(char ch) {
    std::string atom;

    while (((ch = NextChar()) != EOF)) {
        if (ch == '"') 
            return Token(atom, Token::ID, lineno_++);
        else if (isalpha(ch)) 
            atom += ch;
        else{
            PutbackChar();
            return Token(atom, Token::ID, lineno_++);
        }
    }
    return Token();
}

Token Lexer::ParseDigitalLiteral(char ch) {
    std::string atom;
    atom = ch;

    while (((ch = NextChar()) != EOF)) {
        if (isdigit(ch)) 
            atom += ch;
        else{
            PutbackChar();
            return Token(atom, Token::DIGITAL, lineno_++);
        }
    }
    return Token();
}

// AlphaToken may be identifier or keyword
Token Lexer::ParseAlphaToken(char ch) {
    std::string id = GetAtomString(ch);
    int tokenType = GetKeyword(id);

    if (tokenType > 0) 
        return Token(id, tokenType, lineno_++);
    else 
        return Token(id, Token::ID, lineno_++);
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
