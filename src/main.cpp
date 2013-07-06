#include <iostream>
#include <fstream>
#include <unordered_set>
#include <tuple>

#include "bf/Parser.h"
#include "bf/Resrap.h"
#include "bf/Interpreter.h"

#include "sbf/Compiler.h"

int main(int argc, char** argv) {
    std::unordered_set<std::string> programs({"compile", "strip", "run"});
    std::vector<std::tuple<std::string, int, char**>> args({std::make_tuple("__prepending__", 0, &argv[1])});
    for(int i = 1; i < argc; ++i) {
        if(programs.find(argv[i]) != programs.end()) {
            args.push_back(std::make_tuple(argv[i], 0, &argv[i+1]));
        } else {
            std::get<1>(args.back())++;
        }
    }

    std::string str; //The string that will be "piped"

    for(auto arg : args) {
        if(std::get<0>(arg) == "__prepending__") {
            FILE* file = nullptr;
            switch(std::get<1>(arg)) {
                case 0:
                    file = stdin;
                    break;
                case 1:
                    if (!(file = fopen(*std::get<2>(arg), "r"))) {
                        std::cerr << "Cannot open file " << *std::get<2>(arg) << std::endl;
                        return -1;
                    }
                    break;
                default:
                    std::cerr << "Usage: sbf [file] [options]..." << std::endl;
                    return 1;
            }
            char buf[1024];
            while(size_t len = fread(buf, 1, sizeof(buf), file)) {
                str.insert(str.end(), buf, buf + len);
            }
            fclose(file);
        } else if(std::get<0>(arg) == "compile") {
            sbf::Compiler c;
            std::string result;
            c.compileFromString(str, result);
            str = result;
        } else if(std::get<0>(arg) == "strip") {
            bf::Parser p;
            bf::Resrap r;
            std::vector<bf::Command*> cmds;
            p.parse(str, cmds);
            r.esrap(cmds, str);
            for(bf::Command* cmd : cmds) {
                delete cmd;
            }
        } else if(std::get<0>(arg) == "run") {
            bf::Parser parser;
            bf::Interpreter interpreter;

            std::vector<bf::Command*> cmds;
            parser.parse(str, cmds);
            interpreter.interpret(cmds);
            str = "";
            for(bf::Command* cmd : cmds) {
                delete cmd;
            }
        }
    }
    std::cout << str << std::flush;
    return 0;
}

