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

