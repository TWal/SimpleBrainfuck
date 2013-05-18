#ifndef BFCOMMANDS_H
#define BFCOMMANDS_H

#include <vector>

struct BfCommand {
    enum COMMAND_TYPE {
        BF_OUTPUT,
        BF_INPUT,
        BF_POINTER_LEFT,
        BF_POINTER_RIGHT,
        BF_MINUS,
        BF_PLUS,
        BF_START_WHILE,
        BF_END_WHILE
    };

    BfCommand(COMMAND_TYPE t);

    COMMAND_TYPE type;
};

struct BfOutput : public BfCommand {
    BfOutput();
};

struct BfInput : public BfCommand {
    BfInput();
};

struct BfPointerLeft : public BfCommand {
    BfPointerLeft();
};

struct BfPointerRight : public BfCommand {
    BfPointerRight();
};

struct BfMinus : public BfCommand {
    BfMinus();
};

struct BfPlus : public BfCommand {
    BfPlus();
};

struct BfStartWhile : public BfCommand {
    BfStartWhile();
    int matching;
};

struct BfEndWhile : public BfCommand {
    BfEndWhile();
    int matching;
};



#endif

