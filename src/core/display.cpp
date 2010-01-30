
/*
    Copyright (c) 2008 Andrew Caudwell (acaudwell@gmail.com)
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

#include "display.h"
#include "sdlapp.h"

SDLAppDisplay display;

SDLAppDisplay::SDLAppDisplay() {
    clearColour = vec3f(0.0f,0.0f,0.0f);
    enable_shaders=false;
    enable_alpha=false;
    vsync=false;
    multi_sample = 0;
}

SDLAppDisplay::~SDLAppDisplay() {
}

void SDLAppDisplay::setClearColour(vec3f colour) {
    clearColour = colour;
}

int SDLAppDisplay::SDLFlags(bool fullscreen) {
    int flags = SDL_OPENGL | SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF;
    if (fullscreen) flags |= SDL_FULLSCREEN;
    return flags;
}

void SDLAppDisplay::enableVsync(bool vsync) {
    this->vsync = vsync;
}

#ifdef SDLAPP_SHADER_SUPPORT
void SDLAppDisplay::enableShaders(bool enable) {
    enable_shaders = enable;
}
#endif

void SDLAppDisplay::enableAlpha(bool enable) {
    enable_alpha = enable;
}

void SDLAppDisplay::multiSample(int samples) {
    multi_sample = samples;
}

void SDLAppDisplay::init(std::string window_title, int width, int height, bool fullscreen) {

    this->width  = width;
    this->height = height;

    this->fullscreen = fullscreen;

    int flags = SDLFlags(fullscreen);

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    //vsync
    if(vsync) SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    else SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);

    if(multi_sample > 0) {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (GLuint) multi_sample);
    }

    if(enable_alpha) {
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    }

#ifdef _WIN32
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    surface = SDL_SetVideoMode(width, height, 32, flags);
#else
    if(enable_shaders) {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        surface = SDL_SetVideoMode(width, height, 32, flags);
    } else {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        surface = SDL_SetVideoMode(width, height, 24, flags);
    }
#endif

    if (!surface) {
        std::string sdlerr(SDL_GetError());
        throw SDLInitException(sdlerr);
    }


#ifdef SDLAPP_SHADER_SUPPORT
    if(enable_shaders) {
        setupARBExtensions();
    }
#endif

    SDL_WM_SetCaption(window_title.c_str(),0);
}

void SDLAppDisplay::quit() {
    texturemanager.purge();
    fontmanager.purge();
}

void SDLAppDisplay::update() {
    SDL_GL_SwapBuffers();
}

void SDLAppDisplay::clear() {
    glClearColor(clearColour.x, clearColour.y, clearColour.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLAppDisplay::mode3D(float fov, float znear, float zfar) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (GLfloat)width/(GLfloat)height, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SDLAppDisplay::mode2D() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

vec4f SDLAppDisplay::currentColour() {
    vec4f colour;
   	glGetFloatv(GL_CURRENT_COLOR, colour);
   	return colour;
}

void SDLAppDisplay::checkGLErrors() {
//TODO
}

void SDLAppDisplay::fullScreenQuad(bool coord_flip) {

    int y1 = coord_flip ? 0 : 1;
    int y2 = coord_flip ? 1 : 0;

    glBegin(GL_QUADS);
        glTexCoord2i(0,y1);
        glVertex2f(0, 0);

        glTexCoord2i(1,y1);
        glVertex2i(display.width, 0);

        glTexCoord2i(1,y2);
        glVertex2i(display.width, display.height);

        glTexCoord2i(0,y2);
        glVertex2i(0, display.height);
    glEnd();
}

void SDLAppDisplay::renderToTexture(GLuint target, int width, int height, GLenum format) {
    glBindTexture(GL_TEXTURE_2D, target);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, format, 0, 0, width, height, 0);
}

GLuint SDLAppDisplay::emptyTexture(int width, int height, GLenum format) {
    GLuint textureid;

    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    return textureid;
}

GLuint SDLAppDisplay::createTexture(int width, int height, bool mipmaps, bool clamp, bool trilinear, GLenum format, unsigned int* data) {

    GLuint textureid;

    glGenTextures(1, &textureid);
    glBindTexture(GL_TEXTURE_2D, textureid);

    if(mipmaps) {
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, format, GL_UNSIGNED_BYTE, data);
    } else {
    	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    //GL_LINEAR_MIPMAP_LINEAR  - Trilinear
    //GL_LINEAR_MIPMAP_NEAREST - Bilinear

    if(trilinear) {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST_MIPMAP_NEAREST);

    if(clamp) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return textureid;
}

vec3f SDLAppDisplay::project(vec3f pos) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble winX, winY, winZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    gluProject( pos.x, pos.y, pos.z, modelview, projection, viewport, &winX, &winY, &winZ);

    winY = (float)viewport[3] - winY;

    return vec3f((float) winX, (float) winY, (float) winZ);
}

vec3f SDLAppDisplay::unproject(vec2f pos) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = pos.x;
    winY = (float)viewport[3] - pos.y;
    glReadPixels( int(winX), int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    return vec3f((float) posX, (float) posY, (float) posZ);
}
