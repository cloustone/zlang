#pragma once

#include "parser_node.h"
#include <string>

namespace zl {

// ParserXmlBuilder will dump the whole parser tree into xml document for
// confirmating wether the parser work normally.
class ParserXmlBuilder {
public:
    explicit ParserXmlBuilder(const std::string& filePath);
    ~ParserXmlBuilder();
    void BuildXml(Node* parserTree);
};

} // namespace zl
