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

#ifndef TEXT_KEY_H
#define TEXT_KEY_H

#include "core/display.h"
#include "core/vectors.h"
#include "core/fxfont.h"

#include "file.h"

#include <vector>
#include <algorithm>

class TextKeyEntry {
    FXFont font;
    vec3 colour;
    std::string label;
    std::string display_ext;
    float alpha;
    float brightness;
    int count;
    float pos_y;
    float src_y;
    float dest_y;
    float move_elapsed;
    float left_margin;
    float width;
    float height;
    vec2 pos;
    vec2 shadow;
    bool show;
public:
    TextKeyEntry(const FXFont& font, const std::string& label, const vec3& colour);

    const vec3& getColour() const;
    const std::string& getLabel() const;

    void setDestY(float dest_y);

    void colourize();
    
    void inc();
    void dec();
    
    void setShow(bool show);
    
    int getCount() const;
    void setCount(int count);

    bool isNew() const;
    bool isFinished() const;
    
    void logic(float dt);

    void draw();
};

class TextKey {
    std::vector<TextKeyEntry*> active_keys;
    std::map<std::string, TextKeyEntry*> keymap;

    FXFont font;
    float update_interval;
    float interval_remaining;
    bool show;
public:
    TextKey();
    ~TextKey();
    TextKey(float update_interval);
    
    void setShow(bool show);
    
    void clear();

    void colourize();
    
    void inc(RFile* file);
    void dec(RFile* file);

    void logic(float dt);
    
    void draw();
};

#endif
