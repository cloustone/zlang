#pragma once
#include <vector>
#include "location.h"

namespace zl {

class Token {
public:
    string assic_;
    int type_;
    Location location_;

    Token():type_(-1), location_(-1){}
    Token(const char* name, int type, int lineno)
        :assic_(name), type_(type), location_(lineno) {}

    Token(char ch, int type, int lineno)
        :type_(type), location_(lineno) {
            assic_ = ch;
        }
};


class TokenStream {
public:
    TokenStream();
    ~TokenStream();
    
    void PushToken(Token *token);
    bool MatchToken(int type, Token **token);
    bool MatchToken(int type, const char *name);
    Token* GetToken();
    Token* LookNextToken(); 
    void AdvanceToken(Token** = nullptr);
    void Mark() { mark_ = index_; }
    void ClearMark() { mark_ = 0;}
    void GotoMark() { index_ = mark_; } 
    void Reset() { index_ = 0; } 
    void Clear();
    void DumpAllTokens();
private:
    std::vector<Token*> tokens_;
    size_t mark_;
    size_t index_;
};

} // namespace zl
