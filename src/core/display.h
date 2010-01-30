
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

#ifndef SDLAPP_DISPLAY_H
#define SDLAPP_DISPLAY_H

#include "extensions.h"

#include "logger.h"
#include "vectors.h"

#include "texture.h"
#include "fxfont.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>
#include <string>

class SDLInitException : public std::exception {
protected:
    std::string error;
public:
    SDLInitException(std::string& error) : error(error) {}
    virtual ~SDLInitException() throw () {};

    virtual const char* what() const throw() { return error.c_str(); }
};

class SDLAppDisplay {
    SDL_Surface *surface;

    bool enable_shaders;
    bool enable_alpha;
    bool vsync;

    int  multi_sample;

    int    SDLFlags(bool fullscreen);
public:
    int width, height;
    bool fullscreen;
    vec3f clearColour;

    SDLAppDisplay();
    ~SDLAppDisplay();

    void   init(std::string window_title, int xres, int yres, bool fullscreen);

    void   quit();

    void   update();
    void   clear();
    void   enableVsync(bool vsync);
    void   setClearColour(vec3f colour);

#ifdef SDLAPP_SHADER_SUPPORT
    void   enableShaders(bool enable);
#endif

    void   enableAlpha(bool enable);
    void   multiSample(int sample);

    void   mode3D(float fov, float znear, float zfar);
    void   mode2D();

    vec4f  currentColour();

    void fullScreenQuad(bool coord_flip);

    void   renderToTexture(GLuint target, int width, int height, GLenum format);
    GLuint emptyTexture(int width, int height, GLenum format);
    GLuint createTexture(int width, int height, bool mipmaps, bool clamp, bool trilinear, GLenum format, unsigned int* data);

    vec3f project(vec3f pos);
    vec3f unproject(vec2f pos);

    void checkGLErrors();
};

extern SDLAppDisplay display;

#endif
