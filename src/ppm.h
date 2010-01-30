/*
    Copyright (C) 2009 Johannes Schindelin (johannes.schindelin@gmx.de)

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

#ifndef PPM_FRAME_EXPORTER_H
#define PPM_FRAME_EXPORTER_H

#include <iostream>
#include <fstream>
#include <ostream>

#include "SDL_thread.h"

#include "core/display.h"

enum { FRAME_EXPORTER_WAIT,
       FRAME_EXPORTER_DUMP,
       FRAME_EXPORTER_EXIT };

class FrameExporter {
protected:

    char* pixels1;
    char* pixels2;
    char* pixels_shared_ptr;

    size_t rowstride;

    GLuint screentex;

    SDL_Thread* thread;
    SDL_mutex* mutex;
    SDL_cond* cond;
    int dumper_thread_state;

public:
    FrameExporter();
    virtual ~FrameExporter();
    void dump();
    void dumpThr();
    virtual void dumpImpl() {};
};

class PPMExporterException : public std::exception {
protected:
    std::string filename;
public:
    PPMExporterException(std::string& filename) : filename(filename) {}
    virtual ~PPMExporterException() throw () {};

    virtual const char* what() const throw() { return filename.c_str(); }
};

class PPMExporter : public FrameExporter {
protected:
    std::ostream* output;
    std::string filename;
    char ppmheader[1024];

public:
    PPMExporter(std::string outputfile);
    virtual ~PPMExporter();
    virtual void dumpImpl();
};


#endif
