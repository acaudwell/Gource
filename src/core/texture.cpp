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

#include "texture.h"

TextureManager texturemanager;

// texture manager

TextureManager::TextureManager() : ResourceManager() {
}

TextureResource* TextureManager::grab(std::string name, int mipmaps, int clamp, int trilinear, bool external_file) {
    //debugLog("grabing %s\n", name.c_str());
    Resource* r = resources[name];

    if(r==0) {
        //debugLog("%s not found. creating resource...\n", name.c_str());

        r = new TextureResource(name, mipmaps, clamp, trilinear, external_file);

        resources[name] = r;
    }
    r->addref();
    return (TextureResource*)r;
}

// texture resource

TextureResource::TextureResource(std::string file, int mipmaps, int clamp, int trilinear, bool external_file) : Resource(file) {

    //if doesnt have an absolute path, look in resource dir
    if(!external_file && !(file.size() > 2 && file[1] == ':') && !(file.size() > 1 && file[0] == '/')) {
        file = texturemanager.getDir() + file;
    }

    debugLog("creating texture from %s\n", file.c_str());

    SDL_Surface *surface = IMG_Load(file.c_str());

    if(surface==0) throw TextureException(file);

    w = surface->w;
    h = surface->h;

    //figure out image colour order
    int format = colourFormat(surface);

    if(format==0) throw TextureException(file);

    textureid = display.createTexture(w, h, mipmaps, clamp, trilinear, format, (unsigned int*) surface->pixels);
}

int TextureResource::colourFormat(SDL_Surface* surface) {

    int colours = surface->format->BytesPerPixel;
    int format  = 0;

    if (colours == 4) {
        if (surface->format->Rmask == 0x000000ff) {
            format = GL_RGBA;
        } else {
            format = GL_BGRA;
        }
    } else if (colours == 3) {
        if (surface->format->Rmask == 0x000000ff)
            format = GL_RGB;
        else
            format = GL_BGR;
    }

    return format;
}

TextureResource::~TextureResource() {
    if(textureid!=0) glDeleteTextures(1, &textureid);
}
