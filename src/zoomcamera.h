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

#include "core/bounds.h"
#include "core/frustum.h"

#include "gource_settings.h"

class ZoomCamera {
    vec3 pos;
    vec3 dest;
    vec3 target;
    vec3 up;
    vec3 _pos;
    vec3 _target;

    bool lockon;
    float speed;
    float lockon_time;

    float padding;
    
    float min_distance, max_distance;

    float fov;
    float znear, zfar;
public:
    ZoomCamera();
    ZoomCamera(vec3 start, vec3 target, float min_distance, float max_distance);

    void setSpeed(float speed);

    void lockOn(bool lockon);

    void look();
    void lookAt(const vec3& target);
    void focus();
    
    const vec3& getPos()    const { return pos; };
    const vec3& getUp()     const { return up; };
    const vec3& getTarget() const { return target; };
    const vec3& getDest()   const { return dest; };

    float getFOV()   { return fov; };
    float getZNear() { return znear; };
    float getZFar()  { return zfar; };
    
    void setPos(const vec3& pos, bool keep_angle = false);
    
    float getMinDistance();
    float getMaxDistance();

    void setPadding(float padding);
    void setDistance(float distance);

    void setMinDistance(float min);
    void setMaxDistance(float max);


    void reset();
    void logic(float dt);
    void adjustDistance();
    void adjust(const Bounds2D& bounds);
    void adjust(const Bounds2D& bounds, bool adjust_distance);
    
    void stop();
};

extern bool gGourceVerticalCrop;
extern bool gGourceHorizontalCrop;


#endif
