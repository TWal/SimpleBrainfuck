#ifndef SBF_BASICMACRO_H
#define SBF_BASICMACRO_H

#include "Macro.h"
#include <vector>
#include <string>

namespace sbf {

class BasicMacro : public Macro {
    public:
        BasicMacro();
        BasicMacro(const std::vector<std::string>& argNames, const std::string& macro);
        virtual void compute(const std::vector<std::string>& arguments, std::string& output);

    private:
        std::vector<std::string> _argNames;
        std::vector<std::string> _macro;
};

} //namespace sbf

#endif

