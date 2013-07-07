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

