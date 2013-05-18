#include "BfParser.h"
#include "BfCommands.h"
#include <stack>

BfParser::BfParser() {
}

BfParser::~BfParser() {
}

void BfParser::parse(const std::string& code, std::vector<BfCommand*>& commands) {
    commands.clear();
    std::stack<int> bracketsPos;

    for(auto it = code.cbegin(); it != code.cend(); ++it) {
        switch(*it) {
            case '.': {
                commands.push_back(new BfOutput());
                break;
            } case ',': {
                commands.push_back(new BfInput());
                break;
            } case '<': {
                commands.push_back(new BfPointerLeft());
                break;
            } case '>': {
                commands.push_back(new BfPointerRight());
                break;
            } case '-': {
                commands.push_back(new BfMinus());
                break;
            } case '+': {
                commands.push_back(new BfPlus());
                break;
            } case '[': {
                bracketsPos.push(commands.size());
                commands.push_back(new BfStartWhile());
                break;
            } case ']': {
                int matchingPos = bracketsPos.top();
                ((BfStartWhile*)commands[matchingPos])->matching = commands.size();
                commands.push_back(new BfEndWhile());
                ((BfEndWhile*)commands.back())->matching = matchingPos;
                bracketsPos.pop();
                break;
            }
            default:
                break;
        }
    }
}

