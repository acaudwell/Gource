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

#ifndef CAMERA_H
#define CAMERA_H

#include "display.h"

#include <vector>

class Frustum;

class Light {
    vec3f pos;
public:
    Light();
    Light(vec3f pos);
    void lookAt(vec3f target);

    void  setPos(vec3f pos);
    vec3f getPos();
};

class Camera {
protected:
    vec3f _pos;
    vec3f _target;
    vec3f pos;
    vec3f target;

    vec3f up;

    float fov;
    float znear;
    float zfar;

public:
    Camera();
    Camera(vec3f pos, vec3f target = vec3f(0,0,0));

    void setPos(vec3f pos, bool keepangle = false);

    void setFov(float fov);
    void setZNear(float znear);
    void setZFar(float zfar);

    void setTarget(vec3f target);

    vec3f getTarget();
    vec3f getPos();

    vec3f getUp();
    float getFov();
    float getZNear();
    float getZFar();


    void reset();
    void look();
    void lookAt(vec3f target);

    void focus();
    void focusOn(vec3f p);
};

class CameraEvent {
protected:
    bool finished;
public:
    CameraEvent();
    bool isFinished();
    virtual void prepare(Camera* cam) {};
    virtual void logic(float dt, Camera* cam) {};
};

class CameraMoveEvent : public CameraEvent {

    float duration;
    float elapsed;

    float fov;
    vec3f pos;
    vec3f target;
    float old_fov;
    vec3f old_pos;
    vec3f old_target;
    
public:
    CameraMoveEvent(float fov, vec3f pos, vec3f target, float duration = 0.0);
    void prepare(Camera* cam);
    void logic(float dt, Camera* cam);
};

class CameraPath {
    Camera* cam;
    CameraEvent* current;
    int current_index;

    bool loop;
    bool finished;

    std::vector<CameraEvent*> events;
public:
    CameraPath(Camera* cam, bool loop = true);
    ~CameraPath();
    void addEvent(CameraEvent* ce);
    void clear();
    void logic(float dt);
    void reset();
    bool isFinished();
};

#endif
