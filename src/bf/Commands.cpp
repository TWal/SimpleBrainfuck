#include "Commands.h"

namespace bf {

Command::Command(COMMAND_TYPE t) : type(t) {}
Output::Output() : Command(OUTPUT) {}
Input::Input() : Command(INPUT) {}
PointerLeft::PointerLeft() : Command(POINTER_LEFT) {}
PointerRight::PointerRight() : Command(POINTER_RIGHT) {}
Minus::Minus() : Command(MINUS) {}
Plus::Plus() : Command(PLUS) {}
StartWhile::StartWhile() : Command(START_WHILE) {}
EndWhile::EndWhile() : Command(END_WHILE) {}
MultiAdds::MultiAdds() : Command(MULTI_ADDS) {}
Multiplies::Multiplies() : Command(MULTIPLIES) {}

} //namespace bf

