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

#ifndef FX_FONT_H
#define FX_FONT_H

#include "display.h"
#include "vectors.h"
#include "logger.h"

#include "resource.h"

#include <string>
#include <map>

#include <FTGL/ftgl.h>

class FXFontException : public ResourceException {
public:
    FXFontException(std::string& font_file) : ResourceException(font_file) {}
};

class FXFont {

    FTFont* ft;

    bool shadow;
    bool round;

    float shadow_strength;
    vec2f shadow_offset;

    bool align_right, align_top;

    void render(float x, float y, std::string text);
    void init();
public:
    FXFont();
    FXFont(FTFont* ft);

    FTFont* getFTFont();

    void print(float x, float y, const char *str, ...);
    void draw(float x, float y, std::string text);

    float getWidth(std::string text);

    void alignTop(bool top);
    void alignRight(bool right);

    void roundCoordinates(bool round);

    int getFontSize();

    float getHeight();

    void dropShadow(bool shadow);
    void shadowStrength(float s);
    void shadowOffset(float x, float y);
};

class FXFontManager {

    std::string font_dir;

    std::map<std::string, FTFont*> fonts;

    FTFont* create(std::string font_file, int size);
public:
    void setDir(std::string font_dir);
    void purge();
    FXFont grab(std::string font_file, int size);
};

extern FXFontManager fontmanager;

#endif
