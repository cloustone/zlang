#pragma once
#include <string>
#include "location.h"

namespace zl {

class ErrorHandler {
public:
    virtual void ErrorAt(const Location& location, const std::string& msg) = 0;

};

} // namespace zl
