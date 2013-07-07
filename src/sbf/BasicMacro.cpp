/* Copyright (c) 2013, Theophile Wallez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
