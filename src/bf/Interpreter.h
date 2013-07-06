#ifndef BF_INTERPRETER_H
#define BF_INTERPRETER_H

#include <vector>
#include "Commands.h"

namespace bf {

class Interpreter {
    public:
        Interpreter(int positiveMem = 30000, int negativeMem = 0);
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
        char* _mem;
        char* _memToOrigin;
};

} //namespace bf

#endif

