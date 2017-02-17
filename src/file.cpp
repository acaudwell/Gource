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

#include "file.h"

float gGourceFileDiameter  = 8.0;

std::vector<RFile*> gGourceRemovedFiles;

FXFont file_selected_font;
FXFont file_font;

RFile::RFile(const std::string & name, const vec3 & colour, const vec2 & pos, int tagid) : Pawn(name,pos,tagid) {
    hidden = true;
    size = gGourceFileDiameter * 1.05;
    radius = size * 0.5;

    setGraphic(gGourceSettings.file_graphic);

    speed = 5.0;
    nametime = gGourceSettings.filename_time;
    name_interval = nametime;

    namecol     = vec3(1.0, 1.0, 1.0);
    file_colour = colour;

    last_action    = 0.0f;
    fade_start     = -1.0f;
    expired        = false;
    forced_removal = false;

    shadow = true;

    distance = 0;

    setFilename(name);

    if(!file_selected_font.initialized()) {
        file_selected_font = fontmanager.grab("FreeSans.ttf", 18);
        file_selected_font.dropShadow(true);
        file_selected_font.roundCoordinates(false);
        file_selected_font.setColour(vec4(gGourceSettings.selection_colour, 1.0f));
    }

    if(!file_font.initialized()) {
        file_font = fontmanager.grab("FreeSans.ttf", 14);
        file_font.dropShadow(true);
        file_font.roundCoordinates(false);
        file_font.setColour(vec4(gGourceSettings.filename_colour, 1.0f));
    }

    //namelist = glGenLists(1);
    //label = 0;
    setSelected(false);

    dir = 0;
}

RFile::~RFile() {
    //glDeleteLists(namelist, 1);
}

void RFile::remove(bool force) {
    last_action = elapsed;
    fade_start  = elapsed;
    if(force) forced_removal = true;
}

void RFile::setDir(RDirNode* dir) {
    this->dir = dir;
}

RDirNode* RFile::getDir() const{
    return dir;
}

vec2 RFile::getAbsolutePos() const{
    return pos + dir->getPos();
}

bool RFile::overlaps(const vec2& pos) const {

    vec2 abs_pos = getAbsolutePos();

    float halfsize_x = size * 0.5f;
    vec2 halfsize ( halfsize_x, halfsize_x * graphic_ratio );

    Bounds2D file_bounds(abs_pos - halfsize, abs_pos + halfsize);

    return file_bounds.contains(pos);
}

void RFile::setFilename(const std::string& abs_file_path) {

    fullpath = abs_file_path;

    size_t pos = fullpath.rfind('/');

    if(pos != std::string::npos) {
        path = name.substr(0,pos+1);
        name = name.substr(pos+1, std::string::npos);
    } else {
        path = std::string("");
        name = abs_file_path;
    }

    //trim name to just extension
    int dotsep=0;

    if((dotsep=name.rfind(".")) != std::string::npos && dotsep != name.size()-1) {
        ext = name.substr(dotsep+1);
    }
}

int call_count = 0;


void RFile::setSelected(bool selected) {
//    if(font.getFTFont()!=0 && this->selected==selected) return;
    //if(label && this->selected==selected) return;

//    if(!label) label = new FXLabel();

    Pawn::setSelected(selected);

//    updateLabel();

    //pre-compile name display list
    //glNewList(namelist, GL_COMPILE);
    //   font.draw(0.0f, 0.0f, (selected || shortname.size()==0) ? name : shortname);
    //glEndList();
}

void RFile::updateLabel() {
/*    bool show_file_ext = gGourceSettings.file_extensions;

    if(selected) {
        label->setText(file_selected_font, (selected || !show_file_ext) ? name : ext);
    } else {
        label->setText(file_font,          (selected || !show_file_ext) ? name : ext);
    }*/
}

void RFile::colourize() {
    file_colour = ext.size() ? colourHash(ext) : vec3(1.0f, 1.0f, 1.0f);
}

const vec3& RFile::getNameColour() const{
    return selected ? gGourceSettings.selection_colour : namecol;
}

void RFile::setFileColour(const vec3 & colour) {
    file_colour = colour;
}

const vec3 & RFile::getFileColour() const{
    return file_colour;
}

vec3 RFile::getColour() const{
    if(selected) return vec3(1.0f);

    float lc = elapsed - last_action;

    if(lc<1.0f) {
        return touch_colour * (1.0f-lc) + file_colour * lc;
    }

    return file_colour;
}

float RFile::getAlpha() const{
    float alpha = Pawn::getAlpha();

    //user fades out if not doing anything
    if(fade_start > 0.0f) {
        alpha = 1.0 - glm::clamp(elapsed - fade_start, 0.0f, 1.0f);
    }

    return alpha;
}

void RFile::logic(float dt) {
    Pawn::logic(dt);

    vec2 dest_pos = dest;
/*
    if(dir->getParent() != 0 && dir->noDirs()) {
        vec2 dirnorm = dir->getNodeNormal();
        dest_pos = dirnorm + dest;
    }*/

    dest_pos = dest_pos * distance;

    accel = dest_pos - pos;

    // apply accel
    vec2 accel2 = accel * speed * dt;

    if(glm::length2(accel2) > glm::length2(accel)) {
        accel2 = accel;
    }

    pos += accel2;

    //files have no momentum
    accel = vec2(0.0f, 0.0f);

    if(fade_start < 0.0f && gGourceSettings.file_idle_time > 0.0f && (elapsed - last_action) > gGourceSettings.file_idle_time) {
        fade_start = elapsed;
    }
    
    // has completely faded out
    if(fade_start > 0.0f && !expired && (elapsed - fade_start) >= 1.0) {

        expired = true;

        bool found = false;
        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                found = true;
                break;
            }
        }

        if(!found) {
            gGourceRemovedFiles.push_back(this);
            //fprintf(stderr, "expiring %s\n", fullpath.c_str());
        }
    }

    if(isHidden() && !forced_removal) elapsed = 0.0;
}

void RFile::touch(const vec3 & colour) {
    if(forced_removal) return;

    fade_start = -1.0f;
    
    //fprintf(stderr, "touch %s\n", fullpath.c_str());

    last_action = elapsed;
    touch_colour = colour;

    //un expire file
    if(expired) {
        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                gGourceRemovedFiles.erase(it);
                break;
            }
        }
        expired=false;
    }

    showName();
    setHidden(false);
    dir->fileUpdated(true);
}

void RFile::setHidden(bool hidden) {
    if(this->hidden==true && hidden==false && dir !=0) {
        dir->addVisible();
    }

    Pawn::setHidden(hidden);
}

void RFile::calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection) {

    static GLdouble screen_x, screen_y, screen_z;

    vec2 text_pos = getAbsolutePos();
    text_pos.x += 5.5f;

    if(selected)
        text_pos.y -= 2.0f;
    else
        text_pos.y -= 1.0f;

    gluProject( text_pos.x, text_pos.y, 0.0f, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);
    screen_y = (float)viewport[3] - screen_y;

    screenpos.x = screen_x;
    screenpos.y = screen_y;
}

void RFile::drawNameText(float alpha) {
    if(!selected && alpha <= 0.01) return;

    float name_alpha = selected ? 1.0 : alpha;

    if(selected) {
        file_selected_font.draw(screenpos.x, screenpos.y, name);
    } else {
        file_font.setAlpha(name_alpha);
        file_font.draw(screenpos.x, screenpos.y, gGourceSettings.file_extensions ? ext : name);
    }
}

void RFile::draw(float dt) {
    Pawn::draw(dt);

    glLoadName(0);
}
