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

#include "Resrap.h"

#include <map>

namespace bf {

Resrap::Resrap() {
}

void Resrap::esrap(const std::vector<Command*>& commands, std::string& code) const {
    code.clear();
    for(Command* cmd : commands) {
        switch(cmd->type) {
            case Command::OUTPUT:
                code.push_back('.');
                break;
            case Command::INPUT:
                code.push_back(',');
                break;
            case Command::POINTER_LEFT:
                code.push_back('<');
                break;
            case Command::POINTER_RIGHT:
                code.push_back('>');
                break;
            case Command::MINUS:
                code.push_back('-');
                break;
            case Command::PLUS:
                code.push_back('+');
                break;
            case Command::START_WHILE:
                code.push_back('[');
                break;
            case Command::END_WHILE:
                code.push_back(']');
                break;
            case Command::COLLAPSED:
                _mapToBf(((Collapsed*)(cmd))->adds, cmd->shift, code);
                break;
            case Command::MULTIPLIES:
                code.push_back('[');
                _mapToBf(((Collapsed*)(cmd))->adds, 0, code);
                code.push_back('-');
                code.push_back(']');
                break;
            case Command::WHILE_SHIFT:
                code.push_back('[');
                _addShifts(((WhileShift*)(cmd))->nb, code);
                code.push_back(']');
                break;
            default:
                break;
        }
        if(cmd->type != Command::COLLAPSED) {
            _addShifts(cmd->shift, code);
        }
    }
}

void Resrap::_mapToBf(const std::unordered_map<int, int>& map, int finalShift, std::string& result) const {
    std::map<int, int> sortedMap;
    for(auto& it : map) {
        sortedMap[it.first] = it.second;
    }

    int lastPos = 0;
    if(finalShift >= 0) {
        for(auto& it : sortedMap) {
            _addShifts(it.first - lastPos, result);
            _addPlusMinus(it.second, result);
            lastPos = it.first;
        }
    } else {
        for(auto it = sortedMap.rbegin(); it != sortedMap.rend(); ++it) {
            _addShifts(it->first - lastPos, result);
            _addPlusMinus(it->second, result);
            lastPos = it->first;
        }
    }
    _addShifts(finalShift-lastPos, result);
}

void Resrap::_addShifts(int shift, std::string& result) const {
    if(shift > 0) {
        for(int i = 0; i < shift; ++i) {
            result.push_back('>');
        }
    } else {
        for(int i = 0; i < -shift; ++i) {
            result.push_back('<');
        }
    }
}

void Resrap::_addPlusMinus(int n, std::string& result) const {
    if(n > 0) {
        for(int i = 0; i < n; ++i) {
            result.push_back('+');
        }
    } else {
        for(int i = 0; i < -n; ++i) {
            result.push_back('-');
        }
    }
}

} //namespace bf

