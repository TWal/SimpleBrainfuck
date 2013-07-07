/* Copyright (c) 2013, Theophile Wallez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

