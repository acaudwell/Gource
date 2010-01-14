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

#ifndef SDLAPP_H
#define SDLAPP_H

#include "display.h"
#include "logger.h"

#include <vector>

extern std::string gSDLAppResourceDir;
extern std::string gSDLAppPathSeparator;

void SDLAppInit();
bool SDLAppDirExists(std::string dir);
void SDLAppParseArgs(int argc, char *argv[], int* xres, int* yres, bool* fullscreen, std::vector<std::string>* otherargs = 0);

class SDLApp {
    int frame_count;
    int fps_updater;
    int return_code;


    void updateFramerate();
protected:
    float fps;
    bool appFinished;
    void stop(int return_code);
public:
    SDLApp();
    virtual ~SDLApp() {};

    int run();

    virtual void update(float t, float dt) {};
    virtual void init() {};

    virtual void logic(float t, float dt) {};
    virtual void draw(float t, float dt) {};

    virtual void mouseMove(SDL_MouseMotionEvent *e) {};
    virtual void mouseClick(SDL_MouseButtonEvent *e) {};
    virtual void keyPress(SDL_KeyboardEvent *e) {};

    int returnCode();
    bool isFinished();
};

#endif
