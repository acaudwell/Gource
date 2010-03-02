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

Pawn::Pawn(std::string name, vec2f pos, int tagid) {
    this->name  = name;
    this->pos   = pos;
    this->tagid = tagid;
    this->hidden = false;
    this->speed = 1.0;

    selected = false;
    mouseover = false;

    shadow = false;

    this->shadowOffset = vec2f(2.0, 2.0);

    this->elapsed = 0.0;
    this->fadetime = 1.0;
    this->nametime = 5.0;
    this->name_interval = 0.0;
    this->namecol = vec3f(1.0, 1.0, 1.0);
}

float Pawn::getSize() {
    return size;
}

vec2f Pawn::getPos() {
    return pos;
}

void Pawn::setPos(vec2f pos) {
    this->pos = pos;
}

int Pawn::getTagID() {
    return tagid;
}

void Pawn::showName() {
    if(name_interval <= 0.0) name_interval = nametime;
}

void Pawn::setHidden(bool hidden) {
    showName();
    this->hidden = false;
}

bool Pawn::isHidden() {
    return hidden;
}

void Pawn::updateQuadItemBounds() {

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;

    //set bounds
    Bounds2D bounds;
    bounds.update(pos - vec2f(halfsize,halfsize));
    bounds.update(pos + vec2f(halfsize,halfsize));

    quadItemBounds = bounds;
}

void Pawn::logic(float dt) {
    elapsed += dt;

    if(!isHidden()) {
        if(name_interval>0.0) name_interval -= dt;
    }
}

void Pawn::setMouseOver(bool over) {
    showName();
    this->mouseover = over;
}

void Pawn::setSelected(bool selected) {
    this->selected = selected;
}

std::string Pawn::getName() {
    return name;
}

vec3f Pawn::getNameColour() {
    return namecol;
}

void Pawn::drawNameText(float alpha) {

    if(alpha>0.0) {
        vec3f nameCol = getNameColour();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);

        glColor4f(nameCol.x, nameCol.y, nameCol.z, alpha);

        float namewidth = font.getWidth(name);
        font.draw(pos.x - ((float)namewidth/2.0), pos.y - size*1.2, name.c_str()); // above player
    }
}


bool Pawn::nameVisible() {
    return (!selected && name_interval < 0.0 || isHidden()) ? false : true;
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

void Pawn::drawSimple(float dt) {
    if(isHidden()) return;

    glLoadName(tagid);

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize*ratio);

    float alpha = getAlpha();
    vec3f col = getColour();

    glColor4f(col.x, col.y, col.z, alpha);

    glPushMatrix();
        glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(size, 0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(size, size*ratio);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f, size*ratio);
        glEnd();

    glPopMatrix();
}
void Pawn::drawShadow(float dt) {
    if(isHidden() || !shadow) return;

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize*ratio) + shadowOffset;

    float alpha = getAlpha();

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glColor4f(0.0, 0.0, 0.0, alpha * gGourceShadowStrength);

    glPushMatrix();
        glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(size, 0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(size, size*ratio);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f, size*ratio);
        glEnd();
    glPopMatrix();
}

void Pawn::draw(float dt) {
    if(hidden) return;

    glLoadName(tagid);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    float ratio = icon->h / (float) icon->w;
    float halfsize = size * 0.5f;
    vec2f offsetpos = pos - vec2f(halfsize, halfsize*ratio);

    float alpha = getAlpha();

    vec3f col = getColour();

    glBindTexture(GL_TEXTURE_2D, getIcon()->textureid);

    glPushMatrix();
        glTranslatef(offsetpos.x, offsetpos.y, 0.0f);

        glColor4f(col.x, col.y, col.z, alpha);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f, 0.0f);

            glTexCoord2f(1.0f,0.0f);
            glVertex2f(size, 0.0f);

            glTexCoord2f(1.0f,1.0f);
            glVertex2f(size, size*ratio);

            glTexCoord2f(0.0f,1.0f);
            glVertex2f(0.0f, size*ratio);
        glEnd();

    glPopMatrix();
}
