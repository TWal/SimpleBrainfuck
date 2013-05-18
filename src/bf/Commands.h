#ifndef BF_COMMANDS_H
#define BF_COMMANDS_H

#include <vector>

namespace bf {

struct Command {
    enum COMMAND_TYPE {
        OUTPUT,
        INPUT,
        POINTER_LEFT,
        POINTER_RIGHT,
        MINUS,
        PLUS,
        START_WHILE,
        END_WHILE
    };

    Command(COMMAND_TYPE t);

    COMMAND_TYPE type;
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

struct StartWhile : public Command {
    StartWhile();
    int matching;
};

struct EndWhile : public Command {
    EndWhile();
    int matching;
};

} //namespace bf

#endif

