#include "BfInterpreter.h"
#include <cstdio>
#include <iostream>

BfInterpreter::BfInterpreter() {
}

BfInterpreter::~BfInterpreter() {
}

void BfInterpreter::interpret(const std::vector<BfCommand*>& commands) {
    int pointerPos = 0;
    unsigned int programPos = 0;

    while(programPos < commands.size()) {
        BfCommand* command = commands[programPos];
        switch(command->type) {
            case BfCommand::BF_OUTPUT: {
                putchar(_memPos(pointerPos));
                break;
            } case BfCommand::BF_INPUT: {
                int c = getchar();
                _memPos(pointerPos) = (c == EOF ? 0 : c);
                break;
            } case BfCommand::BF_POINTER_LEFT: {
                --pointerPos;
                break;
            } case BfCommand::BF_POINTER_RIGHT: {
                ++pointerPos;
                break;
            } case BfCommand::BF_MINUS: {
                --_memPos(pointerPos);
                break;
            } case BfCommand::BF_PLUS: {
                ++_memPos(pointerPos);
                break;
            } case BfCommand::BF_START_WHILE: {
                if(!_memPos(pointerPos)) {
                    programPos = ((BfStartWhile*)command)->matching;
                }
                break;
            } case BfCommand::BF_END_WHILE:{
                if(_memPos(pointerPos)) {
                    programPos = ((BfEndWhile*)command)->matching;
                }
                break;
            }
            default:
                break;
        }
        ++programPos;
    }
}

char& BfInterpreter::_memPos(int pos) {
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

