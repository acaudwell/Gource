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

#ifndef PAWN_H
#define PAWN_H

#include <string>

#include "gource_settings.h"

#include "core/display.h"
#include "core/fxfont.h"
#include "core/vectors.h"
#include "core/quadtree.h"

class Pawn : public QuadItem {
protected:
    vec2 pos;
    vec2 shadowOffset;

    std::string name;
    float namewidth;
    vec2 accel;
    float speed;

    float elapsed;
    float fadetime;

    float nametime;
    float name_interval;
    vec3 namecol;

    bool shadow;

    bool hidden;

    int tagid;

    FXFont font;

    bool mouseover;

    virtual bool nameVisible() const;

    virtual void drawNameText(float alpha) {};
    virtual const vec3& getNameColour() const;
protected:
    bool selected;
public:
    float size;
    float graphic_ratio;
    TextureResource* graphic;
    vec3 screenpos;
    vec2 dims;

    Pawn(const std::string& name, vec2 pos, int tagid);
    const vec2 & getPos() const { return pos; }
    void setPos(vec2 pos);

    void calcScreenPos(const vec2& offset);

    void updateQuadItemBounds();

    void showName();

    void setMouseOver(bool over);

    float getSize();
    int getTagID();

    const std::string& getName() const { return name; }

    virtual void setSelected(bool selected);
    bool isSelected() { return selected; };

    void setHidden(bool hidden){ this->hidden = hidden; }
    bool isHidden() const { return hidden; }

    virtual float getAlpha() const{ return std::min(elapsed/fadetime, 1.0f); }
    virtual vec3 getColour() const { return vec3(1.0, 1.0, 1.0); }

    void setGraphic(TextureResource* graphic);

    void logic(float dt);
    void draw(float dt);
    void drawShadow(float dt);

    void drawName();
};

extern float gGourceShadowStrength;

#endif

