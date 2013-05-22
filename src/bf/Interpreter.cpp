#include "Interpreter.h"
#include <cstdio>
#include <stack>

namespace bf {

Interpreter::Interpreter() {
}

Interpreter::~Interpreter() {
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
                optiCommands.push_back({OptimizedCommand::OUTPUT, 0, 0});
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
            case Command::COLLAPSED : {
                Collapsed* coll = (Collapsed*)cmd;
                optiCommands.push_back({OptimizedCommand::ADDITIONS, (short)coll->adds.size(), (short)coll->pointerShift});
                for(auto& it : coll->adds) {
                    optiCommands.push_back({OptimizedCommand::DATA, (short)it.second, (short)it.first});
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
                putchar(_memPos(pointerPos));
                break;
           } case OptimizedCommand::INPUT: {
                int c = getchar();
                _memPos(pointerPos) = (c == EOF ? 0 : c);
                break;
            } case OptimizedCommand::START_WHILE: {
                if(!_memPos(pointerPos)) {
                    programPos = cmd.data;
                }
                break;
            } case OptimizedCommand::END_WHILE: {
                if(_memPos(pointerPos)) {
                    programPos = cmd.data;
                }
                break;
            } case OptimizedCommand::ZERO: {
                _memPos(pointerPos) = 0;
                break;
            } case OptimizedCommand::WHILE_SHIFT: {
                while(_memPos(pointerPos) != 0) {
                    pointerPos += cmd.shift;
                }
                break;
            } case OptimizedCommand::ADDITIONS: {
                for(short i = 0; i < cmd.data; ++i) {
                    ++programPos;
                    _memPos(pointerPos + optiCommands[programPos].shift) += optiCommands[programPos].data;
                }
                pointerPos += cmd.shift;
                break;
            } case OptimizedCommand::MULTIPLIES: {
                char initialVal = _memPos(pointerPos);
                for(short i = 0; i < cmd.data; ++i) {
                    ++programPos;
                    _memPos(pointerPos + optiCommands[programPos].shift) += initialVal * optiCommands[programPos].data;
                }
                _memPos(pointerPos) = 0;
                break;
            } default:
                break;
        }
        ++programPos;
    }
}

char& Interpreter::_memPos(int pos) {
    if(pos >= 0) {
        if((unsigned int)pos >= _posMem.size()) {
            _posMem.resize(pos+1, 0);
        }
        return _posMem[pos];
    } else {
        pos = -1 - pos;
        if((unsigned int)pos >= _negMem.size()) {
            _negMem.resize(pos+1, 0);
        }
        return _negMem[pos];
    }
}

} //namespace bf

