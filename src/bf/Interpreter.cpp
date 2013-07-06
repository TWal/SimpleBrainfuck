#include "Interpreter.h"
#include <cstdio>
#include <stack>
#include <cstring>

namespace bf {

Interpreter::Interpreter(int positiveMem, int negativeMem) {
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
                optiCommands.push_back({OptimizedCommand::OUTPUT, 0, (short)cmd->shift});
                break;
            case Command::INPUT:
                optiCommands.push_back({OptimizedCommand::INPUT, 0, (short)cmd->shift});
                break;
            case Command::START_WHILE:
                brackets.push(optiCommands.size());
                optiCommands.push_back({OptimizedCommand::START_WHILE, (short)((StartWhile*)cmd)->matching->position, (short)cmd->shift});
                break;
            case Command::END_WHILE:
                optiCommands[brackets.top()].data = optiCommands.size();
                optiCommands.push_back({OptimizedCommand::END_WHILE, brackets.top(), (short)cmd->shift});
                brackets.pop();
                break;
            case Command::COLLAPSED : {
                Collapsed* coll = (Collapsed*)cmd;
                optiCommands.push_back({OptimizedCommand::ADDITIONS, (short)coll->adds.size(), (short)coll->shift});
                for(auto& it : coll->adds) {
                    optiCommands.push_back({OptimizedCommand::DATA, (short)it.second, (short)it.first});
                }
                break;
            }
            case Command::MULTIPLIES: {
                Multiplies* muls = (Multiplies*)cmd;
                optiCommands.push_back({OptimizedCommand::MULTIPLIES, (short)muls->muls.size(), (short)cmd->shift});
                for(auto& it : muls->muls) {
                    optiCommands.push_back({OptimizedCommand::DATA, (short)it.second, (short)it.first});
                }
                break;
            }
            case Command::WHILE_SHIFT:
                optiCommands.push_back({OptimizedCommand::WHILE_SHIFT, (short)((WhileShift*)cmd)->nb, (short)cmd->shift});
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
                int c = getchar();
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

