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

#ifndef GOURCE_SHELL_H
#define GOURCE_SHELL_H

#include "core/display.h"
#include "core/sdlapp.h"
#include "gource.h"

class GourceShell : public SDLApp {

    Gource* gource;
    bool next;

    TextureResource* transition_texture;
    float transition_interval;
    float toggle_delay;

    FrameExporter* exporter;
    ConfFile* conf;
    ConfSectionList::iterator gource_settings;

    Gource* getNext();
    void blendLastFrame(float dt);
public:
    GourceShell(ConfFile* conf, FrameExporter* exporter);
    ~GourceShell();

    void update(float t, float dt);

    void resize(int width, int height);

    void toggleFullscreen();
    void toggleWindowFrame();

    void quit();

    void keyPress(SDL_KeyboardEvent *e);
    void mouseMove(SDL_MouseMotionEvent *e);
    void mouseClick(SDL_MouseButtonEvent *e);
#if SDL_VERSION_ATLEAST(2,0,0)
    void mouseWheel(SDL_MouseWheelEvent *e);
#endif
};

#endif

extern GourceShell* gGourceShell;
