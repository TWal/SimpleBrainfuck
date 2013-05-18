#include <iostream>
#include <fstream>

#include "BfParser.h"
#include "BfInterpreter.h"

int main(int argc, char** argv) {
    if(argc <= 1) {
        std::cerr << "Usage: ./sbf program.bf" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);

    if(!file.is_open()) {
        std::cerr << "Cannot open file " << argv[1] << std::endl;
        return 2;
    }

    std::string bfprog;

    while(file.good()) {
        bfprog.push_back(file.get());
    }

    file.close();

    BfParser parser;
    BfInterpreter interpreter;

    std::vector<BfCommand*> cmds;
    parser.parse(bfprog, cmds);
    interpreter.interpret(cmds);

    return 0;
}

