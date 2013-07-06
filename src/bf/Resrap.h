#ifndef BF_RESRAP_H
#define BF_RESRAP_H

#include <string>
#include <vector>
#include "Commands.h"

namespace bf {

class Resrap {
    public:
        Resrap();
        void esrap(const std::vector<Command*>& commands, std::string& code) const;

    private:
        void _mapToBf(const std::unordered_map<int, int>& map, int finalShift, std::string& result) const;
        void _addShifts(int shift, std::string& result) const;
        void _addPlusMinus(int n, std::string& result) const;
};

} //namespace bf

#endif

