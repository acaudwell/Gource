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

#ifndef ZOOM_CAMERA_H
#define ZOOM_CAMERA_H

#include "core/camera.h"
#include "core/bounds.h"
#include "core/frustum.h"

class ZoomCamera : public Camera {
    vec3f dest;
    bool lockon;
    float speed;
    float lockon_time;

    float min_distance, max_distance;
public:
    ZoomCamera();
    ZoomCamera(vec3f start, vec3f target, float min_distance, float max_distance);

    void setSpeed(float speed);

    void lockOn(bool lockon);

    float getMinDistance();
    float getMaxDistance();

    void setMinDistance(float min);
    void setMaxDistance(float max);


    void reset();
    void logic(float dt);
    void adjust(Bounds2D& bounds);
};

extern bool gGourceVerticalCrop;
extern bool gGourceHorizontalCrop;


#endif
