#ifndef BF_COMMANDS_H
#define BF_COMMANDS_H

#include <unordered_map>

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
        END_WHILE,
        MULTI_ADDS
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

struct MultiAdds : public Command {
    MultiAdds();
    int pointerShift;
    std::unordered_map<int, char> adds;
};

} //namespace bf

#endif

