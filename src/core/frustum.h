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

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "vectors.h"
#include "camera.h"
#include "plane.h"
#include "pi.h"
#include "bounds.h"

class Camera;

class Frustum {

    float near_distance, far_distance;
    float view_ratio;
    float fov;

    float near_half_width;
    float near_half_height;
    float far_half_width;
    float far_half_height;

    vec3f near_top_left,    near_top_right;
    vec3f near_bottom_left, near_bottom_right;
    vec3f far_top_left,     far_top_right;
    vec3f far_bottom_left,  far_bottom_right;

    Plane planes[6];
public:
    Frustum();
    Frustum(Camera& camera);
    void update(Camera& camera);
    void updatePerspective(Camera& camera);
    void updateView(Camera& camera);

    bool boundsInFrustum(Bounds2D& bounds, float z = 0.0);

    bool contains(vec3f p);
};

#endif

