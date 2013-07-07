/* Copyright (c) 2013, Theophile Wallez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Compiler.h"

#include <iostream>

namespace sbf {

Compiler::Compiler() {
}

Compiler::~Compiler() {
    for(auto& it : _macros) {
        delete it.second;
    }
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

            size_t openingPar = str.find("(", i);
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
                    args.push_back(arg);
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
                } else if(funcName == "cmacro") {
                    std::string firstArg = args.front();
                    std::string rest;
                    for(unsigned int j = 1; j < args.size(); ++j) {
                        rest += args[j];
                        if(j != args.size()-1) {
                            rest.push_back(';');
                        }
                    }
                    _macros[firstArg] = new TccMacro(rest);
                } else if(funcName == "variables") {
                    std::string worldName = args.front();
                    args.erase(args.begin());
                    _variables[worldName] = args;
                }
            } else {
                if(_macros.find(funcName) == _macros.end()) {
                    std::cerr << "Macro '" << funcName << "' not found" << std::endl;
                    return;
                }
                _macros[funcName]->compute(args, macroResult);
            }
            std::string finalMacroResult;
            compileFromString(macroResult, finalMacroResult);
            output += finalMacroResult;
        } else if(str[i] == '@') {
            size_t nextArobas = str.find("@", i+1);
            if(nextArobas == std::string::npos) {
                std::cerr << "No '@' after '@'" << std::endl;
                return;
            }
            size_t nextSemicolon = str.find(";", nextArobas);
            if(nextSemicolon == std::string::npos) {
                std::cerr << "No ';' after '@'" << std::endl;
                return;
            }
            std::string worldName = str.substr(i+1, nextArobas-i-1);
            std::string variableName = str.substr(nextArobas+1, nextSemicolon-nextArobas-1);
            _trim(worldName);
            _trim(variableName);
            if(_variables.find(worldName) == _variables.end()) {
                std::cerr << "Unknown variable world '" << worldName << "'" << std::endl;
                return;
            }
            _currentVarWorld = worldName;
            bool found = false;
            for(unsigned int j = 0; j < _variables[_currentVarWorld].size(); ++j) {
                if(_variables[_currentVarWorld][j] == variableName) {
                    _currentVarPos = j;
                    found = true;
                    break;
                }
            }
            if(!found) {
                std::cerr << "Variable '" << variableName << "' not found in world '" << _currentVarWorld << "'" << std::endl;
                return;
            }
            i = nextSemicolon;
        } else if(str[i] == '=') {
            size_t semicolon = str.find(";", i);
            if(semicolon == std::string::npos) {
                std::cerr << "No ';' after '='" << std::endl;
                return;
            }
            std::string variableName = str.substr(i+1, semicolon-i-1);
            _trim(variableName);
            bool found = false;
            for(unsigned int j = 0; j < _variables[_currentVarWorld].size(); ++j) {
                if(_variables[_currentVarWorld][j] == variableName) {
                    if(_currentVarPos < j) {
                        for(unsigned int k = 0; k < j-_currentVarPos; ++k) {
                            output.push_back('>');
                        }
                    } else {
                        for(unsigned int k = 0; k < _currentVarPos-j; ++k) {
                            output.push_back('<');
                        }
                    }
                    _currentVarPos = j;
                    found = true;
                    break;
                }
            }
            if(!found) {
                std::cerr << "Variable '" << variableName << "' not found in world '" << _currentVarWorld << "'" << std::endl;
                return;
            }
            i = semicolon;
        } else if(str[i] == '(') {
            size_t matchingParenthesis = str.find(")", i);
            if(matchingParenthesis == std::string::npos) {
                std::cerr << "No ')' after '('" << std::endl;
                return;
            }
            size_t numberEnd = str.find_first_not_of("0123456789", matchingParenthesis+1);
            if(numberEnd == matchingParenthesis+1) {
                std::cerr << "Parenthesis must have numbers after them" << std::endl;
                return;
            }
            std::string exprToMultiply = str.substr(i+1, matchingParenthesis-i-1);
            int number = atoi(str.substr(matchingParenthesis+1, numberEnd-matchingParenthesis-1).data());
            std::string exprMultiplied;
            for(int j = 0; j < number; ++j) {
                exprMultiplied += exprToMultiply;
            }
            std::string result;
            compileFromString(exprMultiplied, result);
            output += result;
            i = numberEnd-1;
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

