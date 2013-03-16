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

#include "pawn.h"

float gGourceShadowStrength = 0.5;

Pawn::Pawn(const std::string& name, vec2 pos, int tagid) {
    this->name  = name;
    this->pos   = pos;
    this->tagid = tagid;
    this->hidden = false;
    this->speed = 1.0;

    selected = false;
    mouseover = false;

    shadow = false;

    namewidth = 0;

    this->shadowOffset = vec2(2.0, 2.0);
    this->elapsed = 0.0;
    this->fadetime = 1.0;
    this->nametime = 5.0;
    this->name_interval = 0.0;
    this->namecol = vec3(1.0, 1.0, 1.0);

    this->graphic = 0;
    this->graphic_ratio = 1.0;
}

float Pawn::getSize() {
    return size;
}

void Pawn::setPos(vec2 pos) {
    this->pos = pos;
}

int Pawn::getTagID() {
    return tagid;
}

void Pawn::showName() {
    if(name_interval <= 0.0) name_interval = nametime;
}

void Pawn::updateQuadItemBounds() {

    float halfsize_x = size * 0.5f;

    vec2 halfsize ( halfsize_x, halfsize_x * graphic_ratio );

    //set bounds
    quadItemBounds.set(pos - halfsize, pos + halfsize);
}

void Pawn::logic(float dt) {
    elapsed += dt;

    if(!isHidden()) {
        if(name_interval>0.0) name_interval -= dt;
    }
}

void Pawn::setGraphic(TextureResource* graphic) {

    if(graphic) {
        graphic_ratio = graphic->h / (float) graphic->w;
    } else {
        graphic_ratio = 1.0f;
    }

    this->graphic = graphic;
    this->dims = vec2(size, size*graphic_ratio);
}


void Pawn::setMouseOver(bool over) {
    //showName();
    this->mouseover = over;
}

void Pawn::setSelected(bool selected) {
    this->selected = selected;
}

const vec3& Pawn::getNameColour() const {
    return namecol;
}

void Pawn::calcScreenPos(const vec2& offset) {
    screenpos = display.project(vec3(pos.x+offset.x, pos.y+offset.y, 0.0f));
}

bool Pawn::nameVisible() const {
    return ((!selected && name_interval < 0.0) || isHidden()) ? false : true;
}

void Pawn::drawName() {
    if(!nameVisible()) return;

    float done = nametime - name_interval;

    if(done < 1.0) {
        drawNameText(done);
    } else if(done > 1.0 && done < nametime - 1.0) {
        drawNameText(1.0);
    } else {
        drawNameText((nametime - done));
    }
}

void Pawn::drawShadow(float dt) {
    if(isHidden() || !shadow) return;

    float halfsize = size * 0.5f;
    vec2 offsetpos = pos - vec2(halfsize, halfsize*graphic_ratio) + shadowOffset;

    float alpha = getAlpha();

    glBindTexture(GL_TEXTURE_2D, graphic->textureid);

    glColor4f(0.0, 0.0, 0.0, alpha * gGourceShadowStrength);

    glPushMatrix();
        glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(size, 0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(size, size*graphic_ratio);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f, size*graphic_ratio);
        glEnd();
    glPopMatrix();
}

void Pawn::draw(float dt) {
    if(hidden) return;

    float halfsize = size * 0.5f;
    vec2 offsetpos = pos - vec2(halfsize, halfsize*graphic_ratio);

    float alpha = getAlpha();

    vec3 col = getColour();

    glBindTexture(GL_TEXTURE_2D, graphic->textureid);

    glPushMatrix();
        glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

        glColor4f(col.x, col.y, col.z, alpha);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(size, 0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(size, size*graphic_ratio);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f, size*graphic_ratio);
        glEnd();

    glPopMatrix();
}
