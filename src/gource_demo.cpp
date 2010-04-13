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

    next = false;

    gource = 0;
    gource_settings = conf->getSections("gource")->begin();

    transition_texture = 0;
    transition_interval = 0.0f;

    if(strstr((const char *)glGetString(GL_EXTENSIONS), "GL_ARB_texture_non_power_of_two" )) {
        transition_texture = display.emptyTexture(display.width, display.height, GL_RGBA);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

GourceDemo::~GourceDemo() {
    if(gource!=0) delete gource;
    if(transition_texture!=0) glDeleteTextures(1, &transition_texture);
}

void GourceDemo::keyPress(SDL_KeyboardEvent *e) {

    //Quit demo if the user presses ESC
    if (e->type == SDL_KEYDOWN) {
        if (e->keysym.unicode == SDLK_ESCAPE) {
            appFinished=true;
        }

        if (e->keysym.unicode == SDLK_RETURN) {
            next = true;
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

    if(gource!=0) {
        delete gource;
        gource = 0;

        transition_interval = 1.0f;
    }

    gGourceSettings.importGourceSettings(*conf, *gource_settings);

    if(gGourceSettings.stop_at_time <= 0.0f && gGourceSettings.stop_position <= 0.0f) {
        gGourceSettings.stop_at_time = 90.0f;
    }

    gource_settings++;

    //loop
    if(gource_settings == conf->getSections("gource")->end()) {
        gource_settings = conf->getSections("gource")->begin();
    }

    Gource* gource = new Gource(exporter);

    next = false;

    return gource;
}

void GourceDemo::blendLastFrame(float dt) {
    if(transition_texture==0 || transition_interval<0.0f) return;

    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, transition_texture);

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

void GourceDemo::update(float t, float dt) {

    if(gource == 0 || next || gource->isFinished()) {
        gource = getNext();
    }

    gource->update(t, dt);

    //copy last frame
    if(gource->isFinished() && transition_texture!=0) {
        display.renderToTexture(transition_texture, display.width, display.height, GL_RGBA);
    } else {
        //blend last frame of previous scene
        blendLastFrame(dt);
    }

}
