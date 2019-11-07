/*
    Copyright (C) 2010 Andrew Caudwell (acaudwell@gmail.com)

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

#include "gource_shell.h"

GourceShell* gGourceShell = 0;

// GourceShell

GourceShell::GourceShell(ConfFile* conf, FrameExporter* exporter) {

    this->conf     = conf;
    this->exporter = exporter;

    min_delta_msec = 16;

    next = false;

    gource = 0;
    gource_settings = conf->getSections("gource")->begin();

    gGourceSettings.repo_count = conf->countSection("gource");

    toggle_delay = 0.0;
    transition_texture = 0;
    transition_interval = 0.0f;

    if(GLEW_ARB_texture_non_power_of_two || GLEW_VERSION_2_0) {
        transition_texture = texturemanager.create(display.width, display.height, false, GL_CLAMP_TO_EDGE, GL_RGBA);
    }
}

GourceShell::~GourceShell() {
    if(gource!=0) delete gource;
    if(transition_texture!=0) texturemanager.release(transition_texture);
}

void GourceShell::toggleFullscreen() {

    if(exporter != 0) return;

    texturemanager.unload();
    shadermanager.unload();
    fontmanager.unload();

    if(gource!=0) gource->unload();

    //recreate gl context
    display.toggleFullscreen();

    texturemanager.reload();
    shadermanager.reload();
    fontmanager.reload();

    if(gource!=0) gource->reload();
}

void GourceShell::toggleWindowFrame() {
#if SDL_VERSION_ATLEAST(2,0,0)
    if(toggle_delay > 0.0) return;
    if(display.isFullscreen()) return;
    if(exporter != 0) return;

    texturemanager.unload();
    shadermanager.unload();
    fontmanager.unload();

    if(gource!=0) gource->unload();

    display.toggleFrameless();

    texturemanager.reload();
    shadermanager.reload();
    fontmanager.reload();

    if(gource!=0) gource->reload();

    toggle_delay = 0.25f;
#endif
}


void GourceShell::resize(int width, int height) {

    texturemanager.unload();
    shadermanager.unload();
    fontmanager.unload();

    if(gource!=0) gource->unload();

    //recreate gl context
    display.resize(width, height);

    texturemanager.reload();
    shadermanager.reload();
    fontmanager.reload();

    if(gource!=0) gource->reload();
}

void GourceShell::keyPress(SDL_KeyboardEvent *e) {

    bool repeat = false;
#if SDL_VERSION_ATLEAST(2,0,0)
    repeat = (e->repeat > 0);
#endif

    //Quit demo if the user presses ESC
    if (e->type == SDL_KEYDOWN && !repeat) {

#if SDL_VERSION_ATLEAST(2,0,0)
        bool key_escape = e->keysym.sym == SDLK_ESCAPE;
        bool key_return = e->keysym.sym == SDLK_RETURN;
#else
        bool key_escape = e->keysym.unicode == SDLK_ESCAPE;
        bool key_return = e->keysym.unicode == SDLK_RETURN;
#endif

        if (key_escape) {
            quit();
        }

        if(gGourceSettings.disable_input) {
            // disable keyboard input other than the escape key
            return;
        }

        if (e->keysym.sym == SDLK_F11) {
            toggleWindowFrame();
        }

        if(key_return) {

#if SDL_VERSION_ATLEAST(2,0,0)
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            if(keystate[SDL_SCANCODE_RALT] || keystate[SDL_SCANCODE_LALT]) {
#else
            Uint8* keystate = SDL_GetKeyState(NULL);
            if(keystate[SDLK_RALT] || keystate[SDLK_LALT]) {
#endif

                toggleFullscreen();

            } else {
                if(gGourceSettings.repo_count>1)
                    next = true;
            }
        }
    }

    if(gource!=0) gource->keyPress(e);
}

void GourceShell::mouseMove(SDL_MouseMotionEvent *e) {
    if(gource!=0) gource->mouseMove(e);
}

#if SDL_VERSION_ATLEAST(2,0,0)
void GourceShell::mouseWheel(SDL_MouseWheelEvent *e) {
    if(gource!=0) gource->mouseWheel(e);
}
#endif

void GourceShell::mouseClick(SDL_MouseButtonEvent *e) {
    if(gource!=0) gource->mouseClick(e);
}

void GourceShell::quit() {
    if(gource!=0) gource->quit();
    gGourceSettings.shutdown=true;
}

Gource* GourceShell::getNext() {

    if(gource != 0) {
        transition_interval = 1.0f;
        delete gource;
        gource = 0;
    }

    if(gGourceSettings.shutdown || gource_settings == conf->getSections("gource")->end()) {
        // done
        return 0;
    }

    gGourceSettings.importGourceSettings(*conf, *gource_settings);

    //recording a video kind of implies you want this, unless:
    // -- dont stop requested
    // -- loop requested
    // -- reading from STDIN
    if(exporter!=0 && !(gGourceSettings.dont_stop || gGourceSettings.loop || gGourceSettings.path == "-"))
        gGourceSettings.stop_at_end = true;

    //multiple repo special settings
    if(gGourceSettings.repo_count > 1) {

        //set a stop condition
        if(gGourceSettings.stop_at_time <= 0.0f && gGourceSettings.stop_position <= 0.0f) {
            gGourceSettings.stop_at_time = 60.0f;
        }
    }

    gource_settings++;

    //loop unless only 1 repo
    if(gource_settings == conf->getSections("gource")->end()) {
        if(gGourceSettings.repo_count>1 && exporter==0) {
            gource_settings = conf->getSections("gource")->begin();
        }
    }

    // replace gource
    gource = new Gource(exporter);

    next = false;

    return gource;
}

void GourceShell::blendLastFrame(float dt) {
    if(transition_texture==0 || transition_interval <= 0.0f) return;

    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    transition_texture->bind();

    glColor4f(1.0, 1.0, 1.0, transition_interval);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0.0f, 0.0);

        glTexCoord2f(1.0, 1.0f);
        glVertex2f(display.width, 0.0);

        glTexCoord2f(1.0, 0.0f);
        glVertex2f(display.width, display.height);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0.0f, display.height);
    glEnd();

    transition_interval -= dt;
}

void GourceShell::update(float t, float dt) {

    if(gource == 0 || gource->isFinished()) {
        if(!getNext()) appFinished=true;

        return;
    }

    gource->fps = this->fps;
    gource->update(t, dt);

    if(toggle_delay > 0.0) toggle_delay -= dt;

    //copy last frame
    if( (next|| gource->isFinished()) && transition_texture!=0) {

        glEnable(GL_TEXTURE_2D);
        transition_texture->bind();
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, display.width, display.height, 0);

    } else {
        //blend last frame of previous scene
        blendLastFrame(dt);
    }

    if(next) {
        delete gource;
        gource = 0;
        transition_interval = 1.0f;
        next = false;
    }
}
