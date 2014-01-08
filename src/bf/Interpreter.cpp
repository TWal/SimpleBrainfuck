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

#include "Interpreter.h"
#include <cstdio>
#include <stack>
#include <cstring>

namespace bf {

Interpreter::Interpreter(std::istream* is, int positiveMem, int negativeMem) {
    _is = is;
    _memToOrigin = new char[positiveMem + negativeMem];
    _mem = _memToOrigin + negativeMem;
    memset(_memToOrigin, 0, positiveMem + negativeMem);
}

Interpreter::~Interpreter() {
    delete[] _memToOrigin;
}

void Interpreter::interpret(const std::vector<Command*>& commands) {
    std::vector<OptimizedCommand> optiCommands;
    std::stack<short> brackets;

    for(Command* cmd : commands) {
        switch(cmd->type) {
            case Command::OUTPUT:
                optiCommands.push_back({OptimizedCommand::OUTPUT, 0, 0});
                break;
            case Command::INPUT:
                optiCommands.push_back({OptimizedCommand::INPUT, 0, 0});
                break;
            case Command::START_WHILE:
                brackets.push(optiCommands.size());
                optiCommands.push_back({OptimizedCommand::START_WHILE, (short)((StartWhile*)cmd)->matching->position, 0});
                break;
            case Command::END_WHILE:
                optiCommands[brackets.top()].data = optiCommands.size();
                optiCommands.push_back({OptimizedCommand::END_WHILE, brackets.top(), 0});
                brackets.pop();
                break;
            case Command::ADDS : {
                Adds* adds = (Adds*)cmd;
                if(adds->adds.size() == 0) {
                    auto it = optiCommands.rbegin();
                    while(it != optiCommands.rend() && it->type == OptimizedCommand::DATA) {
                        ++it;
                    }
                    if(it != optiCommands.rend()) {
                        it->shift += adds->shift;
                    } else {
                        optiCommands.push_back({OptimizedCommand::ADDITIONS, 0, (short)adds->shift});
                    }
                } else {
                    optiCommands.push_back({OptimizedCommand::ADDITIONS, (short)adds->adds.size(), (short)adds->shift});
                    for(auto& it : adds->adds) {
                        optiCommands.push_back({OptimizedCommand::DATA, (short)it.second, (short)it.first});
                    }
                }
                break;
            }
            case Command::MULTIPLIES: {
                Multiplies* muls = (Multiplies*)cmd;
                optiCommands.push_back({OptimizedCommand::MULTIPLIES, (short)muls->muls.size(), 0});
                for(auto& it : muls->muls) {
                    optiCommands.push_back({OptimizedCommand::DATA, (short)it.second, (short)it.first});
                }
                break;
            }
            case Command::WHILE_SHIFT:
                optiCommands.push_back({OptimizedCommand::WHILE_SHIFT, (short)((WhileShift*)cmd)->nb, 0});
                break;
            default:
                break;
        }
    }





    int pointerPos = 0;
    unsigned int programPos = 0;

    while(programPos < optiCommands.size()) {
        OptimizedCommand& cmd = optiCommands[programPos];
        switch(cmd.type) {
            case OptimizedCommand::OUTPUT: {
                putchar(_mem[pointerPos]);
                pointerPos += cmd.shift;
                break;
           } case OptimizedCommand::INPUT: {
                int c = _is->get();
                _mem[pointerPos] = (c == EOF ? 0 : c);
                pointerPos += cmd.shift;
                break;
            } case OptimizedCommand::START_WHILE: {
                if(!_mem[pointerPos]) {
                    programPos = cmd.data;
                    pointerPos += optiCommands[programPos].shift;
                } else {
                    pointerPos += cmd.shift;
                }
                break;
            } case OptimizedCommand::END_WHILE: {
                if(_mem[pointerPos]) {
                    programPos = cmd.data;
                    pointerPos += optiCommands[programPos].shift;
                } else {
                    pointerPos += cmd.shift;
                }
                break;
            } case OptimizedCommand::ZERO: {
                _mem[pointerPos] = 0;
                pointerPos += cmd.shift;
                break;
            } case OptimizedCommand::WHILE_SHIFT: {
                while(_mem[pointerPos] != 0) {
                    pointerPos += cmd.data;
                }
                pointerPos += cmd.shift;
                break;
            } case OptimizedCommand::ADDITIONS: {
                while(optiCommands[++programPos].type == OptimizedCommand::DATA) {
                    _mem[pointerPos + optiCommands[programPos].shift] += optiCommands[programPos].data;
                }
                --programPos;
                pointerPos += cmd.shift;
                break;
            } case OptimizedCommand::MULTIPLIES: {
                char& initialVal = _mem[pointerPos];
                while(optiCommands[++programPos].type == OptimizedCommand::DATA) {
                    _mem[pointerPos + optiCommands[programPos].shift] += initialVal * optiCommands[programPos].data;
                }
                --programPos;
                initialVal = 0;
                pointerPos += cmd.shift;
                break;
            } default:
                break;
        }
        ++programPos;
    }
}

} //namespace bf

