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

#include "zoomcamera.h"

ZoomCamera::ZoomCamera() {
}

ZoomCamera::ZoomCamera(vec3 pos, vec3 target, float min_distance, float max_distance) :
    pos(pos), _pos(pos), target(target), _target(target),  dest(pos), fov(90.0f) {

    znear = 0.1;

    up = vec3(0.0f, -1.0f, 0.0f);

    setMinDistance(min_distance);
    setMaxDistance(max_distance);

    padding = 1.0;
    speed = 1.0;
    lockon = false;
    lockon_time = 0.0;
    reset();
}

void ZoomCamera::reset() {
    pos    = _pos;
    target = _target;
}

float ZoomCamera::getMaxDistance() { return max_distance; }
float ZoomCamera::getMinDistance() { return min_distance; }

void ZoomCamera::setPadding(float padding) {
    this->padding = padding;
}

void ZoomCamera::setMaxDistance(float max) {
    max_distance = max;
    zfar = max + 1.0;
}

void ZoomCamera::setMinDistance(float min) {
    min_distance = min;
}

void ZoomCamera::lockOn(bool lockon) {

    if(lockon) {
         lockon_time = 1.0;
    }

    this->lockon = lockon;
}

void ZoomCamera::look() {
    lookAt(target);
}

void ZoomCamera::lookAt(const vec3& target) {
    gluLookAt( pos.x,    pos.y,    pos.z,
               target.x, target.y, target.z,
               up.x,     up.y,     up.z);
}

void ZoomCamera::focus() {
    display.mode3D(fov, znear, zfar);
    look();
}

void ZoomCamera::stop() {
    this->dest = pos;
}

void ZoomCamera::setSpeed(float speed) {
    this->speed = speed;
}
void ZoomCamera::adjust(const Bounds2D& bounds) {
    adjust(bounds, true);
}

void ZoomCamera::adjust(const Bounds2D& bounds, bool adjust_distance) {

    //center camera on bounds

    vec2 centre  = bounds.centre();

    //adjust by screen ratio
    dest.x = centre.x;
    dest.y = centre.y;

    if(!adjust_distance) return;

    //scale by 10% so we dont have stuff right on the edge of the screen
    float width  = bounds.width() * padding;
    float height = bounds.height() * padding;

    float aspect_ratio = display.width / (float) display.height;

    if(aspect_ratio < 1.0) {
        height /= aspect_ratio;
    } else {
        width /= aspect_ratio;
    }

    //calc visible width of the opposite wall at a distance of 1 this fov
    float toa = tan( fov * 0.5f * DEGREES_TO_RADIANS ) * 2.0;

    float distance;

    //TOA = tan = opposite/adjacent (distance = adjacent)
    //use the larger side of the box

    //cropping: vertical, horizontal or none
    if(gGourceSettings.crop_vertical) {

        distance =  width / toa;

        
    } else if (gGourceSettings.crop_horizontal) {

        distance =  height / toa;
        
    } else {

        if(width >= height) {
            distance =  width / toa;
        } else {
            distance =  height / toa;
        }
    }

    //debugLog("toa %.2f, distance %.2f width %.2f height %.2f dratio %.2f\n", toa, distance, width, height, dratio);

    //check bounds are valid
    if(distance < min_distance) distance = min_distance;
    if(distance > max_distance) distance = max_distance;

    this->dest.z = -distance;
}

void ZoomCamera::setDistance(float distance) {
    dest.z = -distance;
}

void ZoomCamera::setPos(const vec3& pos, bool keep_angle)  {
    if(keep_angle) {
        vec3 dir = target - this->pos;
        this->pos = pos;
        this->target = pos + dir;
    } else {
        this->pos = pos;
    }
}

void ZoomCamera::logic(float dt) {
    vec3 dp = (dest - pos);

    vec3 dpt = dp * dt * speed;

    if(lockon) {
        dpt = dpt * lockon_time + dp * (1.0f-lockon_time);

        if(lockon_time>0.0) {
            lockon_time = std::max(0.0f, lockon_time-dt*0.5f);
        }
    }

    if(glm::length2(dpt) > glm::length2(dp)) dpt = dp;

    pos += dpt;

    target = vec3(pos.x, pos.y, 0.0);
}
