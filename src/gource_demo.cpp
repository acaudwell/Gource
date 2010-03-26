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

#include "gource_demo.h"

// GourceDemo

GourceDemo::GourceDemo(ConfFile* conf, FrameExporter* exporter) {

    this->conf     = conf;
    this->exporter = exporter;

    gource = 0;
    gource_settings = conf->getSections("gource")->begin();
}

GourceDemo::~GourceDemo() {
    if(gource!=0) delete gource;
}

void GourceDemo::keyPress(SDL_KeyboardEvent *e) {

    //Quit demo if the user presses ESC
    if (e->type == SDL_KEYDOWN) {
        if (e->keysym.sym == SDLK_ESCAPE) {
            appFinished=true;
        }
    }

    if(gource!=0) gource->keyPress(e);
}

void GourceDemo::mouseMove(SDL_MouseMotionEvent *e) {
    if(gource!=0) gource->mouseMove(e);
}

void GourceDemo::mouseClick(SDL_MouseButtonEvent *e) {
    if(gource!=0) gource->mouseClick(e);
}

Gource* GourceDemo::getNext() {

    if(gource!=0) delete gource;
    gource = 0;

    gGourceSettings.importGourceSettings(*conf, *gource_settings);

    if(gGourceSettings.stop_at_time<=0.0f) {
        gGourceSettings.stop_at_time=60.0f;
    }

    gource_settings++;

    //loop
    if(gource_settings == conf->getSections("gource")->end()) {
        gource_settings = conf->getSections("gource")->begin();
    }

    Gource* gource = new Gource(exporter);

    return gource;
}

void GourceDemo::update(float t, float dt) {

    if(gource==0 || gource->isFinished()) {
        gource = getNext();
    }

    gource->update(t, dt);
}
