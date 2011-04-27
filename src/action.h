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
public:
    RUser* source;
    RFile* target;

    float addedtime;

    float progress;
    float rate;

    RAction(RUser* source, RFile* target, float addedtime);

    inline bool isFinished() const { return (progress >= 1.0); };

    virtual void logic(float dt);

    void drawToVBO(quadbuf& buffer) const ;
    void draw(float dt);
};

class CreateAction : public RAction {
public:
    CreateAction(RUser* source, RFile* target, float addedtime);
};

class RemoveAction : public RAction {
public:
    RemoveAction(RUser* source, RFile* target, float addedtime);

    void logic(float dt);
};

class ModifyAction : public RAction {
public:
    ModifyAction(RUser* source, RFile* target, float addedtime);
};

class RenameAction : public RAction {
    std::string rename_to;
public:
    RenameAction(RUser* source, RFile* target, const std::string& rename_to, float addedtime);

    void logic(float dt);
};

#endif

