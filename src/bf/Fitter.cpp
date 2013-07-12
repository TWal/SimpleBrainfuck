#include "Fitter.h"
#include <iostream>
#include <random>
#include <functional>
#include "Parser.h"
#include "Resrap.h"

namespace bf {

Fitter::Fitter() {
}

int Fitter::fit(const std::string& bfCode, const std::string& image, std::string& result, const std::string& charsToReplace) {
    bf::Parser p;
    bf::Resrap r;
    std::vector<bf::Command*> cmds;
    std::string extendedCode;
    p.parse(bfCode, cmds);
    r.esrap(cmds, extendedCode);
    for(bf::Command* cmd : cmds) {
        delete cmd;
    }
    cmds.clear();
    size_t finalCodeSize = 0;
    for(char c : image) {
        if(charsToReplace.find_first_of(std::string(&c, 1)) != std::string::npos) {
            ++finalCodeSize;
        }
    }

    if(extendedCode.size() > finalCodeSize) {
        std::cerr << "The given code is too big" << std::endl;
        return finalCodeSize - extendedCode.size();
    }
    std::string chars0to2 = "++";
    std::string chars3to8 = "[-]>+>++";
    std::string chars9toN = "[-]>+<";
    size_t nbCharToAdd = finalCodeSize - extendedCode.size();
    switch(nbCharToAdd) {
        case 0:
        case 1:
        case 2:
            extendedCode += chars0to2.substr(0, nbCharToAdd);
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            extendedCode += chars3to8.substr(0, nbCharToAdd);
            break;
        default:
            std::uniform_int_distribution<int> distribution(0,3);
            std::mt19937 engine;
            std::hash<std::string> strHash;
            engine.seed(strHash(extendedCode));
            auto generator = std::bind(distribution, engine);

            extendedCode += chars9toN;
            extendedCode.push_back('[');
            nbCharToAdd -= 8;
            bool doLoop = false;
            while(nbCharToAdd != 0) {
                std::string codeToStrip;
                if(doLoop && nbCharToAdd > 2) {
                    codeToStrip.push_back('[');
                    for(size_t i = 0; i < nbCharToAdd-2; ++i) {
                        codeToStrip.push_back("><+-"[generator()]);
                    }
                    codeToStrip.push_back(']');
                } else {
                    for(size_t i = 0; i < nbCharToAdd; ++i) {
                        codeToStrip.push_back("><+-"[generator()]);
                    }
                    codeToStrip.push_back('.');
                }
                p.parse(codeToStrip, cmds);
                r.esrap(cmds, codeToStrip);
                for(bf::Command* cmd : cmds) {
                    delete cmd;
                }
                cmds.clear();
                extendedCode += codeToStrip;
                nbCharToAdd -= codeToStrip.size();
                if(extendedCode.back() == '.') {
                    extendedCode.erase(extendedCode.size()-1);
                    ++nbCharToAdd;
                }
                doLoop = !doLoop;
            }
            extendedCode.push_back(']');
            break;
    }

    result.clear();
    int i = 0;
    for(char c : image) {
        if(charsToReplace.find_first_of(std::string(&c, 1)) != std::string::npos) {
            result.push_back(extendedCode[i++]);
        } else {
            result.push_back(c);
        }
    }
    return finalCodeSize - extendedCode.size();
}

} //namespace bf

