#include "BasicMacro.h"

#include <iostream>
#include <algorithm>

namespace sbf {

BasicMacro::BasicMacro() {
}

BasicMacro::BasicMacro(const std::vector<std::string>& argNames, const std::string& macro) {
    _argNames = argNames;

    std::vector<std::string> sortedArgNames = argNames;
    std::sort(sortedArgNames.begin(), sortedArgNames.end(), [](const std::string& a, const std::string& b) {return a.size() > b.size();});
    unsigned int lastCut = 0;
    for(unsigned int i = 0; i < macro.size(); ++i) {
        for(const std::string& name : sortedArgNames) {
            bool cut = true;
            for(unsigned int j = 0; j < name.size(); ++j) {
                if(i + j >= macro.size() || macro[i+j] != name[j]) {
                    cut = false;
                    break;
                }
            }
            if(cut) {
                if(i != lastCut) {
                    _macro.push_back(macro.substr(lastCut, i - lastCut));
                }
                _macro.push_back(macro.substr(i, name.size()));
                i += name.size() - 1;
                lastCut = i + 1;
            }
        }
    }
    if(lastCut != macro.size()) {
        _macro.push_back(macro.substr(lastCut));
    }
}

void BasicMacro::compute(const std::vector<std::string>& arguments, std::string& output) {
    output.clear();
    for(const std::string& m : _macro) {
        bool isArgName = false;
        for(unsigned int i = 0; i < _argNames.size(); ++i) {
            if(m == _argNames[i]) {
                output += arguments[i];
                isArgName = true;
                break;
            }
        }
        if(!isArgName) {
            output += m;
        }
    }
}

} //namespace sbf
