#pragma once

#include "parser_node.h"
#include "ast.h"

namespace zl {

class ASTBuilder {
public:
    static ASTNode* Build(Node* parseTree); 

};

} // namespace zl
