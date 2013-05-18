#ifndef BFPARSER_H
#define BFPARSER_H

#include <string>
#include <vector>
#include "BfCommands.h"

class BfParser {
    public:
        BfParser();
        ~BfParser();
        void parse(const std::string& code, std::vector<BfCommand*>& commands);
};

#endif

