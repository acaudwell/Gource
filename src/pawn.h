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

#include "core/display.h"
#include "core/fxfont.h"
#include "core/vectors.h"
#include "core/quadtree.h"

class Pawn : public QuadItem {
protected:
    vec2f pos;
    vec2f shadowOffset;

    std::string name;
    float namewidth;
    float size;
    vec2f accel;
    float speed;

    float elapsed;
    float fadetime;

    float nametime;
    float name_interval;
    vec3f namecol;

    bool shadow;

    bool hidden;

    int tagid;

    FXFont font;

    TextureResource* icon;

    int class_id;

    bool mouseover;

    virtual bool nameVisible();

    virtual void drawNameText(float alpha);
    virtual vec3f getNameColour();
protected:
    bool selected;
public:
    Pawn(std::string name, vec2f pos, int tagid);
    vec2f getPos();
    void setPos(vec2f pos);

    void updateQuadItemBounds();

    void showName();

    void setMouseOver(bool over);

    float getSize();
    int getTagID();
    std::string getName();

    virtual void setSelected(bool selected);

    void setHidden(bool hidden);
    bool isHidden();

    virtual float getAlpha() { return std::min(elapsed/fadetime, 1.0f); }
    virtual vec3f getColour() { return vec3f(1.0, 1.0, 1.0); }

    virtual TextureResource* getIcon() { return icon; };

    void logic(float dt);
    void draw(float dt);
    void drawShadow(float dt);
    void drawSimple(float dt);

    void drawName();
};

extern float gGourceShadowStrength;

#endif

