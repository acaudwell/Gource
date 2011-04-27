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

    RDirNode* dir;

    bool removing;
    bool expiring;

    float last_action;

    float radius;

    vec2f dest;
    float distance;

   // FXLabel* label;

    //GLuint namelist;

    void setFilename(const std::string& abs_file_path);

    const vec3f& getNameColour() const;
    void drawNameText(float alpha);
public:
    std::string path;
    std::string fullpath;
    std::string ext;

    RFile(const std::string & name, const vec3f & colour, const vec2f & pos, int tagid);
    ~RFile();

    bool isExpiring() const { return expiring; }
    bool isRemoving() const { return removing; }

    bool overlaps(const vec2f& pos) const;

    const vec3f & getFileColour() const;
    vec3f getColour() const;
    void colourize();

    float getAlpha() const;

    void touch(const vec3f& colour);
    void rename(const std::string& rename_to);

    void setSelected(bool selected);

    void updateLabel();

    void setHidden(bool hidden);

    void setDest(const vec2f & dest){ this->dest = dest; }
    void setDistance(float distance){ this->distance = distance; }

    void calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection);

    void logic(float dt);
    void draw(float dt);

    void remove(bool force=false);

    vec2f getAbsolutePos() const;

    RDirNode* getDir() const;
    void setDir(RDirNode* dir);
};

extern float gGourceFileDiameter;

extern std::vector<RFile*> gGourceRemovedFiles;

#endif
