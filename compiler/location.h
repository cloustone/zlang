#pragma once
#include <string>

using namespace std;

namespace zl {

class Location {
    int lineno_;
public:
    Location() { lineno_ = -1; }
    Location(int lineno):lineno_(lineno){}
    ~Location(){}
    int GetLineno() const { return lineno_;  }
    void SetLineno(int lineno) {  lineno_ = lineno; }

    Location& operator = (const Location &location) {
        lineno_ = location.lineno_;
        return *this;
    }
    void operator = (int v) { lineno_ = v; }
};

class LocationManager {
public:
    LocationManager(){}
    ~LocationManager(){}
    void SetCompileSourceFile(const string& file) { sourceFile_ = file; }
    void SetCompileSourcePath(const string& path) { sourcePath_ = path; }
    const string& GetCompileSourceFile() { return sourceFile_; }
    const string& GetCompileSourcePath() { return sourcePath_; }
private:
    string sourceFile_;
    string sourcePath_;
};

} // namespace zl


