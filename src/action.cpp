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

RAction::RAction(RUser* source, RFile* target, time_t timestamp, float t, const vec3& colour)
    : colour(colour), source(source), target(target), timestamp(timestamp), t(t), progress(0.0f), rate(0.5f) {
}

void RAction::apply() {
    target->touch(timestamp, colour);
}

void RAction::logic(float dt) {
    if(progress >= 1.0) return;

    if(progress == 0.0) {
        apply();
    }

    float action_rate = std::min(10.0f, rate * std::max(1.0f, ((float)source->getPendingActionCount())));

    progress = std::min(progress + action_rate * dt, 1.0f);
}

void RAction::drawToVBO(quadbuf& buffer) const {
    if(isFinished()) return;

    vec2 src  = source->getPos();
    vec2 dest = target->getAbsolutePos();

    //TODO: could use glm::perp

    vec2 n    = normalise(dest - src);
    vec2 perp = vec2(-n.y, n.x);

    vec2 offset     = perp * target->getSize() * 0.5f;
    vec2 offset_src = offset * 0.3f;

    float alpha = 1.0 - progress;
    float alpha2 = alpha * 0.1;

    vec4 col1 = vec4(colour, alpha);
    vec4 col2 = vec4(colour, alpha2);

    quadbuf_vertex v1(src  - offset_src,  col2, vec2(0.0f, 0.0f));
    quadbuf_vertex v2(src  + offset_src,  col2, vec2(0.0f, 1.0f));
    quadbuf_vertex v3(dest + offset,      col1, vec2(1.0f, 1.0f));
    quadbuf_vertex v4(dest - offset,      col1, vec2(1.0f, 0.0f));

    buffer.add(0, v1, v2, v3, v4);
}

void RAction::draw(float dt) {
    if(isFinished()) return;

    vec2 src  = source->getPos();
    vec2 dest = target->getAbsolutePos();

    vec2 n    = normalise(dest - src);
    vec2 perp = vec2(-n.y, n.x);

    vec2 offset     = perp * target->getSize() * 0.5f;
    vec2 offset_src = offset * 0.3f;

    float alpha = 1.0 - progress;
    float alpha2 = alpha * 0.1;

    vec4 col1 = vec4(colour, alpha);
    vec4 col2 = vec4(colour, alpha2);

    glBegin(GL_QUADS);
        glColor4fv(glm::value_ptr(col2));
        glTexCoord2f(0.0,0.0);
        glVertex2f(src.x - offset_src.x, src.y - offset_src.y);
        glTexCoord2f(0.0,1.0);
        glVertex2f(src.x + offset_src.x, src.y + offset_src.y);

        glColor4fv(glm::value_ptr(col1));
        glTexCoord2f(1.0,1.0);
        glVertex2f(dest.x + offset.x, dest.y + offset.y);
        glTexCoord2f(1.0,0.0);
       glVertex2f(dest.x - offset.x, dest.y - offset.y);
    glEnd();
}

CreateAction::CreateAction(RUser* source, RFile* target, time_t timestamp, float t)
    : RAction(source, target, timestamp, t, vec3(0.0f, 1.0f, 0.0f)) {
}

RemoveAction::RemoveAction(RUser* source, RFile* target, time_t timestamp, float t)
    : RAction(source, target, timestamp, t, vec3(1.0f, 0.0f, 0.0f)) {
}

void RemoveAction::logic(float dt) {
    float old_progress = progress;

    RAction::logic(dt);

    if(old_progress < 1.0 && progress >= 1.0) {
        target->remove(timestamp);
    }
}

ModifyAction::ModifyAction(RUser* source, RFile* target, time_t timestamp, float t, const vec3& modify_colour)
    : RAction(source, target, timestamp, t, vec3(1.0f, 0.7f, 0.3f)), modify_colour(modify_colour) {
}

void ModifyAction::apply() {
    RAction::apply();
    target->setFileColour(modify_colour);
}
