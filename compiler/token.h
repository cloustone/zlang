#pragma once
#include "location.h"

namespace zl {

class Token {
public:
    enum TokenType { UNKNOWN, PACKAGE, IMPORT, OP, STRING, ID, DIGITAL, CLASS, INTERFACE, LET, VAR, DEF,
        IF, WHILE, IN, FOR, PUBLIC, PRIVATE,  FINAL, EXTENDS, IMPLEMENTS, BOOL, USING, FUNCTION, ASSIGN,
        CONST,
    };
    static Token InvalidToken;
public:
    string assic_;
    int type_;
    Location location_;

    Token():type_(-1), location_(-1){}
    Token(const char* name, int type, int lineno)
        :assic_(name), type_(type), location_(lineno) {}

    Token(const std::string& name, int type, int lineno)
        :assic_(name), type_(type), location_(lineno) {}

    Token(char ch, int type, int lineno)
        :type_(type), location_(lineno) {
            assic_ = ch;
        }
    bool Valid() {
        return assic_ =="" || type_ == TokenType::UNKNOWN;
    }
    bool operator == (const Token& rhs) {
        return (this->assic_ == rhs.assic_ && this->location_ == rhs.location_);
    }
};

} // namespace zl
