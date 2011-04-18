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

#include "vbo.h"

qbuf2f_vertex::qbuf2f_vertex() {
}

//qbuf2f

qbuf2f::qbuf2f() {
    bufferid   = 0;
    item_count = 0;
    //fprintf(stderr, "size of qbuf2f_vertex = %d\n", sizeof(qbuf2f_vertex));
}

qbuf2f::~qbuf2f() {
    if(bufferid !=0) glDeleteBuffers(1, &bufferid);
}

void qbuf2f::reset() {
    data.resize(0);
    item_count = 0;
}

size_t qbuf2f::size() {
    return data.size();
}

size_t qbuf2f::capacity() {
    return data.capacity();
}

vec4f qbuf2f_default_texcoord(0.0f, 0.0f, 1.0f, 1.0f);

void qbuf2f::add(const vec2f& pos, const vec2f& dims, const vec4f& colour) {
    add(pos, dims, colour, qbuf2f_default_texcoord);
}

void qbuf2f::add(const vec2f& pos, const vec2f& dims, const vec4f& colour, const vec4f& texcoord) {
    //debugLog("%d: %.2f, %.2f, %.2f, %.2f\n", i, pos.x, pos.y, dims.x, dims.y);

    vec2f offset = pos - dims * 0.5;

    qbuf2f_vertex v1(offset,                       colour, vec2f(texcoord.x, texcoord.y));
    qbuf2f_vertex v2(offset + vec2f(dims.x, 0.0f), colour, vec2f(texcoord.z, texcoord.y));
    qbuf2f_vertex v3(offset + dims,                colour, vec2f(texcoord.z, texcoord.w));
    qbuf2f_vertex v4(offset + vec2f(0.0f, dims.y), colour, vec2f(texcoord.x, texcoord.w));

    int i = data.size();

    data.resize(data.size()+4);

    data[i]   = v1;
    data[i+1] = v2;
    data[i+2] = v3;
    data[i+3] = v4;
}

void qbuf2f::update() {
    if(data.empty()) return;

    //note possibly better to have a queue and cycle them here
    if(bufferid==0) {
        glGenBuffers(1, &bufferid);
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferid);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(qbuf2f_vertex), &(data[0].pos.x), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void qbuf2f::draw() {
    if(data.empty() || bufferid==0) return;

    glBindBuffer(GL_ARRAY_BUFFER, bufferid);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2,   GL_FLOAT, sizeof(qbuf2f_vertex), 0);  
    glColorPointer(4,    GL_FLOAT, sizeof(qbuf2f_vertex), (GLvoid*)8);  // offset pos (2x4 bytes)
    glTexCoordPointer(2, GL_FLOAT, sizeof(qbuf2f_vertex), (GLvoid*)24); // offset pos + colour (2x4 + 4x4 bytes)

    glDrawArrays(GL_QUADS, 0, data.size());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
