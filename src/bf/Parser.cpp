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

#include "Parser.h"
#include "Commands.h"
#include <stack>

namespace bf {

Parser::Parser() {
}

Parser::~Parser() {
}

void Parser::parse(const std::string& code, std::vector<Command*>& commands) const {
    commands.clear();
    commands.reserve(code.size());
    std::stack<StartWhile*> brackets;
    std::unordered_map<int, int> currentAdds;
    int currentShift = 0;

    for(char c : code) {
        switch(c) {
            case '.': {
                _addAdds(currentAdds, currentShift, commands);
                commands.push_back(new Output());
                break;
            } case ',': {
                _addAdds(currentAdds, currentShift, commands);
                commands.push_back(new Input());
                break;
            } case '<': {
                --currentShift;
                break;
            } case '>': {
                ++currentShift;
                break;
            } case '-': {
                if(currentAdds.count(currentShift)) {
                    --currentAdds[currentShift];
                } else {
                    currentAdds[currentShift] = -1;
                }
                break;
            } case '+': {
                if(currentAdds.count(currentShift)) {
                    ++currentAdds[currentShift];
                } else {
                    currentAdds[currentShift] = 1;
                }
                break;
            } case '[': {
                _addAdds(currentAdds, currentShift, commands);
                StartWhile* startWhile = new StartWhile();
                brackets.push(startWhile);
                commands.push_back(startWhile);
                break;
            } case ']': {
                _addAdds(currentAdds, currentShift, commands);
                StartWhile* matching = brackets.top();
                brackets.pop();
                EndWhile* endWhile = new EndWhile();
                ((StartWhile*)matching)->matching = endWhile;
                endWhile->matching = matching;
                commands.push_back(endWhile);
                break;
            }
            default:
                break;
        }
    }

    _unrollLoops(commands);
    _addWhilePos(commands);
}

void Parser::_addAdds(std::unordered_map<int, int>& adds, int& shift, std::vector<Command*>& commands) const {
    Adds* cmd = new Adds();
    cmd->adds = adds;
    cmd->shift = shift;

    auto it = cmd->adds.begin();
    while(it != cmd->adds.end()) {
        if(it->second == 0) {
            it = cmd->adds.erase(it);
        } else {
            ++it;
        }
    }

    if(cmd->adds.size() != 0 || shift != 0) {
        commands.push_back(cmd);
    } else {
        delete cmd;
    }

    adds.clear();
    shift = 0;
}

void Parser::_addWhilePos(std::vector<Command*>& commands) const {
    for(unsigned int i = 0; i < commands.size(); ++i) {
        switch(commands[i]->type) {
            case Command::START_WHILE:
                ((StartWhile*)commands[i])->position = i;
                break;
            case Command::END_WHILE:
                ((EndWhile*)commands[i])->position = i;
                break;
            default:
                break;
        }
    }
}

void Parser::_unrollLoops(std::vector<Command*>& commands) const {
    std::vector<size_t> toRemove;
    size_t i = 0;
    while(i < commands.size()) {
        if(commands[i]->type == Command::START_WHILE &&
           commands[i+1]->type == Command::ADDS &&
           commands[i+2]->type == Command::END_WHILE) {
            if(((Adds*)commands[i+1])->shift == 0 && ((Adds*)commands[i+1])->adds[0] == -1) {
                Multiplies* mult = new Multiplies();
                mult->muls = ((Adds*)commands[i+1])->adds;
                mult->muls.erase(0);
                delete commands[i];
                delete commands[i+1];
                delete commands[i+2];
                commands[i] = mult;
                toRemove.push_back(i+1);
                toRemove.push_back(i+2);
                i += 2;
            } else if(((Adds*)commands[i+1])->adds.size() == 0 && ((Adds*)commands[i+1])->shift != 0) {
                WhileShift* whshift = new WhileShift();
                whshift->nb = ((Adds*)commands[i+1])->shift;
                delete commands[i];
                delete commands[i+1];
                delete commands[i+2];
                commands[i] = whshift;
                toRemove.push_back(i+1);
                toRemove.push_back(i+2);
                i += 2;
            }
        }
        ++i;
    }

    if(toRemove.size() != 0) {
        std::vector<Command*> newCommands;
        commands.swap(newCommands);
        commands.reserve(newCommands.size());
        size_t j = 0;
        for(size_t i = 0; i < newCommands.size(); ++i) {
            if(j < toRemove.size() && i == toRemove[j]) {
                ++j;
            } else {
                commands.push_back(newCommands[i]);
            }
        }
    }
}

} //namespace bf

