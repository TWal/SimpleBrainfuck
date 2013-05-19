#include "Parser.h"
#include "Commands.h"
#include <stack>

namespace bf {

Parser::Parser() {
}

Parser::~Parser() {
}

void Parser::parse(const std::string& code, std::vector<Command*>& commands, char optimizations) const {
    std::list<Command*> commandList;
    std::stack<StartWhile*> brackets;

    for(char c : code) {
        switch(c) {
            case '.': {
                commandList.push_back(new Output());
                break;
            } case ',': {
                commandList.push_back(new Input());
                break;
            } case '<': {
                commandList.push_back(new PointerLeft());
                break;
            } case '>': {
                commandList.push_back(new PointerRight());
                break;
            } case '-': {
                commandList.push_back(new Minus());
                break;
            } case '+': {
                commandList.push_back(new Plus());
                break;
            } case '[': {
                StartWhile* startWhile = new StartWhile();
                brackets.push(startWhile);
                commandList.push_back(startWhile);
                break;
            } case ']': {
                StartWhile* matching = brackets.top();
                brackets.pop();
                EndWhile* endWhile = new EndWhile();
                ((StartWhile*)matching)->matching = endWhile;
                endWhile->matching = matching;
                commandList.push_back(endWhile);
                break;
            }
            default:
                break;
        }
    }

    if(optimizations & MULTI_ADDS || optimizations & UNROLL_LOOPS) {
        _useMultiAdds(commandList);
    }

    if(optimizations & UNROLL_LOOPS) {
        _unrollLoops(commandList);
    }

    commands.clear();
    commands.reserve(_addWhilePos(commandList)); //Hack: _addWhilePos return the size of the list (because list.size() is O(n), we save some time)
    commands.assign(commandList.begin(), commandList.end());
}

unsigned int Parser::_addWhilePos(std::list<Command*>& commands) const {
    int size = 0;
    for(Command* cmd : commands) {
        switch(cmd->type) {
            case Command::START_WHILE:
                ((StartWhile*)cmd)->position = size;
                break;
            case Command::END_WHILE:
                ((EndWhile*)cmd)->position = size;
                break;
            default:
                break;
        }
        ++size;
    }
    return size;
}

void Parser::_useMultiAdds(std::list<Command*>& commands) const {
    std::unordered_map<int, char> currentAdds;
    int currentRelativePos = 0;

    auto it = commands.begin();
    while(it != commands.end()) {
        bool insertMultiAdd = false;
        bool deleteCurrentInstruction = false;
        switch((*it)->type) {
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
                insertMultiAdd = true;
                break;
            default:
                break;
        }

        if(deleteCurrentInstruction) {
            delete *it;
            it = commands.erase(it);
        }

        if(insertMultiAdd) {
            MultiAdds* cmd = new MultiAdds();
            cmd->adds = currentAdds;
            cmd->pointerShift = currentRelativePos;
            commands.insert(it, cmd);
            currentAdds.clear();
            currentRelativePos = 0;
        }

        if(!deleteCurrentInstruction) {
            ++it;
        }
    }
}

void Parser::_unrollLoops(std::list<Command*>& commands) const {
    auto it = commands.begin();
    while(it != commands.end()) {
        bool unroll = false;
        switch((*it)->type) {
            case Command::START_WHILE: {
                Command* nextCmd = *(++it);
                if(nextCmd->type == Command::MULTI_ADDS && ((MultiAdds*)nextCmd)->pointerShift == 0 && ((MultiAdds*)nextCmd)->adds[0] == -1) {
                    nextCmd = *(++it);
                    if(nextCmd->type == Command::END_WHILE) {
                        unroll = true;
                    }
                    --it;
                }
                --it;
                break;
            }
            default:
                break;
        }

        if(unroll) {
            delete *it; // StartWhile
            it = commands.erase(it);
            Multiplies* cmd = new Multiplies();
            cmd->muls = ((MultiAdds*)*it)->adds;
            cmd->muls.erase(0);
            delete *it; // MultiAdds
            it = commands.erase(it);
            delete *it; // EndWhile
            it = commands.erase(it);
            commands.insert(it, cmd);
        }
        ++it;
    }
}

} //namespace bf

