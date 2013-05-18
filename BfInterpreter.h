#ifndef BFINTERPRETER_H
#define BFINTERPRETER_H

#include <vector>
#include "BfCommands.h"

class BfInterpreter {
    public:
        BfInterpreter();
        ~BfInterpreter();
        void interpret(const std::vector<BfCommand*>& commands);
    private:
        char& _memPos(int pos);
        std::vector<char> _negMem;
        std::vector<char> _posMem;
};

#endif

