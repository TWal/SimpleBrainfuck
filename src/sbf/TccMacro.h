#ifndef SBF_TCCMACRO_H
#define SBF_TCCMACRO_H

#include <string>
#include <vector>

#include "Macro.h"

namespace sbf {

class TccMacro : public Macro {
    public:
        TccMacro(const std::string& code);
        virtual ~TccMacro();
        virtual void compute(const std::vector<std::string>& arguments, std::string& output);
        static void push(const char* str);

    private:
        int(*_func)(int, const char**);
        void* _mem;
        static std::string _result;
};

} //namespace sbf

#endif

