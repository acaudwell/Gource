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

#ifndef RFILE_H
#define RFILE_H

#include "core/stringhash.h"

#include "pawn.h"
#include "dirnode.h"

class RDirNode;

class RFile : public Pawn {
    vec3f file_colour;
    vec3f touch_colour;

    int path_hash;

    RDirNode* dir;

    bool removing;
    float last_action;

    float radius;

    vec2f dest;
    float distance;

    GLuint namelist;

    void setPath();
    vec3f getNameColour();
    void drawNameText(float alpha);
public:
    std::string path;
    std::string fullpath;

    RFile(std::string name, vec3f colour, vec2f pos, int tagid);
    ~RFile();

    vec3f getFileColour();
    vec3f getColour();

    float getAlpha();

    void touch(vec3f colour);

    std::string getFullPath();

    void setSelected(bool selected);

    void setHidden(bool hidden);

    void setDest(vec2f dest);
    void setDistance(float distance);

    void logic(float dt);
    void draw(float dt);

    void remove();

    vec2f getAbsolutePos();

    RDirNode* getDir();
    void setDir(RDirNode* dir);

    int getPathHash();
};

extern float gGourceFileDiameter;

extern std::vector<RFile*> gGourceRemovedFiles;

#endif
