#ifndef SBF_MACRO_H
#define SBF_MACRO_H

#include <vector>
#include <string>

namespace sbf {

class Macro {
    public:
        virtual void compute(const std::vector<std::string>& arguments, std::string& output) = 0;
};

} //namespace sbf

#endif

