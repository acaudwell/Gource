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

bool gGourceVerticalCrop   = false;
bool gGourceHorizontalCrop = false;

ZoomCamera::ZoomCamera() {
}

ZoomCamera::ZoomCamera(vec3f start, vec3f target, float min_distance, float max_distance) : Camera(start,target) {
    dest = start;
    up = vec3f(0.0f, -1.0f, 0.0f);

    setMinDistance(min_distance);
    setMaxDistance(max_distance);

    speed = 1.0;
    lockon = false;
    lockon_time = 0.0;
    reset();
}

void ZoomCamera::reset() {
    Camera::reset();
}

float ZoomCamera::getMaxDistance() { return max_distance; }
float ZoomCamera::getMinDistance() { return min_distance; }

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

void ZoomCamera::setSpeed(float speed) {
    this->speed = speed;
}

void ZoomCamera::adjust(Bounds2D& bounds) {

    //center camera on bounds

    //scale by 10% so we dont have stuff right on the edge of the screen
    float width  = bounds.width() * 1.10;
    float height = bounds.height() * 1.10;

    vec2f centre  = bounds.centre();

    //adjust by screen ratio
    float dratio = display.height / (float) display.width;

      if(dratio > 1.0) {
          height /= dratio;
      } else {
          width *= dratio;
      }

    //calc visible width of the opposite wall at a distance of 1 this fov
    float toa = tan( getFov() * 0.5f * DEGREES_TO_RADIANS ) * 2.0;

    float distance;

    //TOA = tan = opposite/adjacent (distance = adjacent)
    //use the larger side of the box

    //cropping: vertical, horizontal or none
    if(gGourceVerticalCrop) {
        distance =  width / toa ;

    } else if (gGourceHorizontalCrop) {
        distance =  height / toa ;

    } else {

        if(width > height) {
            distance =  width / toa ;
        } else {
            distance =  height / toa ;
        }
    }

    //debugLog("toa %.2f, distance %.2f width %.2f height %.2f dratio %.2f\n", toa, distance, width, height, dratio);

    //check bounds are valid
    if(distance < min_distance) distance = min_distance;
    if(distance > max_distance) distance = max_distance;

    this->dest = vec3f(centre.x, centre.y, -distance);
}

void ZoomCamera::logic(float dt) {
    vec3f dp = (dest - pos);

    vec3f dpt = dp * dt * speed;

    if(lockon) {
        dpt = dpt * lockon_time + dp * (1.0-lockon_time);

        if(lockon_time>0.0) {
            lockon_time = std::max(0.0f, lockon_time-dt*0.5f);
        }
    }

    if(dpt.length2() > dp.length2()) dpt = dp;

    pos += dpt;

    target = vec3f(pos.x, pos.y, 0.0);
}
