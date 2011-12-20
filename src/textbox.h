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
#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <vector>
#include <string>

#include "core/display.h"
#include "core/vectors.h"
#include "core/fxfont.h"

class TextBox {

    std::vector<std::string> content;

    vec3 colour;
    float alpha;
    float brightness;
    vec2 corner;
    vec2 shadow;
    FXFont font;
    int max_width_chars;
    int rect_width;
    int rect_height;
    bool visible;
    
public:
    TextBox();
    TextBox(const FXFont& font);

    void hide();
    void show();
    
    void clear();

    void setPos(const vec2& pos, bool adjust = false);
    void setColour(const vec3& colour);
    void setAlpha(float alpha);
    void setBrightness(float brightness);
    
    void addLine(std::string str);

    void setText(const std::string& str);
    void setText(const std::vector<std::string>& content);

    void draw() const;
};

#endif
