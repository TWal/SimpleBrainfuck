#ifndef SBF_COMPILER_H
#define SBF_COMPILER_H

#include <string>
#include <unordered_map>
#include <vector>
#include "BasicMacro.h"

namespace sbf {

class Compiler {
    public:
        Compiler();
        void compileFromFile(const std::string& file, std::string& output);
        void compileFromString(const std::string& str, std::string& output);
    private:
        void _readFile(const std::string& file, std::string& output);
        void _trim(std::string& str);
        std::vector<std::vector<std::string>> _variables;
        std::unordered_map<std::string, BasicMacro*> _macros;
        unsigned int _currentVarWorld;
        unsigned int _currentVarPos;
};

} //namespace sbf

#endif

