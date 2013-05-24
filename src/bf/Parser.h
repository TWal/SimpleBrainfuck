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
        void parse(const std::string& code, std::vector<Command*>& commands) const;
    private:
        void _clean(std::vector<Command*>& commands) const;
        void _clean_hashMap(std::unordered_map<int, int>& map) const;
        void _addWhilePos(std::vector<Command*>& commands) const;
        void _collapse(std::vector<Command*>& commands) const;
        void _unrollLoops(std::vector<Command*>& commands) const;
};

} //namespace bf

#endif

