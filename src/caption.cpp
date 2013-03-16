/*
    Copyright (C) 2010 Andrew Caudwell (acaudwell@gmail.com)

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

#include "caption.h"

RCaption::RCaption(const std::string& caption, time_t timestamp, const FXFont& font) {

    this->caption   = caption;
    this->timestamp = timestamp;
    this->font      = font;
    
    alpha   = 0.0;
    elapsed = 0.0;
    colour  = gGourceSettings.caption_colour;
}

void RCaption::setPos(const vec2& pos) {
    this->pos = pos;
}

const vec2& RCaption::getPos() const {
    return pos;
}

const std::string& RCaption::getCaption() const {
    return caption;
}

bool RCaption::isFinished() const {
    return elapsed >= gGourceSettings.caption_duration;
}

void RCaption::logic(float dt) {
    float fade_in = glm::min(2.0f, gGourceSettings.caption_duration / 3.0f);
    elapsed += dt;
    alpha = glm::min(1.0f, glm::min(elapsed,glm::max(0.0f,gGourceSettings.caption_duration-elapsed)) / fade_in);
}

void RCaption::draw() {
    font.setColour(vec4(colour.x, colour.y, colour.z, alpha));
    font.draw(pos.x, pos.y, caption);
}
