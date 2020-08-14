#pragma once

#include <string>
#include "ast.h"

namespace zl {

// ParserXmlBuilder will dump the whole parser tree into xml document for
// confirmating wether the parser work normally.
class XmlBuilder {
public:
    explicit XmlBuilder(const std::string& filePath);
    ~XmlBuilder();
    void BuildXml(ast::Node* tree);
};

} // namespace zl
