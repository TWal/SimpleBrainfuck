#ifndef BF_COMMANDS_H
#define BF_COMMANDS_H

#include <unordered_map>

namespace bf {

struct Command {
    enum COMMAND_TYPE {
        NO_OPERATION,
        OUTPUT,
        INPUT,
        POINTER_LEFT,
        POINTER_RIGHT,
        MINUS,
        PLUS,
        START_WHILE,
        END_WHILE,
        COLLAPSED,
        MULTIPLIES
    };

    Command(COMMAND_TYPE t);

    COMMAND_TYPE type;
    int shift;
};

struct NoOperation : public Command {
    NoOperation();
};

struct Output : public Command {
    Output();
};

struct Input : public Command {
    Input();
};

struct PointerLeft : public Command {
    PointerLeft();
};

struct PointerRight : public Command {
    PointerRight();
};

struct Minus : public Command {
    Minus();
};

struct Plus : public Command {
    Plus();
};

struct EndWhile;
struct StartWhile : public Command {
    StartWhile();
    int position;
    EndWhile* matching;
};

struct EndWhile : public Command {
    EndWhile();
    int position;
    StartWhile* matching;
};

struct Collapsed : public Command {
    Collapsed();
    std::unordered_map<int, int> adds;
};

struct Multiplies : public Command {
    Multiplies();
    std::unordered_map<int, int> muls;
};

} //namespace bf

#endif

