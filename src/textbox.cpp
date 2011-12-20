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

#include "textbox.h"

TextBox::TextBox() {
}

TextBox::TextBox(const FXFont& font) {
    this->font   = font;

    shadow = vec2(3.0f, 3.0f);
    
    colour = vec3(0.7f, 0.7f, 0.7f);
    corner = vec2(0.0f,0.0f);
    alpha  = 1.0f;
    brightness = 1.0f;
    max_width_chars = 1024;   
    rect_width = 0;
    rect_height = 0;
    visible = false;
}

void TextBox::hide() {
    visible = false;
}

void TextBox::show() {
    visible = true;
}

void TextBox::setBrightness(float brightness) {
    this->brightness = brightness;
}

void TextBox::setColour(const vec3& colour) {
    this->colour = colour;
}

void TextBox::setAlpha(float alpha) {
    this->alpha = alpha;
}

void TextBox::clear() {
    content.clear();
    rect_width = 0;
    rect_height = 2;
}

void TextBox::addLine(std::string str) {

    if(max_width_chars> 0 && str.size() > max_width_chars) {
        str = str.substr(0, max_width_chars);
    }

    int width = font.getWidth(str) + 6;

    if(width > rect_width) rect_width = width;
    
    rect_height += (font.getFontSize()+4);
    
    content.push_back(str);   
}

void TextBox::setText(const std::string& str) {

    clear();
   
    addLine(str);
}

void TextBox::setText(const std::vector<std::string>& content) {

    clear();

    for(std::vector<std::string>::const_iterator it = content.begin(); it != content.end(); it++) {
        addLine(*it);
    }
}

void TextBox::setPos(const vec2& pos, bool adjust) {

    corner = pos;

    if(!adjust) return;

    int fontheight = font.getFontSize() + 4;
    
    corner.y -= rect_height;

    if((corner.x + rect_width) > display.width) {
        if((corner.x - rect_width - fontheight )>0) {
            corner.x -= rect_width;
        } else {
            corner.x = display.width - rect_width;
        }
    }

    if(corner.y < 0) corner.y += rect_height + fontheight;
    if(corner.y +rect_height > display.height) corner.y -= rect_height;

}

void TextBox::draw() const {
    if(!visible) return;

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    glColor4f(0.0f, 0.0f, 0.0f, alpha * 0.333f);
    
    glPushMatrix();
        glTranslatef(shadow.x, shadow.y, 0.0f);
    
        glBegin(GL_QUADS);
            glVertex2f(corner.x,           corner.y);
            glVertex2f(corner.x,           corner.y + rect_height);
            glVertex2f(corner.x+rect_width, corner.y + rect_height);
            glVertex2f(corner.x+rect_width, corner.y);
        glEnd();
    glPopMatrix();
        
    glColor4f(colour.x * brightness, colour.y * brightness, colour.z * brightness, alpha);

    glBegin(GL_QUADS);
        glVertex2f(corner.x,           corner.y);
        glVertex2f(corner.x,           corner.y + rect_height);
        glVertex2f(corner.x+rect_width, corner.y + rect_height);
        glVertex2f(corner.x+rect_width, corner.y);
    glEnd();    
    
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    int yinc = 3;

    std::vector<std::string>::const_iterator it;
    for(it = content.begin(); it != content.end(); it++) {
        font.draw((int)corner.x+2, (int)corner.y+yinc,  (*it).c_str());
        yinc += font.getFontSize() + 4;
    }
}
