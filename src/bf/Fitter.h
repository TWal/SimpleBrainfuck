#ifndef BF_FITTER_H
#define BF_FITTER_H

#include <string>

namespace bf {

class Fitter {
    public:
        Fitter();
        int fit(const std::string& bfCode, const std::string& image, std::string& result, const std::string& charsToReplace = "#");

    private:
};

} //namespace bf

#endif

