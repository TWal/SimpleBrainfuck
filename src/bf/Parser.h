#ifndef BF_PARSER_H
#define BF_PARSER_H

#include <string>
#include <vector>
#include "Commands.h"

namespace bf {

class Parser {
    public:
        Parser();
        ~Parser();
        void parse(const std::string& code, std::vector<Command*>& commands);
};

} //namespace bf

#endif

