#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <tuple>
#include <getopt.h>

#include "bf/Parser.h"
#include "bf/Resrap.h"
#include "bf/Interpreter.h"
#include "sbf/Compiler.h"
#include "bf/Fitter.h"

void prepending(int argc, char** argv, std::string& inout);
void compile(int argc, char** argv, std::string& inout);
void strip(int argc, char** argv, std::string& inout);
void fit(int argc, char** argv, std::string& inout);
void run(int argc, char** argv, std::string& inout);
void writeOut(const std::string& str, const std::string& filename);
bool has(const std::string& str, char c);

int main(int argc, char** argv) {
    std::unordered_set<std::string> programs({"compile", "strip", "fit", "run"});
    std::vector<std::tuple<std::string, int, char**>> args({std::make_tuple("__prepending__", 1, &argv[0])});
    for(int i = 1; i < argc; ++i) {
        if(programs.find(argv[i]) != programs.end()) {
            args.push_back(std::make_tuple(argv[i], 1, &argv[i]));
        } else {
            std::get<1>(args.back())++;
        }
    }

    std::string str; //The string that will be "piped"
    for(auto arg : args) {
        optind = 1;
        if(std::get<0>(arg) == "__prepending__") {
            prepending(std::get<1>(arg), std::get<2>(arg), str);
        } else if(std::get<0>(arg) == "compile") {
            compile(std::get<1>(arg), std::get<2>(arg), str);
        } else if(std::get<0>(arg) == "strip") {
            strip(std::get<1>(arg), std::get<2>(arg), str);
        } else if(std::get<0>(arg) == "fit") {
            fit(std::get<1>(arg), std::get<2>(arg), str);
        } else if(std::get<0>(arg) == "run") {
            run(std::get<1>(arg), std::get<2>(arg), str);
        }
    }
    std::cout << str << std::flush;
    return 0;
}

void prepending(int argc, char** argv, std::string& inout) {
    FILE* file = nullptr;
    switch(argc) {
        case 1:
            file = stdin;
            break;
        case 2:
            if (!(file = fopen(argv[1], "r"))) {
                std::cerr << "Cannot open file " << argv[1] << std::endl;
                exit(-1);
            }
            break;
        default:
            std::cerr << "Usage: sbf [file] [options]..." << std::endl;
            exit(1);
    }
    char buf[1024];
    while(size_t len = fread(buf, 1, sizeof(buf), file)) {
        inout.insert(inout.end(), buf, buf + len);
    }
    fclose(file);
}

void compile(int argc, char** argv, std::string& inout) {
    struct {
        std::string opts;
        std::string outputFile;
    } opts = {"", ""};
    while(true) {
        static option longOptions[] = {
            {"output", required_argument, 0, 'o'}
        };
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }
        opts.opts.push_back(c);
        switch(c) {
            case 'o':
                opts.outputFile = optarg;
                break;
            default:
                break;
        }
    }

    sbf::Compiler c;
    std::string result;
    c.compileFromString(inout, result);
    inout = result;
    if(has(opts.opts, 'o')) {
        writeOut(inout, opts.outputFile);
    }
}

void strip(int argc, char** argv, std::string& inout) {
    struct {
        std::string opts;
        std::string outputFile;
    } opts = {"", ""};
    while(true) {
        static option longOptions[] = {
            {"output", required_argument, 0, 'o'},
            {"stats", no_argument, 0, 's'}
        };
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:s", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }
        opts.opts.push_back(c);
        switch(c) {
            case 'o':
                opts.outputFile = optarg;
                break;
            default:
                break;
        }
    }

    size_t oldSize = inout.size();
    if(has(opts.opts, 's')) {
        std::cerr << "Before: " << inout.size() << std::endl;
    }
    bf::Parser p;
    bf::Resrap r;
    std::vector<bf::Command*> cmds;
    p.parse(inout, cmds);
    r.esrap(cmds, inout);
    for(bf::Command* cmd : cmds) {
        delete cmd;
    }
    if(has(opts.opts, 's')) {
        std::cerr << "After: " << inout.size() << " (" << ((float)inout.size()/oldSize)*100 << "%, -" << oldSize - inout.size() << "ops)" << std::endl;
    }
    if(has(opts.opts, 'o')) {
        writeOut(inout, opts.outputFile);
    }
}

void fit(int argc, char** argv, std::string& inout) {
    struct {
        std::string opts;
        std::string outputFile;
        std::string imageFile;
    } opts = {"", "", ""};
    while(true) {
        static option longOptions[] = {
            {"output", required_argument, 0, 'o'},
            {"image", required_argument, 0, 'i'}
        };
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:i:", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }
        opts.opts.push_back(c);
        switch(c) {
            case 'o':
                opts.outputFile = optarg;
                break;
            case 'i':
                opts.imageFile = optarg;
                break;
            default:
                break;
        }
    }

    if(!has(opts.opts, 'i')) {
        std::cerr << "You must specify an image file" << std::endl;
        exit(-1);
    }
    bf::Fitter f;
    std::string image;
    std::string result;
    if (FILE *fp = fopen(opts.imageFile.data(), "r")) {
        char buf[1024];
        while(size_t len = fread(buf, 1, sizeof(buf), fp)) {
            image.insert(image.end(), buf, buf + len);
        }
        fclose(fp);
        f.fit(inout, image, result);
        inout = result;
    } else {
        std::cerr << "Cannot open file '" << opts.imageFile << "'" << std::endl;
    }
    if(has(opts.opts, 'o')) {
        writeOut(inout, opts.outputFile);
    }
}

void run(int argc, char** argv, std::string& inout) {
    struct {
        std::string opts;
        std::string inputDelimiter;
        int positiveMem;
        int negativeMem;
    } opts = {"", "", 30000, 30000};
    while(true) {
        static option longOptions[] = {
            {"delimiter", required_argument, 0, 'd'},
            {"positive-memory", required_argument, 0, 'p'},
            {"negative-memory", required_argument, 0, 'n'}
        };
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "d:p:n:", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }
        opts.opts.push_back(c);
        switch(c) {
            case 'd':
                opts.inputDelimiter = optarg;
                break;
            case 'p':
                opts.positiveMem = atoi(optarg);
                break;
            case 'n':
                opts.negativeMem = atoi(optarg);
                break;
            default:
                break;
        }
    }

    size_t delimPos = inout.find(opts.inputDelimiter);
    bool hasDelimiter = has(opts.opts, 'd') && delimPos != std::string::npos;

    bf::Parser parser;
    std::istream* in = hasDelimiter ? new std::istringstream(inout.substr(delimPos + opts.inputDelimiter.size())) : &std::cin;
    bf::Interpreter interpreter(in, opts.positiveMem, opts.negativeMem);
    std::vector<bf::Command*> cmds;

    parser.parse(hasDelimiter ? inout.substr(0, delimPos) : inout, cmds);
    interpreter.interpret(cmds);

    inout = "";
    if(hasDelimiter) {
        delete in;
    }
    for(bf::Command* cmd : cmds) {
        delete cmd;
    }
}

void writeOut(const std::string& str, const std::string& filename) {
    FILE* file = fopen(filename.data(), "w+");
    if(!file) {
        std::cerr << "Cannot write to file '" << file << "'" << std::endl;
        return;
    }
    fputs(str.data(), file);
    fclose(file);
}

bool has(const std::string& str, char c) {
    return str.find_first_of(std::string(&c, 1)) != std::string::npos;
}

