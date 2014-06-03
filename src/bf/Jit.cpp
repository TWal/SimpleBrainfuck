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

#include "Jit.h"
#include <cstring>
#include <cstdio>
#include <stack>
#include <list>

static char getCharFromIstream(std::istream* s) {
    int c = s->get();
    return (c == EOF ? 0 : c);
}

namespace bf {

Jit::Jit(std::istream* is, int cellSize, int positiveMem, int negativeMem) {
    _is = is;
    _cellSize = cellSize;
    _memToOrigin = new char[(positiveMem + negativeMem) * cellSize];
    _mem = _memToOrigin + negativeMem;
    memset(_memToOrigin, 0, (positiveMem + negativeMem) * cellSize);
    _context = jit_context_create();
}

Jit::~Jit() {
    delete _memToOrigin;
    jit_context_destroy(_context);
}

void Jit::compile(const std::vector<Command*>& commands) {
    jit_type_t cellType;
    switch(_cellSize) {
        case 4:
            cellType = jit_type_uint;
            break;
        case 2:
            cellType = jit_type_ushort;
            break;
        default:
            cellType = jit_type_ubyte;
            _cellSize = 1;
            break;
    }
    jit_context_build_start(_context);
    jit_type_t cellPtr = jit_type_create_pointer(cellType, 1);
    jit_type_t params[2] = {cellPtr, jit_type_void_ptr};
    jit_type_t signature = jit_type_create_signature(jit_abi_cdecl, jit_type_void, params, 2, 1);
    _func = jit_function_create(_context, signature);

    jit_type_t putcharParams[1] = {jit_type_sys_int};
    jit_type_t putcharSig = jit_type_create_signature(jit_abi_cdecl, jit_type_void, putcharParams, 1, 1);

    jit_type_t getcharParams[1] = {jit_type_void_ptr};
    jit_type_t getcharSig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_char, getcharParams, 1, 1);

    jit_value_t ptr = jit_value_get_param(_func, 0);
    jit_value_t istreamPtr = jit_value_get_param(_func, 1);
    jit_value_t tmp;
    jit_value_t tmp2;
    jit_value_t tmp3;
    jit_value_t cst;
    std::list<jit_label_t> labels;
    std::stack<std::pair<jit_label_t*, jit_label_t*>> loopStack;

    for(Command* cmd : commands) {
        switch(cmd->type) {
            case Command::OUTPUT: {
                tmp = jit_insn_load_relative(_func, ptr, 0, cellType);
                jit_insn_call_native(_func, NULL, (void*)putchar, putcharSig, &tmp, 1, JIT_CALL_NOTHROW);
            }
                break;
            case Command::INPUT:
                tmp = jit_insn_call_native(_func, NULL, (void*)getCharFromIstream, getcharSig, &istreamPtr, 1, JIT_CALL_NOTHROW);
                jit_insn_store_relative(_func, ptr, 0, tmp);
                break;
            case Command::START_WHILE: {
                labels.push_back(jit_label_undefined);
                jit_label_t* labelStart = &labels.back();
                labels.push_back(jit_label_undefined);
                jit_label_t* labelEnd = &labels.back();
                loopStack.push(std::make_pair(labelStart, labelEnd));
                tmp = jit_insn_load_relative(_func, ptr, 0, cellType);
                jit_insn_branch_if_not(_func, tmp, labelEnd);
                jit_insn_label(_func, labelStart);
                break;
            }
            case Command::END_WHILE: {
                tmp = jit_insn_load_relative(_func, ptr, 0, cellType);
                jit_insn_branch_if(_func, tmp, loopStack.top().first);
                jit_insn_label(_func, loopStack.top().second);
                loopStack.pop();
                break;
            }
            case Command::ADDS : {
                Adds* adds = (Adds*)cmd;
                for(const auto& it : adds->adds) {
                    tmp = jit_insn_load_relative(_func, ptr, it.first * _cellSize, cellType);
                    cst = jit_value_create_nint_constant(_func, cellType, it.second);
                    tmp = jit_insn_add(_func, tmp, cst);
                    tmp = jit_insn_convert(_func, tmp, cellType, 0);
                    jit_insn_store_relative(_func, ptr, it.first * _cellSize, tmp);
                }
                tmp = jit_insn_add_relative(_func, ptr, adds->shift * _cellSize);
                jit_insn_store(_func, ptr, tmp);
                break;
            }
            case Command::MULTIPLIES: {
                Multiplies* muls = (Multiplies*)cmd;
                tmp2 = jit_insn_load_relative(_func, ptr, 0, cellType);
                for(const auto& it : muls->muls) {
                    cst = jit_value_create_nint_constant(_func, cellType, it.second);
                    tmp = jit_insn_mul(_func, tmp2, cst);
                    tmp3 = jit_insn_load_relative(_func, ptr, it.first * _cellSize, cellType);
                    tmp3 = jit_insn_add(_func, tmp3, tmp);
                    tmp3 = jit_insn_convert(_func, tmp3, cellType, 0);
                    jit_insn_store_relative(_func, ptr, it.first * _cellSize, tmp3);
                }
                cst = jit_value_create_nint_constant(_func, cellType, 0);
                jit_insn_store_relative(_func, ptr, 0, cst);
                break;
            }
            case Command::WHILE_SHIFT: {
                labels.push_back(jit_label_undefined);
                jit_label_t* labelStart = &labels.back();
                labels.push_back(jit_label_undefined);
                jit_label_t* labelEnd = &labels.back();
                tmp = jit_insn_load_relative(_func, ptr, 0, cellType);
                jit_insn_branch_if_not(_func, tmp, labelEnd);
                jit_insn_label(_func, labelStart);
                tmp = jit_insn_add_relative(_func, ptr, ((WhileShift*)cmd)->nb * _cellSize);
                jit_insn_store(_func, ptr, tmp);
                tmp = jit_insn_load_relative(_func, ptr, 0, cellType);
                jit_insn_branch_if(_func, tmp, labelStart);
                jit_insn_label(_func, labelEnd);
                break;
            }
            default:
                break;
        }
    }
    jit_insn_return(_func, NULL);
    jit_function_compile(_func);
    jit_context_build_end(_context);
}

void Jit::run() {
    void* args[2] = {&_mem, &_is};
    jit_function_apply(_func, args, NULL);
}


}

