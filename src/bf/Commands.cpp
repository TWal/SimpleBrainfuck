#include "Commands.h"
#include <stdio.h>

namespace bf {

Command::Command(COMMAND_TYPE t) : type(t), shift(0) {}
Command::~Command() {}
NoOperation::NoOperation() : Command(NO_OPERATION) {}
NoOperation::~NoOperation() {}
Output::Output() : Command(OUTPUT) {}
Output::~Output() {}
Input::Input() : Command(INPUT) {}
Input::~Input() {}
PointerLeft::PointerLeft() : Command(POINTER_LEFT) {}
PointerLeft::~PointerLeft() {}
PointerRight::PointerRight() : Command(POINTER_RIGHT) {}
PointerRight::~PointerRight() {}
Minus::Minus() : Command(MINUS) {}
Minus::~Minus() {}
Plus::Plus() : Command(PLUS) {}
Plus::~Plus() {}
StartWhile::StartWhile() : Command(START_WHILE) {}
StartWhile::~StartWhile() {}
EndWhile::EndWhile() : Command(END_WHILE) {}
EndWhile::~EndWhile() {}
Collapsed::Collapsed() : Command(COLLAPSED) {}
Collapsed::~Collapsed() {}
Multiplies::Multiplies() : Command(MULTIPLIES) {}
Multiplies::~Multiplies() {}
WhileShift::WhileShift() : Command(WHILE_SHIFT) {}
WhileShift::~WhileShift() {}

} //namespace bf

