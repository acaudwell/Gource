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

#include "slider.h"

// PositionSlider

PositionSlider::PositionSlider(float percent) {
    this->percent = percent;

    font = fontmanager.grab("FreeSans.ttf", 16);
    font.dropShadow(true);

    int gap = display.width / 30;

    bounds.update(vec2f(gap, display.height - gap*2));
    bounds.update(vec2f(display.width - gap, display.height - gap));

    slidercol = vec3f(1.0, 1.0, 1.0);

    mouseover = -1.0;

    mouseover_elapsed = 1.0;
    fade_time = 1.0;
    alpha = 0.0;
}

void PositionSlider::setColour(vec3f col) {
    slidercol = col;
}

void PositionSlider::show() {
    mouseover_elapsed = 0.0;
}

bool PositionSlider::mouseOver(vec2f pos, float* percent_ptr) {
    if(bounds.contains(pos)) {

        mouseover_elapsed = 0;
        mouseover = pos.x;

        if(percent_ptr != 0) {
            *percent_ptr = (float) (pos.x - bounds.min.x) / (bounds.max.x - bounds.min.x);
        }

        return true;
    }

    mouseover = -1.0;

    return false;
}

bool PositionSlider::click(vec2f pos, float* percent_ptr) {
    if(mouseOver(pos, &percent)) {

        if(percent_ptr != 0) {
            *percent_ptr = percent;
        }

        return true;
    }

    return false;
}

void PositionSlider::setCaption(const std::string& caption) {
    capwidth = 0.0;
    this->caption = caption;

    if(caption.size()) {
        capwidth = font.getWidth(caption.c_str());
    }
}

void PositionSlider::setPercent(float percent) {
    this->percent = percent;
}

void PositionSlider::logic(float dt) {

    if(mouseover < 0.0 && mouseover_elapsed < fade_time) mouseover_elapsed += dt;

    if(mouseover_elapsed < fade_time && alpha < 1.0) {
        alpha = std::min(1.0f, alpha+dt);

    } else if(mouseover_elapsed >= fade_time && alpha > 0.0) {

        alpha = std::max(0.0f, alpha-dt);

    }
}


void PositionSlider::draw(float dt) {
    glDisable(GL_TEXTURE_2D);
    glLineWidth(2.0f);
    glColor4f(slidercol.x, slidercol.y, slidercol.z, alpha);

    bounds.draw();

    float posx = bounds.min.x + (bounds.max.x - bounds.min.x) * percent;

    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(posx, bounds.min.y);
        glVertex2f(posx, bounds.max.y);
    glEnd();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    if(caption.size() && mouseover >= 0.0) {
        font.draw(std::min((double)display.width - capwidth - 1.0, std::max(1.0, mouseover - (capwidth/2.0))), bounds.min.y - 25.0, caption);
    }

}

