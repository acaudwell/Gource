/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef BOUNDS_H
#define BOUNDS_H

#include "display.h"
#include "vectors.h"

class Bounds2D {
public:
    vec2f min;
    vec2f max;
    bool first;

    vec2f centre() {
        return min + (max - min) * 0.5f;
    }

    float width() {
        return max.x - min.x;
    }

    float height() {
        return max.y - min.y;
    }

    float area() {
        return width() * height();
    }

    void reset() {
        min = vec2f(0.0, 0.0);
        max = vec2f(0.0, 0.0);
        first = true;
    }

    Bounds2D() {
        reset();
    }

    Bounds2D(vec2f min, vec2f max) {
        reset();
        update(min);
        update(max);
    }

    void update(vec2f point) {
        if(first) {
            min = point;
            max = point;
            first=false;
            return;
        }

        if(min.x > point.x) min.x = point.x;
        if(min.y > point.y) min.y = point.y;
        if(max.x < point.x) max.x = point.x;
        if(max.y < point.y) max.y = point.y;
    }

    bool contains(vec2f point) {
        if(first) return false;

        if(min.x<=point.x && min.y<=point.y && max.x >= point.x && max.y >= point.y)
            return true;

        return false;
    }

    bool overlaps(Bounds2D& b) {

        if(max.y < b.min.y) return false;
        if(min.y > b.max.y) return false;
        if(max.x < b.min.x) return false;
        if(min.x > b.max.x) return false;

        return true;
    }

    void draw() {
        glBegin(GL_LINE_STRIP);
            glVertex2fv(min);
            glVertex2f(max.x, min.y);
            glVertex2fv(max);
            glVertex2f(min.x, max.y);
            glVertex2fv(min);
        glEnd();
    }
};

class Bounds3D {
public:
    vec3f min;
    vec3f max;
    bool first;

    void reset() {
        min = vec3f(0.0, 0.0, 0.0);
        max = vec3f(0.0, 0.0, 0.0);
        first  = true;
    }

    Bounds3D() {
        reset();
    }

    Bounds3D(vec3f min, vec3f max) {
        reset();
        update(min);
        update(max);
    }

    float width() {
        return max.x - min.x;
    }

    float height() {
        return max.y - min.y;
    }

    float depth() {
        return max.z - min.z;
    }

    float area() {
        return width() * height() * depth();
    }

    vec3f centre() {
         return min + ((max-min) * 0.5);
    }

    void update(vec3f point) {
        if(first) {
            min = point;
            max = point;
            first = false;
            return;
        }

        if(min.x > point.x) min.x = point.x;
        if(min.y > point.y) min.y = point.y;
        if(min.z > point.z) min.z = point.z;
        if(max.x < point.x) max.x = point.x;
        if(max.y < point.y) max.y = point.y;
        if(max.z < point.z) max.z = point.z;
    }

    bool contains(vec3f& point) {
        if(first) return false;

        if(min.x<=point.x && min.y<=point.y && min.z<=point.z && max.x >= point.x && max.y >= point.y && max.z >= point.z)
            return true;

        return false;
    }

    void draw() {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
            glVertex3fv(min);
            glVertex3fv(max);
        glEnd();
    }

};

#endif
