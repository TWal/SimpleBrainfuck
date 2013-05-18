#include "BfCommands.h"

BfCommand::BfCommand(COMMAND_TYPE t) : type(t) {}
BfOutput::BfOutput() : BfCommand(BF_OUTPUT) {}
BfInput::BfInput() : BfCommand(BF_INPUT) {}
BfPointerLeft::BfPointerLeft() : BfCommand(BF_POINTER_LEFT) {}
BfPointerRight::BfPointerRight() : BfCommand(BF_POINTER_RIGHT) {}
BfMinus::BfMinus() : BfCommand(BF_MINUS) {}
BfPlus::BfPlus() : BfCommand(BF_PLUS) {}
BfStartWhile::BfStartWhile() : BfCommand(BF_START_WHILE) {}
BfEndWhile::BfEndWhile() : BfCommand(BF_END_WHILE) {}
