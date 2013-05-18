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
    int size = 0;

    for(char c : code) {
        switch(c) {
            case '.': {
                commandList.push_back(new Output());
                ++size;
                break;
            } case ',': {
                commandList.push_back(new Input());
                ++size;
                break;
            } case '<': {
                commandList.push_back(new PointerLeft());
                ++size;
                break;
            } case '>': {
                commandList.push_back(new PointerRight());
                ++size;
                break;
            } case '-': {
                commandList.push_back(new Minus());
                ++size;
                break;
            } case '+': {
                commandList.push_back(new Plus());
                ++size;
                break;
            } case '[': {
                StartWhile* startWhile = new StartWhile();
                brackets.push(startWhile);
                startWhile->position = size;
                commandList.push_back(startWhile);
                ++size;
                break;
            } case ']': {
                StartWhile* matching = brackets.top();
                brackets.pop();
                EndWhile* endWhile = new EndWhile();
                ((StartWhile*)matching)->matching = endWhile;
                endWhile->matching = matching;
                endWhile->position = size;
                commandList.push_back(endWhile);
                ++size;
                break;
            }
            default:
                break;
        }
    }

    if(optimizations & MULTI_ADDS) {
        _useMultiAdds(commandList);
    }

    commands.clear();
    commands.assign(commandList.begin(), commandList.end());
}

void Parser::_useMultiAdds(std::list<Command*>& commands) const {
    std::unordered_map<int, char> currentAdds;
    int currentRelativePos = 0;
    int removedSoFar = 0;
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
                ((StartWhile*)*it)->position -= removedSoFar - 1;
                insertMultiAdd = true;
                break;
            case Command::END_WHILE:
                ((EndWhile*)*it)->position -= removedSoFar - 1;
                insertMultiAdd = true;
                break;
            case Command::OUTPUT:
            case Command::INPUT:
                insertMultiAdd = true;
                break;
            default:
                break;
        };

        if(deleteCurrentInstruction) {
            delete *it;
            it = commands.erase(it);
            ++removedSoFar;
        }

        if(insertMultiAdd) {
            MultiAdds* cmd = new MultiAdds();
            cmd->adds = currentAdds;
            cmd->pointerShift = currentRelativePos;
            commands.insert(it, cmd);
            currentAdds.clear();
            currentRelativePos = 0;
            --removedSoFar;
        }

        if(!deleteCurrentInstruction) {
            ++it;
        }
    }
}

} //namespace bf

