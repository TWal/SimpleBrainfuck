#include "Interpreter.h"
#include <cstdio>

namespace bf {

Interpreter::Interpreter() {
}

Interpreter::~Interpreter() {
}

void Interpreter::interpret(const std::vector<Command*>& commands) {
    int pointerPos = 0;
    unsigned int programPos = 0;

    while(programPos < commands.size()) {
        Command* command = commands[programPos];
        switch(command->type) {
            case Command::OUTPUT: {
                putchar(_memPos(pointerPos));
                break;
            } case Command::INPUT: {
                int c = getchar();
                _memPos(pointerPos) = (c == EOF ? 0 : c);
                break;
            } case Command::POINTER_LEFT: {
                --pointerPos;
                break;
            } case Command::POINTER_RIGHT: {
                ++pointerPos;
                break;
            } case Command::MINUS: {
                --_memPos(pointerPos);
                break;
            } case Command::PLUS: {
                ++_memPos(pointerPos);
                break;
            } case Command::START_WHILE: {
                if(!_memPos(pointerPos)) {
                    programPos = ((StartWhile*)command)->matching->position;
                }
                break;
            } case Command::END_WHILE: {
                if(_memPos(pointerPos)) {
                    programPos = ((EndWhile*)command)->matching->position;
                }
                break;
            } case Command::MULTI_ADDS: {
                for (auto it : ((MultiAdds*)command)->adds) {
                    _memPos(pointerPos + it.first) += it.second;
                }
                pointerPos += ((MultiAdds*)command)->pointerShift;
                break;
            } case Command::MULTIPLIES: {
                char value = _memPos(pointerPos);
                for (auto it : ((Multiplies*)command)->muls) {
                    _memPos(pointerPos + it.first) += it.second * value;
                }
                _memPos(pointerPos) = 0;
                break;
            }
            default:
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

