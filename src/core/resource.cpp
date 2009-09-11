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

#include "resource.h"

ResourceManager::ResourceManager() {
}

std::string ResourceManager::getDir() {
    return resource_dir;
}

void ResourceManager::setDir(std::string resource_dir) {
    this->resource_dir = resource_dir;
}

void ResourceManager::purge() {
    //free all resources - should be called BEFORE SDL_QUIT ...
    for(std::map<std::string, Resource*>::iterator it= resources.begin(); it!=resources.end();it++) {
        delete it->second;
    }

    resources.clear();
}

ResourceManager::~ResourceManager() {
}

Resource* ResourceManager::grab(std::string name) {
    //debugLog("grabing %s\n", name.c_str());
    Resource* r = resources[name];

    if(r==0) {
        //debugLog("%s not found. creating resource...\n", name.c_str());
        r = create(name);
        resources[name] = r;
    }
    r->addref();
    return r;
}

void ResourceManager::release(Resource* resource) {
    Resource* r = resources[resource->name];

    if(r==0) return;

    //debugLog("decrementing ref count for %s\n", name.c_str());
    r->deref();

    if(r->refcount()<=0) {
        //debugLog("no refs to %s, deleting\n", name.c_str());
        resources.erase(r->name); //sufficient?
        delete r;
    }
}

