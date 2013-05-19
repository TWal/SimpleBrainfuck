#ifndef BF_PARSER_H
#define BF_PARSER_H

#include <string>
#include <vector>
#include <list>
#include "Commands.h"

namespace bf {

class Parser {
    public:
        Parser();
        ~Parser();
        void parse(const std::string& code, std::vector<Command*>& commands, char optimizations) const;
        enum OPTIMIZATIONS {
            NO_OPTIMIZATION,
            MULTI_ADDS,
            UNROLL_LOOPS,
            FULL_OPTIMIZATION = MULTI_ADDS | UNROLL_LOOPS
        };
    private:
        unsigned int _addWhilePos(std::list<Command*>& commands) const;
        void _useMultiAdds(std::list<Command*>& commands) const;
        void _unrollLoops(std::list<Command*>& commands) const;
};

} //namespace bf

#endif

