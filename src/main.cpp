#include <iostream>
#include <fstream>

#include "bf/Parser.h"
#include "bf/Interpreter.h"

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

    bf::Parser parser;
    bf::Interpreter interpreter;

    std::vector<bf::Command*> cmds;
    parser.parse(bfprog, cmds, bf::Parser::FULL_OPTIMIZATION);
    interpreter.interpret(cmds);

    return 0;
}

