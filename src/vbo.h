/*
    Copyright (C) 2011 Andrew Caudwell (acaudwell@gmail.com)

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

#ifndef GOURCE_VBO_H
#define GOURCE_VBO_H

#include <stack>

#include "core/display.h"
#include "core/vectors.h"
#include "core/logger.h"

//note this should be 32 bytes (8x4 bytes)
class qbuf2f_vertex {
public:
    qbuf2f_vertex() {};
    qbuf2f_vertex(const vec2f& pos, const vec4f& colour, const vec2f& texcoord) : pos(pos), colour(colour), texcoord(texcoord) {};

    vec2f pos;
    vec4f colour;
    vec2f texcoord;
};

//maintain ranges corresponding to each texture
class qbuf2f_tex {
public:
    qbuf2f_tex() {};
    qbuf2f_tex(int start_index, GLuint textureid) : start_index(start_index), textureid(textureid) {};
    int start_index;
    GLuint textureid;
};

class qbuf2f {

    qbuf2f_vertex* data;
    int data_size;

    std::vector<qbuf2f_tex> textures;
    
    GLuint bufferid;
    int buffer_size;

    int vertex_count;
    
    void resize(int new_size);
public:
    qbuf2f(int data_size = 1);
    ~qbuf2f();

    void reset();

    size_t vertices();
    size_t capacity();
    size_t texture_changes();
    
    void add(GLuint textureid, const vec2f& pos, const vec2f& dims, const vec4f& colour);
    void add(GLuint textureid, const vec2f& pos, const vec2f& dims, const vec4f& colour, const vec4f& texcoord);

    void update();
    void draw();
};

#endif
