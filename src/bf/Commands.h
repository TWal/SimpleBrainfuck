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
        MULTIPLIES,
        WHILE_SHIFT
    };

    Command(COMMAND_TYPE t);
    virtual ~Command();

    COMMAND_TYPE type;
    int shift;
};

struct NoOperation : public Command {
    NoOperation();
    virtual ~NoOperation();
};

struct Output : public Command {
    Output();
    virtual ~Output();
};

struct Input : public Command {
    Input();
    virtual ~Input();
};

struct PointerLeft : public Command {
    PointerLeft();
    virtual ~PointerLeft();
};

struct PointerRight : public Command {
    PointerRight();
    virtual ~PointerRight();
};

struct Minus : public Command {
    Minus();
    virtual ~Minus();
};

struct Plus : public Command {
    Plus();
    virtual ~Plus();
};

struct EndWhile;
struct StartWhile : public Command {
    StartWhile();
    virtual ~StartWhile();
    int position;
    EndWhile* matching;
};

struct EndWhile : public Command {
    EndWhile();
    virtual ~EndWhile();
    int position;
    StartWhile* matching;
};

struct Collapsed : public Command {
    Collapsed();
    virtual ~Collapsed();
    std::unordered_map<int, int> adds;
};

struct Multiplies : public Command {
    Multiplies();
    virtual ~Multiplies();
    std::unordered_map<int, int> muls;
};

struct WhileShift : public Command {
    WhileShift();
    virtual ~WhileShift();
    int nb;
};

} //namespace bf

#endif

