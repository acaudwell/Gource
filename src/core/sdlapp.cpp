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

#include "sdlapp.h"

SDLApp::SDLApp() {
    fps=0;
    return_code=0;
    appFinished=false;
}

void SDLApp::updateFramerate() {
    if(fps_updater>0) {
        fps = (float)frame_count / (float)fps_updater * 1000.0f;
    } else {
        fps = 0;
    }
    fps_updater = 0;
    frame_count = 0;
}

bool SDLApp::isFinished() {
    return appFinished;
}

int SDLApp::returnCode() {
    return return_code;
}

void SDLApp::stop(int return_code) {
    this->return_code = return_code;
    appFinished=true;
}

int SDLApp::run() {

    Uint32 msec=0, last_msec=0, buffer_msec=0, total_msec = 0;

    frame_count = 0;
    fps_updater = 0;

    if(!appFinished) init();

    msec = SDL_GetTicks();
    last_msec = msec;

    while(!appFinished) {
        last_msec = msec;
        msec      = SDL_GetTicks();

        Uint32 delta_msec = msec - last_msec;

        // cant have delta ticks be 0
        buffer_msec += delta_msec;
        if(buffer_msec < 1) {
            SDL_Delay(1);
            continue;
        }

        delta_msec = buffer_msec;
        buffer_msec =0;

        //determine time elapsed since last time we were here
        total_msec += delta_msec;

        float t  = total_msec / 1000.0f;
        float dt = delta_msec / 1000.0f;

        fps_updater += delta_msec;

        //update framerate if a second has passed
        if (fps_updater >= 1000) {
            updateFramerate();
        }

        //process new events
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {

            switch(event.type) {
                case SDL_QUIT:
                    appFinished=true;
                    break;

                case SDL_MOUSEMOTION:
                    mouseMove(&event.motion);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    mouseClick(&event.button);
                    break;

                case SDL_MOUSEBUTTONUP:
                    mouseClick(&event.button);
                    break;

                case SDL_KEYDOWN:
                    keyPress(&event.key);
                    break;

                case SDL_KEYUP:
                    keyPress(&event.key);
                    break;

                default:
                    break;
            }
        }

        update(t, dt);

        //update display
        display.update();
        frame_count++;
    }

    return return_code;
}
