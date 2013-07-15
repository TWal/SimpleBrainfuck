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

#include "Debugger.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unordered_set>

namespace bf {

Debugger::Debugger(std::istream* is, int positiveMem, int negativeMem) {
    _is = is;
    _positiveMem = positiveMem;
    _negativeMem = negativeMem;
    _memToOrigin = new char[positiveMem + negativeMem];
    memset(_memToOrigin, 0, positiveMem + negativeMem);
    _mem = _memToOrigin + negativeMem;
}

Debugger::~Debugger() {
    delete[] _memToOrigin;
}

void Debugger::_resetMemory() {
    delete[] _memToOrigin;
    _memToOrigin = new char[_positiveMem + _negativeMem];
    memset(_memToOrigin, 0, _positiveMem + _negativeMem);
    _mem = _memToOrigin + _negativeMem;
}

void Debugger::debug(const std::string& prog) {
    std::unordered_set<int> watching;
    size_t progPos = 0;
    int memPos = 0;
    while(!std::cin.eof()) {
        std::cout << ">>> " << std::flush;
        std::string input;
        std::getline(std::cin, input);
        size_t delimiter = input.find_first_of(" \t\n\r");
        std::string inputCmd = input.substr(0, delimiter);
        std::string inputArgs = delimiter == std::string::npos ? "" : input.substr(delimiter);
        if(inputCmd == "q" || inputCmd == "quit" || inputCmd == "exit") {
            break;
        } else if(inputCmd == "r" || inputCmd == "run") {
            bool stop = false;
            for(;progPos < prog.size() && !stop; ++progPos) {
                switch(prog[progPos]) {
                    case '+':
                        ++_mem[memPos];
                        if(watching.count(memPos) > 0) {
                            stop = true;
                        }
                        break;
                    case '-':
                        --_mem[memPos];
                        if(watching.count(memPos) > 0) {
                            stop = true;
                        }
                        break;
                    case '<':
                        --memPos;
                        break;
                    case '>':
                        ++memPos;
                        break;
                    case ',': {
                        int c = _is->get();
                        _mem[memPos] = (c == EOF ? 0 : c);
                        break;
                    }
                    case '.':
                        putchar(_mem[memPos]);
                        break;
                    case '[':
                        if(!_mem[memPos]) {
                            int depth = 1;
                            while(depth) {
                                ++progPos;
                                depth += prog[progPos] == '[';
                                depth -= prog[progPos] == ']';
                            }
                        }
                        break;
                    case ']':
                        if(_mem[memPos]) {
                            int depth = 1;
                            while(depth) {
                                --progPos;
                                depth -= prog[progPos] == '[';
                                depth += prog[progPos] == ']';
                            }
                        }
                        break;
                    case '#':
                        stop = true;
                        break;
                    default:
                        break;
                }
            }
            if(progPos != prog.size()) {
                size_t progPosBackup = progPos;
                --progPos;
                while(prog[++progPos] != '\n' && progPos < prog.size());
                size_t lineEnd = progPos-1;
                while(prog[--progPos] != '\n' && progPos);
                size_t lineBegin = progPos+1;
                size_t lineNb = 1;
                ++progPos;
                while(progPos-- != 0) {
                    lineNb += prog[progPos] == '\n';
                }
                std::cout << "Breakpoint reached, line " << lineNb << ", cell " << memPos << ":" << std::endl << prog.substr(lineBegin, lineEnd - lineBegin + 1) << std::endl;
                progPos = progPosBackup;
                for(size_t i = lineBegin+1; i < progPos; ++i) {
                    std::cout << " ";
                }
                std::cout << "^" << std::endl;
            } else {
                std::cout << "Program ended successfully" << std::endl;
                progPos = 0;
                _resetMemory();
            }
        } else if(inputCmd == "d" || inputCmd == "dump" || inputCmd == "hexdump") {
            size_t colonPos = inputArgs.find_first_of(":");
            if(colonPos == std::string::npos) {
                hexdump(0, atoi(inputArgs.data()));
            } else {
                hexdump(atoi(inputArgs.substr(0, colonPos).data()), atoi(inputArgs.substr(colonPos+1).data()));
            }
        } else if(inputCmd == "dummydump") {
            size_t colonPos = inputArgs.find_first_of(":");
            if(colonPos == std::string::npos) {
                dummydump(0, atoi(inputArgs.data()));
            } else {
                dummydump(atoi(inputArgs.substr(0, colonPos).data()), atoi(inputArgs.substr(colonPos+1).data()));
            }
        } else if(inputCmd == "w" || inputCmd == "watch") {
            watching.insert(atoi(inputArgs.data()));
        }
    }
}

void Debugger::hexdump(int min, int max) {
    int i, j;
    for (i = min; i < max; i += 16) {
        printf("%07x: ", i);
        for (j = 0; j < 16; ++j) {
            if (i+j < max) {
                printf("%02x ", _mem[i+j]);
            } else {
                printf("   ");
            }
        }
        printf(" ");
        for (j = 0; j < 16; ++j) {
            if (i+j < max) {
                printf("%c", isprint(_mem[i+j]) ? _mem[i+j] : '.');
            }
        }
        printf("\n");
    }
}

void Debugger::dummydump(int min, int max) {
    for(int i = min; i < max; ++i) {
        if(isprint(_mem[i])) {
            printf("%d: %03d %c\n", i, _mem[i], _mem[i]);
        } else {
            printf("%d: %03d\n", i, _mem[i]);
        }
    }
}

} //namespace bf

