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

#include "action.h"

RAction::RAction(RUser* source, RFile* target, float addedtime) {
    this->source    = source;
    this->target    = target;
    this->addedtime = addedtime;

    progress = 0.0;

    rate = 0.5;
}

void RAction::logic(float dt) {
    if(progress >= 1.0) return;

    if(progress == 0.0) {
        target->touch(colour);
    }

    float action_rate = std::min(10.0f, rate * std::max(1.0f, ((float)source->getPendingActionCount())));

    progress = std::min(progress + action_rate * dt, 1.0f);
}

void RAction::drawToVBO(quadbuf& buffer) const {
    if(isFinished()) return;

    vec2f src  = source->getPos();
    vec2f dest = target->getAbsolutePos();

    vec2f offset     = (dest - src).normal().perpendicular() * target->getSize() * 0.5;
    vec2f offset_src = offset * 0.3f;

    float alpha = 1.0 - progress;
    float alpha2 = alpha * 0.1;

    vec4f col1 = vec4f(colour, alpha);
    vec4f col2 = vec4f(colour, alpha2);

    quadbuf_vertex v1(src  - offset_src,  col2, vec2f(0.0f, 0.0f));
    quadbuf_vertex v2(src  + offset_src,  col2, vec2f(0.0f, 1.0f));
    quadbuf_vertex v3(dest + offset,      col1, vec2f(1.0f, 1.0f));
    quadbuf_vertex v4(dest - offset,      col1, vec2f(1.0f, 0.0f));

    buffer.add(0, v1, v2, v3, v4);
}

void RAction::draw(float dt) {
    if(isFinished()) return;

    vec2f src  = source->getPos();
    vec2f dest = target->getAbsolutePos();

    vec2f offset     = (dest - src).normal().perpendicular() * target->getSize() * 0.5;
    vec2f offset_src = offset * 0.3f;

    float alpha = 1.0 - progress;
    float alpha2 = alpha * 0.1;

    vec4f col1 = vec4f(colour, alpha);
    vec4f col2 = vec4f(colour, alpha2);

    glBegin(GL_QUADS);
        glColor4fv(col2);
        glTexCoord2f(0.0,0.0);
        glVertex2f(src.x - offset_src.x, src.y - offset_src.y);
        glTexCoord2f(0.0,1.0);
        glVertex2f(src.x + offset_src.x, src.y + offset_src.y);

        glColor4fv(col1);
        glTexCoord2f(1.0,1.0);
        glVertex2f(dest.x + offset.x, dest.y + offset.y);
        glTexCoord2f(1.0,0.0);
       glVertex2f(dest.x - offset.x, dest.y - offset.y);
    glEnd();
}

CreateAction::CreateAction(RUser* source, RFile* target, float addedtime) : RAction(source, target, addedtime) {
    colour = vec3f(0.0, 1.0, 0.0);
}

RemoveAction::RemoveAction(RUser* source, RFile* target, float addedtime): RAction(source, target, addedtime) {
    colour = vec3f(1.0, 0.0, 0.0);
}

void RemoveAction::logic(float dt) {
    float old_progress = progress;

    RAction::logic(dt);

    if(old_progress < 1.0 && progress >= 1.0) {
        target->remove();
    }
}

ModifyAction::ModifyAction(RUser* source, RFile* target, float addedtime) : RAction(source, target, addedtime) {
    colour = vec3f(1.0, 0.7, 0.3);
}

RenameAction::RenameAction(RUser* source, RFile* target, const std::string& rename_to, float addedtime): RAction(source, target, addedtime) {
    colour = vec3f(0.0, 1.0, 1.0);
    this->rename_to = rename_to;
}

void RenameAction::logic(float dt) {
    float old_progress = progress;

    if(progress == 0.0) {

        if(rename_to[rename_to.size()-1] == '/') {
            rename_to += target->getName();
        }

        target->rename(rename_to);
    }

    RAction::logic(dt);
}
