/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RACTION_H
#define RACTION_H

#include "user.h"
#include "file.h"

class RUser;
class RFile;

class RAction {
protected:
    vec3f colour;
    std::string action;
public:
    RUser* source;
    RFile* target;

    float progress;
    float rate;

    RAction(RUser* source, RFile* target);

    bool isFinished();

    virtual void logic(float dt);
    void draw(float dt);
};

class CreateAction : public RAction {
public:
    CreateAction(RUser* source, RFile* target);
};

class RemoveAction : public RAction {
public:
    RemoveAction(RUser* source, RFile* target);

    void logic(float dt);
};

class ModifyAction : public RAction {
public:
    ModifyAction(RUser* source, RFile* target);
};

#endif

