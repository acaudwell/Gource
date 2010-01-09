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

#include "core/display.h"

class FrameExporter {
protected:
    SDL_Surface *surface;
    char *surfacepixels, *pixels;

    size_t rowstride;
public:
    FrameExporter();
    virtual ~FrameExporter();
    void dump();
    virtual void dumpImpl() {};
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

