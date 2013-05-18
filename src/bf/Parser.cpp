#include "Parser.h"
#include "Commands.h"
#include <stack>

namespace bf {

Parser::Parser() {
}

Parser::~Parser() {
}

void Parser::parse(const std::string& code, std::vector<Command*>& commands) {
    commands.clear();
    std::stack<int> bracketsPos;

    for(auto it = code.cbegin(); it != code.cend(); ++it) {
        switch(*it) {
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
                bracketsPos.push(commands.size());
                commands.push_back(new StartWhile());
                break;
            } case ']': {
                int matchingPos = bracketsPos.top();
                ((StartWhile*)commands[matchingPos])->matching = commands.size();
                commands.push_back(new EndWhile());
                ((EndWhile*)commands.back())->matching = matchingPos;
                bracketsPos.pop();
                break;
            }
            default:
                break;
        }
    }
}

} //namespace bf

