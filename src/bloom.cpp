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

#include "bloom.h"

//bloombuf

bloombuf::bloombuf(int data_size) : data_size(data_size) {
    bufferid     = 0;
    buffer_size  = 0;
    vertex_count = 0;

    data = data_size > 0 ? new bloom_vertex[data_size] : 0;

    //fprintf(stderr, "size of bloom_vertex = %d\n", sizeof(bloom_vertex));
}

bloombuf::~bloombuf() {
    if(bufferid !=0) glDeleteBuffers(1, &bufferid);
    if(data != 0) delete[] data;
}

void bloombuf::resize(int new_size) {

    bloom_vertex* _data = data;

    data = new bloom_vertex[new_size];

    for(int i=0;i<data_size;i++) {
        data[i] = _data[i];
    }

    data_size = new_size;

    if(_data != 0) delete[] _data;
}

void bloombuf::reset() {
    vertex_count = 0;
}

void bloombuf::unload() {
    if(bufferid !=0) glDeleteBuffers(1, &bufferid);
    bufferid = 0;
    buffer_size = 0;
}

size_t bloombuf::vertices() {
    return vertex_count;
}

size_t bloombuf::capacity() {
    return data_size;
}

void bloombuf::add(GLuint textureid, const vec2& pos, const vec2& dims, const vec4& colour, const vec4& texcoord) {

    bloom_vertex v1(pos,                       colour, texcoord);
    bloom_vertex v2(pos + vec2(dims.x, 0.0f), colour, texcoord);
    bloom_vertex v3(pos + dims,                colour, texcoord);
    bloom_vertex v4(pos + vec2(0.0f, dims.y), colour, texcoord);

    int i = vertex_count;

    vertex_count += 4;

    if(vertex_count > data_size) {
        resize(vertex_count*2);
    }

    data[i]   = v1;
    data[i+1] = v2;
    data[i+2] = v3;
    data[i+3] = v4;
}

void bloombuf::update() {
    if(vertex_count==0) return;

    //note possibly better to have a queue and cycle them here
    if(bufferid==0) {
        glGenBuffers(1, &bufferid);
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferid);

    //recreate buffer if less than the vertex_count
    if(buffer_size < vertex_count) {
        buffer_size = data_size;
        glBufferData(GL_ARRAY_BUFFER, buffer_size*sizeof(bloom_vertex), &(data[0].pos.x), GL_DYNAMIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_count*sizeof(bloom_vertex), &(data[0].pos.x));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void bloombuf::draw() {
    if(vertex_count==0 || bufferid==0) return;

    glBindBuffer(GL_ARRAY_BUFFER, bufferid);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(2,   GL_FLOAT, sizeof(bloom_vertex), 0);
    glColorPointer(4,    GL_FLOAT, sizeof(bloom_vertex), (GLvoid*)8);  // offset pos (2x4 bytes)
    glTexCoordPointer(4, GL_FLOAT, sizeof(bloom_vertex), (GLvoid*)24); // offset pos + colour (2x4 + 4x4 bytes)

    glDrawArrays(GL_QUADS, 0, vertex_count);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
