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

    for(char c : code) {
        switch(c) {
            case '.': {
                commands.push_back(new Output());
                break;
            } case ',': {
                commands.push_back(new Input());
                break;
            } case '<': {
                commands.push_back(new PointerLeft());
                break;
            } case '>': {
                commands.push_back(new PointerRight());
                break;
            } case '-': {
                commands.push_back(new Minus());
                break;
            } case '+': {
                commands.push_back(new Plus());
                break;
            } case '[': {
                StartWhile* startWhile = new StartWhile();
                brackets.push(startWhile);
                commands.push_back(startWhile);
                break;
            } case ']': {
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

    _collapse(commands);
    _clean(commands);

    _unrollLoops(commands);
    _clean(commands);

    _addWhilePos(commands);
}

void Parser::_clean(std::vector<Command*>& commands) const {
    std::vector<Command*> copy = commands;
    commands.clear();
    commands.reserve(copy.size());
    for(Command* cmd : copy) {
        if(cmd->type == Command::NO_OPERATION) {
            delete cmd;
        } else {
            commands.push_back(cmd);
        }
    }
}

void Parser::_clean_hashMap(std::unordered_map<int, int>& map) const {
    for(auto& it : map) {
        if(it.second == 0) {
            map.erase(it.first);
        }
    }
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

void Parser::_collapse(std::vector<Command*>& commands) const {
    std::unordered_map<int, int> currentAdds;
    int currentRelativePos = 0;

    unsigned int i = 0;
    while(i < commands.size()) {
        bool insertCollapse = false;
        bool deleteCurrentInstruction = false;
        switch(commands[i]->type) {
            case Command::POINTER_LEFT:
                --currentRelativePos;
                deleteCurrentInstruction = true;
                break;
            case Command::POINTER_RIGHT:
                ++currentRelativePos;
                deleteCurrentInstruction = true;
                break;
            case Command::MINUS:
                --currentAdds[currentRelativePos];
                deleteCurrentInstruction = true;
                break;
            case Command::PLUS:
                ++currentAdds[currentRelativePos];
                deleteCurrentInstruction = true;
                break;
            case Command::START_WHILE:
            case Command::END_WHILE:
            case Command::OUTPUT:
            case Command::INPUT:
                insertCollapse = true;
                break;
            default:
                break;
        }

        if(deleteCurrentInstruction) {
            delete commands[i];
            commands[i] = new NoOperation();
        }

        if(insertCollapse && i != 0 && commands[i-1]->type == Command::NO_OPERATION) {
            _clean_hashMap(currentAdds);
            if(!currentAdds.empty() || currentRelativePos != 0) {
                Command* cmd;
                if(currentAdds.empty()) {
                    unsigned int j = i;
                    while(commands[--j]->type == Command::NO_OPERATION && j != 0);
                    if(j != 0) {
                        commands[j]->shift = currentRelativePos;
                    } else {
                        cmd = new Collapsed();
                        ((Collapsed*)cmd)->shift = currentRelativePos;
                        delete commands[i-1];
                        commands[i-1] = cmd;
                    }
                } else {
                    cmd = new Collapsed();
                    ((Collapsed*)cmd)->adds = currentAdds;
                    ((Collapsed*)cmd)->shift = currentRelativePos;
                    delete commands[i-1];
                    commands[i-1] = cmd;
                }
                currentAdds.clear();
                currentRelativePos = 0;
            }
        }
        ++i;
    }
}

void Parser::_unrollLoops(std::vector<Command*>& commands) const {
    unsigned int i = 0;
    while(i < commands.size()) {
        bool multiplies = false;
        bool whileShift = false;
        switch(commands[i]->type) {
            case Command::START_WHILE: {
                if(commands[i+1]->type == Command::COLLAPSED && commands[i+1]->shift == 0 && ((Collapsed*)commands[i+1])->adds[0] == -1 && commands[i+2]->type == Command::END_WHILE) {
                    multiplies = true;
                } else if(commands[i]->shift != 0 && commands[i+1]->type == Command::END_WHILE) {
                    whileShift = true;
                }
                break;
            }
            default:
                break;
        }

        if(multiplies) {
            delete commands[i]; // StartWhile
            commands[i] = new NoOperation();
            ++i;
            Multiplies* cmd = new Multiplies();
            cmd->muls = ((Collapsed*)commands[i])->adds;
            cmd->muls.erase(0);
            delete commands[i]; // Collapsed
            commands[i] = new NoOperation();
            ++i;
            cmd->shift = commands[i]->shift;
            delete commands[i]; // EndWhile
            commands[i] = cmd;
        }

        if(whileShift) {
            WhileShift* cmd = new WhileShift();
            cmd->nb = commands[i]->shift;
            delete commands[i]; // StartWhile
            commands[i] = new NoOperation();
            ++i;
            cmd->shift = commands[i]->shift;
            delete commands[i]; // EndWhile
            commands[i] = cmd;
        }
        ++i;
    }
}

} //namespace bf

