#include "Compiler.h"

#include <iostream>

namespace sbf {

Compiler::Compiler() {
}

void Compiler::compileFromFile(const std::string& file, std::string& output) {
    std::string str;
    _readFile(file, str);
    compileFromString(str, output);
}

void Compiler::compileFromString(const std::string& str, std::string& output) {
    output.clear();
    for(unsigned int i = 0; i < str.size(); ++i) {
        if(str[i] == '#' || str[i] == '_') {
            char prependingChar = str[i];
            if(str[i+1] == '_') {
                while(str[i] != '\n') {
                    ++i;
                }
                continue;
            }

            std::size_t openingPar = str.find("(", i);
            if(openingPar == std::string::npos) {
                std::cerr << "No opening parenthesis" << std::endl;
                return;
            }
            std::string funcName = str.substr(i+1, openingPar-i-1);
            _trim(funcName);
            unsigned int lastArgStart = openingPar+1;
            int deepness = 0;
            std::vector<std::string> args;
            i = lastArgStart;
            while(str[i] != ')' || deepness != 0) {
                if(str[i] == '(') {
                    ++deepness;
                } else if(str[i] == ')') {
                    --deepness;
                }
                if((str[i] == ';' || str[i+1] == ')') && (deepness == 0 || (deepness == 1 && str[i] == ')'))) {
                    std::string arg = str.substr(lastArgStart, i-lastArgStart + (int)(str[i+1] == ')'));
                    args.push_back("");
                    compileFromString(arg, args.back());
                    _trim(args.back());
                    lastArgStart = i+1;
                }
                ++i;
            }

            std::string macroResult;
            if(prependingChar == '#') {
                if(funcName == "include") {
                    for(const std::string& arg : args) {
                        std::string fileContent;
                        _readFile(arg, fileContent);
                        macroResult += fileContent;
                    }
                } else if(funcName == "macro") {
                    std::string firstArg = args.front();
                    std::string lastArg = args.back();
                    args.pop_back();
                    args.erase(args.begin());
                    _macros[firstArg] = new BasicMacro(args, lastArg);
                } else if(funcName == "variables") {
                    _variables.push_back(args);
                }
            } else {
                if(funcName == "at") {
                    bool varNameFound = false;
                    for(unsigned int i = 0; i < _variables.size() && !varNameFound; ++i) {
                        for(unsigned int j = 0; j < _variables[i].size() && !varNameFound; ++j) {
                            if(_variables[i][j] == args[0]) {
                                _currentVarWorld = i;
                                _currentVarPos = j;
                                varNameFound = true;
                            }
                        }
                    }
                    if(!varNameFound) {
                        std::cerr << "Variable name not found" << std::endl;
                    }
                } else if (funcName == "to") {
                    bool varNameFound = false;
                    for(unsigned int i = 0; i < _variables.size() && !varNameFound; ++i) {
                        for(unsigned int j = 0; j < _variables[i].size() && !varNameFound; ++j) {
                            if(_variables[i][j] == args[0]) {
                                if(i != _currentVarWorld) {
                                    std::cerr << "Going to a different variable world" << std::endl;
                                }
                                if(_currentVarPos < j) {
                                    for(unsigned int k = 0; k < j-_currentVarPos; ++k) {
                                        macroResult.push_back('>');
                                    }
                                } else {
                                    for(unsigned int k = 0; k < _currentVarPos-j; ++k) {
                                        macroResult.push_back('<');
                                    }
                                }
                                _currentVarPos = j;
                                varNameFound = true;
                            }
                        }
                    }
                    if(!varNameFound) {
                        std::cerr << "Variable name not found" << std::endl;
                    }
                } else {
                    _macros[funcName]->compute(args, macroResult);
                }
            }
            std::string finalMacroResult;
            compileFromString(macroResult, finalMacroResult);
            output += finalMacroResult;
        } else {
            output.push_back(str[i]);
        }
    }
}

void Compiler::_readFile(const std::string& file, std::string& output) {
    output.clear();
    if(FILE *fp = fopen(file.c_str(), "r")) {
        char buf[1024];
        while(size_t len = fread(buf, 1, sizeof(buf), fp)) {
            output.insert(output.end(), buf, buf + len);
        }
        fclose(fp);
    }
}
void Compiler::_trim(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\n\r")); //ltrim
    str.erase(str.find_last_not_of(" \t\n\r")+1); //rtrim
}

} //namespace sbf

