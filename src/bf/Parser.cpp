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
    _ultra_clean(commands);
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

void Parser::_ultra_clean(std::vector<Command*>& commands) const {
    for(Command* cmd: commands) {
        switch(cmd->type) {
            case Command::COLLAPSED:
                for(auto& it : ((Collapsed*)cmd)->adds) {
                    if(it.second == 0) {
                        ((Collapsed*)cmd)->adds.erase(it.first);
                    }
                }
                break;
            case Command::MULTIPLIES:
                for(auto& it : ((Multiplies*)cmd)->muls) {
                    if(it.second == 0) {
                        ((Multiplies*)cmd)->muls.erase(it.first);
                    }
                }
                break;
            default:
                break;
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
            delete commands[i-1];
            Collapsed* cmd = new Collapsed();
            cmd->adds = currentAdds;
            cmd->pointerShift = currentRelativePos;
            commands[i-1] = cmd;
            currentAdds.clear();
            currentRelativePos = 0;
        }
        ++i;
    }
}

void Parser::_unrollLoops(std::vector<Command*>& commands) const {
    unsigned int i = 0;
    while(i < commands.size()) {
        bool unroll = false;
        switch(commands[i]->type) {
            case Command::START_WHILE: {
                if(commands[i+1]->type == Command::COLLAPSED && ((Collapsed*)commands[i+1])->pointerShift == 0 && ((Collapsed*)commands[i+1])->adds[0] == -1 && commands[i+2]->type == Command::END_WHILE) {
                    unroll = true;
                }
                break;
            }
            default:
                break;
        }

        if(unroll) {
            delete commands[i]; // StartWhile
            commands[i] = new NoOperation();
            ++i;
            Multiplies* cmd = new Multiplies();
            cmd->muls = ((Collapsed*)commands[i])->adds;
            cmd->muls.erase(0);
            delete commands[i]; // Collapsed
            commands[i] = new NoOperation();
            ++i;
            delete commands[i]; // EndWhile
            commands[i] = cmd;
        }
        ++i;
    }
}

} //namespace bf

