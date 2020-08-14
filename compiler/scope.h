#pragma once 
#include <string>
#include <map>
#include "ast.h"

namespace zl {
namespace ast {

enum class ObjectKind: uint8_t {
    Bad,
    Package,
    Constant,
    Type,
    Variable,
    Func,
    Label,
};
// An object describe a names language entity such as package,
// constant, type, variable, function, or label
struct Object {
    ObjectKind kind;
    std::string name;
    Decl* decl;
    Type* type;
    void* data;
    // Return the object position in source file
    Location Pos();
    // Return the object kind string
    std::string Kind();
};

class Scope {
public:
    explicit Scope(Scope* scope):outer_(scope) {}
    virtual ~Scope() {}

    // Lookup returns the object with the given name if it is
    // found in scope s, otherwise it returns nil. Outer scopes
    // are ignored.
    //
    Object* Lookup(const std::string& name);

    // Insert attempts to insert a named object obj into the scope s.
    // If the scope already contains an object alt with the same name,
    // Insert leaves the scope unchanged and returns alt. Otherwise
    // it inserts obj and returns nil.
    //
    Object* Insert(Object* obj);
    void Dump();
private:
    std::map<std::string, Object*> objects_;
    Scope* outer_;
};

} // namespace ast
} // namespace zl
