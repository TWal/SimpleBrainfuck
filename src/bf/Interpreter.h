#ifndef BF_INTERPRETER_H
#define BF_INTERPRETER_H

#include <vector>
#include "Commands.h"

namespace bf {

class Interpreter {
    public:
        Interpreter();
        ~Interpreter();
        void interpret(const std::vector<Command*>& commands);
        struct OptimizedCommand {
            enum COMMAND_TYPE {
                OUTPUT,
                INPUT,
                START_WHILE,
                END_WHILE,
                ZERO,
                WHILE_SHIFT,
                ADDITIONS,
                MULTIPLIES,
                DATA
            };
            COMMAND_TYPE type;
            short data;
            short shift;
        };

    private:
        char& _memPos(int pos);
        std::vector<char> _negMem;
        std::vector<char> _posMem;
};

} //namespace bf

#endif

