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

#include "pawn.h"
#include "dirnode.h"
#include "core/stringhash.h"

class RDirNode;

class RFile : public Pawn {
    vec3 file_colour;
    vec3 touch_colour;

    RDirNode* dir;

    bool forced_removal;
    bool expired;

    float fade_start;
    
    float last_action;

    float radius;

    vec2 dest;
    float distance;

   // FXLabel* label;

    //GLuint namelist;

    void setFilename(const std::string& abs_file_path);

    const vec3& getNameColour() const;
    void drawNameText(float alpha);
public:
    std::string path;
    std::string fullpath;
    std::string ext;

    RFile(const std::string & name, const vec3 & colour, const vec2 & pos, int tagid);
    ~RFile();

    bool overlaps(const vec2& pos) const;

    void setFileColour(const vec3 & colour);
    const vec3 & getFileColour() const;
    vec3 getColour() const;
    void colourize();

    float getAlpha() const;

    void touch(const vec3 & colour);

    void setSelected(bool selected);

    void updateLabel();

    void setHidden(bool hidden);

    void setDest(const vec2 & dest){ this->dest = dest; }
    void setDistance(float distance){ this->distance = distance; }

    void calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection);

    void logic(float dt);
    void draw(float dt);

    void remove(bool force=false);

    vec2 getAbsolutePos() const;

    RDirNode* getDir() const;
    void setDir(RDirNode* dir);
};

extern float gGourceFileDiameter;

extern std::vector<RFile*> gGourceRemovedFiles;

#endif
