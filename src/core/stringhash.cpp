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

#include "stringhash.h"

int stringHash(std::string& str) {

    int val = 0;
    int n = str.size();

    for (int i = 0; i < n; i++) {
        val = val + str[i] * (31^(n-i));
    }

     if(val<0) {
         val = -val;
     }

//     unsigned long hash = 5381;
// 
//     size_t strlen = str.size();
//     for(size_t i=0;i<strlen;i++) {
//         hash = ((hash << 5) + hash) + str[i];
//     }
// 
//     int val = (int) hash;
//     if(val<0) {
//         val = -val;
//     }

    return val;
}

vec2f vec2Hash(std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;

    vec2f v = vec2f(x, y);
    v.normalize();

    return v;
}

vec3f vec3Hash(std::string& str) {
    int hash = stringHash(str);

    int x = ((hash/7) % 255) - 127;
    int y = ((hash/3) % 255) - 127;
    int z = hash % 255;

    vec3f v = vec3f(x, y, z);
    v.normalize();

    return v;
}

vec3f colourHash(std::string& str) {
    int hash = stringHash(str);

    int r = (hash/7) % 255;
    if(r<0) r=0;
    int g = (hash/3) % 255;
    if(g<0) g=0;
    int b = hash % 255;

    vec3f colour = vec3f(r, g, b);
    colour.normalize();

    return colour;
}

vec3f stylizedColourHash(std::string& str) {
    vec3f colour = colourHash(str);

    vec3f colourc;

    //how red
    float z = colour.z;
    float t = 0.5;

    if(z <= t) {
        float zp = z/t;

        colourc = vec3f(1.0, 1.0, 1.0) * zp + vec3f(0.6, 0.6, 0.6) * (1.0 - zp);
    } else {
        float zp = (z-t)/(1.0 - t);
        colourc = vec3f(1.0, 0.0, 0.0) * zp + vec3f(1.0, 1.0, 1.0) * (1.0 - zp);
    }

    return colour * 0.1 + colourc * 0.9;
}
