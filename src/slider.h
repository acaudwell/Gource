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

#ifndef POS_SLIDER_H
#define POS_SLIDER_H

#include "core/logger.h"
#include "core/bounds.h"
#include "core/fxfont.h"

class PositionSlider {

    FXFont font;

    Bounds2D bounds;
    float percent;
    float mouseover;
    float mouseover_elapsed;
    float fade_time;
    float alpha;

    vec3 slidercol;

    float capwidth;
    std::string caption;

    void drawSlider(float position) const;
public:
    PositionSlider(float percent = 0.0f);

    void setColour(vec3 col);

    void setCaption(const std::string& cap);

    void setPercent(float percent);

    void resize();

    void show();

    const Bounds2D& getBounds() const;

    bool mouseOver(vec2 pos, float* percent_ptr);
    bool click(vec2 pos, float* percent_ptr);
    void logic(float dt);
    void draw(float dt);
};

#endif
