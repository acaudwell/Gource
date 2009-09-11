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

#include "camera.h"

//Light

Light::Light() {
}

Light::Light(vec3f pos) {
    this->pos = pos;
}

void Light::lookAt(vec3f target) {
    gluLookAt( pos.x,    pos.y,    pos.z,
               target.x, target.y, target.z,
               0.0f,   1.0f,  0.0f);
}

void Light::setPos(vec3f pos) {
    this->pos = pos;
}

vec3f Light::getPos() {
    return pos;
}

//Camera

Camera::Camera() {
}

Camera::Camera(vec3f pos, vec3f target) {
    fov  = 90.0f;
    znear = 0.1f;
    zfar  = 1000.0f;

    up = vec3f(0.0, 1.0, 0.0);

    this->_pos=pos;
    this->_target=target;
    reset();
}

void Camera::focus() {
    display.mode3D(fov, znear, zfar);
    glMatrixMode(GL_PROJECTION);

    look();
}

void Camera::focusOn(vec3f p) {
    display.mode3D(fov, znear, zfar);
    glMatrixMode(GL_PROJECTION);

    lookAt(p);
}


void Camera::look() {
    lookAt(target);
}

void Camera::lookAt(vec3f target) {
    gluLookAt( pos.x,    pos.y,    pos.z,
               target.x, target.y, target.z,
               up.x, up.y, up.z);
}

vec3f Camera::getUp() {
    return up;
}

float Camera::getZNear() {
    return znear;
}

float Camera::getZFar() {
    return zfar;
}

float Camera::getFov() {
    return fov;
}

void Camera::setFov(float fov) {
    this->fov = fov;
}

void Camera::setZNear(float znear) {
    this->znear = znear;
}

void Camera::setZFar(float zfar) {
    this->zfar = zfar;
}

void Camera::setPos(vec3f pos, bool keepangle) {
    if(keepangle) {
        vec3f dir = target - this->pos;
        this->pos = pos;
        this->target = pos + dir;
    } else {
        this->pos = pos;
    }
}

void Camera::setTarget(vec3f target) {
    this->target = target;
}

vec3f Camera::getPos() {
    return pos;
}

vec3f Camera::getTarget() {
    return target;
}

void Camera::reset() {
    pos    = _pos;
    target = _target;
}

// CameraEvent
CameraEvent::CameraEvent() {
    finished=false;
}

bool CameraEvent::isFinished() {
    return finished;
}

// CameraMoveEvent

CameraMoveEvent::CameraMoveEvent(float fov, vec3f pos, vec3f target, float duration) : CameraEvent() {
    this->fov = fov;
    this->pos = pos;
    this->target = target;
    this->duration = duration;
    this->elapsed = 0.0;
}

void CameraMoveEvent::prepare(Camera* cam) {
    this->elapsed    = 0.0;
    this->finished   = false;
    this->old_fov    = cam->getFov();
    this->old_pos    = cam->getPos();
    this->old_target = cam->getTarget();
}

void CameraMoveEvent::logic(float dt, Camera* cam) {
    elapsed += dt;

    if(duration <= 0.0 || elapsed >= duration) {
        cam->setFov(fov);
        cam->setPos(pos);
        cam->setTarget(target);
        finished=true;
        return;
    }

    float pc = elapsed/duration;

    float f = old_fov * (1.0 - pc) + fov * pc;
    vec3f p = old_pos * (1.0 - pc) + pos * pc;
    vec3f t = old_target * (1.0 - pc) + target * pc;

    cam->setFov(f);
    cam->setPos(p);
    cam->setTarget(t);
}

// Camera Event Path

CameraPath::CameraPath(Camera* cam, bool loop) {
    this->cam = cam;
    current   = 0;
    current_index = -1;
    this->loop = loop;
    finished=false;
}

CameraPath::~CameraPath() {
    clear();
}

bool CameraPath::isFinished() {
    return finished;
}

void CameraPath::reset() {
    finished=false;
    current_index = -1;
    current = 0;
}

void CameraPath::clear() {
    for(std::vector<CameraEvent*>::iterator it = events.begin(); it != events.end(); it++) {
        delete *it;
    }
    events.clear();
}

void CameraPath::addEvent(CameraEvent* ce) {
    events.push_back(ce);
}

void CameraPath::logic(float dt) {
    if(finished || events.size() == 0 && current==0) return;

    if(current == 0) {
        if(loop) {
            current_index = (current_index + 1) % events.size();
        } else {
            current_index++;

            if(current_index >= events.size()) {
                finished=true;
                return;
            }
        }

        current = events[current_index];

        current->prepare(cam);
    }

    current->logic(dt, cam);

    if(current->isFinished()) {
        current = 0;
    }
}
