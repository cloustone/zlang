#pragma once
#include "location.h"

namespace zl {

class Token {
public:
    enum TokenType { 
        ILLEGAL = -1,
        COMMENT,
        LITERAL_BEGIN,
        // Identifiers and basic type literals
        // (these tokens stand for classes of literals)
        ID,  // identifier 
        INT,    // 12345
        FLOAT,  // 123.45
        IMAG,   // 123.45i
        CHAR,   // 'a'
        STRING, // "abc"
        LITERAL_END,
        OPERATOR_BEGIN,
        // Operators and delimiters
        ADD, // +
        SUB, // -
        MUL, // *
        QUO, // /
        REM, // %

        AND,     // &
        OR,      // |
        XOR,     // ^
        SHL,     // <<
        SHR,     // >>
        AND_NOT, // &^

        ADD_ASSIGN, // +=
        SUB_ASSIGN, // -=
        MUL_ASSIGN, // *=
        QUO_ASSIGN, // /=
        REM_ASSIGN, // %=

        AND_ASSIGN,     // &=
        OR_ASSIGN,      // |=
        XOR_ASSIGN,     // ^=
        SHL_ASSIGN,     // <<=
        SHR_ASSIGN,     // >>=
        AND_NOT_ASSIGN, // &^=

        LAND,  // &&
        LOR,   // ||
        ARROW, // <-
        INC,   // ++
        DEC,   // --

        EQL,    // ==
        LSS,    // <
        GTR,    // >
        ASSIGN, // =
        NOT,    // !

        NEQ,      // !=
        LEQ,      // <=
        GEQ,      // >=
        DEFINE,   // :=
        ELLIPSIS, // ...

        LPAREN, // (
        LBRACK, // [
        LBRACE, // {
        COMMA,  // ,
        PERIOD, // .

        RPAREN,    // )
        RBRACK,    // ]
        RBRACE,    // }
        SEMICOLON, // ;
        COLON,     // :
        OPERATOR_END,	

        KEYWORD_BEGIN,	
        // Keywords
        BREAK,
        CASE,
        CHAN,
        CONST,
        CONTINUE,

        DEFAULT,
        DEFER,
        ELSE,
        FALLTHROUGH,
        FOR,

        FUNC,
        GOTO,
        IF,
        IMPORT,

        INTERFACE,
        MAP,
        PACKAGE,
        RANGE,
        RETURN,

        SELECT,
        CLASS,	
        SWITCH,
        VAR,
        PUBLIC,
        PRIVATE,
        USING,
        FUNCTION,
        VOID,
        IMPLEMENTS,
        KEYWORD_END,
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
    bool Valid() const {
        return assic_ =="" || type_ == TokenType::ILLEGAL;
    }
    bool operator == (const Token& rhs) {
        return (this->assic_ == rhs.assic_ && this->location_ == rhs.location_);
    }
};

inline std::string TokenTypeString(Token::TokenType type) {
    return "" ;// now
}

} // namespace zl
