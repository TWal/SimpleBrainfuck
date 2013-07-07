#include "TccMacro.h"
#include <libtcc.h>
#include <iostream>

namespace sbf {

TccMacro::TccMacro(const std::string& code) {
    std::vector<std::string> blacklistedKeywords({"fopen", "tmpfile", "fprintf", "fwrite", "fputc", "fputs", "ungetc", "#include"});
    for(std::string& keyword : blacklistedKeywords) {
        if(code.find(keyword) != std::string::npos) {
            std::cerr << "Keyword '" << keyword << "' is blacklisted" << std::endl;
            return;
        }
    }

    std::string fullCode = "void f(int argc, const char** argv) {" + code + "}";

    TCCState* s = tcc_new();
    if(!s) {
        std::cerr << "Cannot create TCC context" << std::endl;
        return;
    }
    void(*pushFunc)(const char*) = &TccMacro::push;
    tcc_add_symbol(s, "push", *(void**)&pushFunc); //Meh.
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    if(tcc_compile_string(s, fullCode.data()) != 0) {
        std::cerr << "Compilation error" << std::endl;
        return;
    }

    int size = tcc_relocate(s, NULL);
    if(size == -1) {
        return;
    }

    _mem = malloc(size);
    tcc_relocate(s, _mem);
    *(void**)&_func = tcc_get_symbol(s, "f");

    tcc_delete(s);
}

TccMacro::~TccMacro() {
    free(_mem);
}

void TccMacro::compute(const std::vector<std::string>& arguments, std::string& output) {
    _result.clear();
    int argc = arguments.size();
    const char** argv = new const char*[argc];
    for(int i = 0; i < argc; ++i) {
        argv[i] = arguments[i].data();
    }
    _func(argc, argv);
    output = _result;
}

void TccMacro::push(const char* str) {
    _result += str;
}

std::string TccMacro::_result = "";

}
