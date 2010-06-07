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

RFile::RFile(const std::string & name, const vec3f & colour, const vec2f & pos, int tagid) : Pawn(name,pos,tagid) {
    hidden = true;
    icon = texturemanager.grab("file.png");
    size = gGourceFileDiameter;
    radius = size * 0.5;

    speed = 5.0;
    nametime = 4.0;
    name_interval = nametime;

    namecol     = vec3f(1.0, 1.0, 1.0);
    file_colour = colour;

    last_action = 0.0;
    removing=false;

    shadow = true;

    distance = 0;

    this->fullpath = name;
    this->name = name;
    path_hash = 0;

    setPath();

    namelist = glGenLists(1);

    setSelected(false);

    dir = 0;

    if(path.size()) path_hash = stringHash(path);
}

RFile::~RFile() {
    glDeleteLists(namelist, 1);
}

void RFile::remove() {
    last_action = elapsed - gGourceSettings.file_idle_time;
}

void RFile::setDir(RDirNode* dir) {
    this->dir = dir;
}

const std::string & RFile::getFullPath() const {
    return fullpath;
}

RDirNode* RFile::getDir() const{
    return dir;
}

vec2f RFile::getAbsolutePos() const{
    return pos + dir->getPos();
}

int RFile::getPathHash() const{
    return path_hash;
}

void RFile::setPath() {

    size_t pos = name.rfind('/');

    if(pos == std::string::npos) {
        pos = name.rfind('\\');
    }

    if(pos != std::string::npos) {
        path = name.substr(0,pos+1);
        name = name.substr(pos+1, std::string::npos);
        return;
    }

    path = std::string("");
}

void RFile::setSelected(bool selected) {

    if(selected) {
        font = fontmanager.grab("FreeSans.ttf", 18);
    } else {
        font = fontmanager.grab("FreeSans.ttf", 11);
    }
    font.dropShadow(false);
    font.roundCoordinates(true);

    namewidth = font.getWidth(name.c_str());

    Pawn::setSelected(selected);

    //pre-compile name display list
    glNewList(namelist, GL_COMPILE);
       font.print(0.0f, 0.0f, name.c_str());
    glEndList();
}

vec3f RFile::getNameColour() const{
    return selected ? vec3f(1.0, 1.0, 0.3) : namecol;
}

const vec3f & RFile::getFileColour() const{
    return file_colour;
}

vec3f RFile::getColour() const{
    if(selected) return vec3f(1.0, 1.0, 1.0);

    float lc = elapsed - last_action;

    if(lc<1.0) {
        return touch_colour * (1.0-lc) + file_colour * lc;
    }

    return file_colour;
}

float RFile::getAlpha() const{
    float alpha = Pawn::getAlpha();

    //user fades out if not doing anything
    if(gGourceSettings.file_idle_time > 0.0f && elapsed - last_action > gGourceSettings.file_idle_time) {
        alpha = 1.0 - std::min(elapsed - last_action - gGourceSettings.file_idle_time, 1.0f);
    }

    return alpha;
}

void RFile::logic(float dt) {
    Pawn::logic(dt);

    vec2f dest_pos = dest;
/*
    if(dir->getParent() != 0 && dir->noDirs()) {
        vec2f dirnorm = dir->getNodeNormal();
        dest_pos = dirnorm + dest;
    }*/

    float dradius = dir->getRadius();

    dest_pos = dest_pos * distance;

    accel = dest_pos - pos;

    // apply accel
    vec2f accel2 = accel * speed * dt;

    if(accel2.length2() > accel.length2()) {
        accel2 = accel;
    }

    pos += accel2;

    //files have no momentum
    accel = vec2f(0.0f, 0.0f);

    // has completely faded out
    if(!removing && gGourceSettings.file_idle_time > 0.0f && elapsed - last_action >= gGourceSettings.file_idle_time + 1.0) {
        removing=true;

        bool found = false;

        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                found = true;
                break;
            }

        }

        if(!found) gGourceRemovedFiles.push_back(this);
    }

    if(isHidden()) elapsed = 0.0;
}

void RFile::touch(const vec3f & colour) {
    last_action = elapsed;
    touch_colour = colour;

    if(removing) {
        for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
            if((*it) == this) {
                gGourceRemovedFiles.erase(it);
                break;
            }
        }

        removing=false;
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

void RFile::drawNameText(float alpha) {

    vec3f nameCol = getNameColour();

    if(gGourceSettings.hide_filenames) alpha = 0.0;

    if(selected || alpha > 0.01) {

        float name_alpha = selected ? 1.0 : alpha;

        vec3f drawpos = vec3f(pos.x, pos.y, 0.0);

        vec3f screenpos = display.project(drawpos);

        screenpos.x += 10.0;
        screenpos.y -= 10.0;

        glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
                glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

         glMatrixMode(GL_MODELVIEW);
             glPushMatrix();
                glLoadIdentity();

            glTranslatef(screenpos.x, screenpos.y, 0.0);

            //hard coded drop shadow
            glPushMatrix();
                glTranslatef(1.0, 1.0, 0.0);
                glColor4f(0.0, 0.0, 0.0, name_alpha * 0.7f);
                glCallList(namelist);
            glPopMatrix();

            //draw name
            glColor4f(nameCol.x, nameCol.y, nameCol.z, name_alpha);
            glCallList(namelist);

         glMatrixMode(GL_PROJECTION);
            glPopMatrix();

         glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

    }
}

void RFile::draw(float dt) {
    Pawn::draw(dt);

    glLoadName(0);
}
