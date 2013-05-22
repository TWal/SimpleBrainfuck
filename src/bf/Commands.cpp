#include "Commands.h"

namespace bf {

Command::Command(COMMAND_TYPE t) : type(t) {}
NoOperation::NoOperation() : Command(NO_OPERATION) {}
Output::Output() : Command(OUTPUT) {}
Input::Input() : Command(INPUT) {}
PointerLeft::PointerLeft() : Command(POINTER_LEFT) {}
PointerRight::PointerRight() : Command(POINTER_RIGHT) {}
Minus::Minus() : Command(MINUS) {}
Plus::Plus() : Command(PLUS) {}
StartWhile::StartWhile() : Command(START_WHILE) {}
EndWhile::EndWhile() : Command(END_WHILE) {}
Collapsed::Collapsed() : Command(COLLAPSED) {}
Multiplies::Multiplies() : Command(MULTIPLIES) {}

} //namespace bf

