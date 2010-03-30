/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

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

#ifndef SPLINE_EDGE_H
#define SPLINE_EDGE_H

#include "core/display.h"
#include "core/vectors.h"
#include "core/pi.h"

#include "pawn.h"

#include <vector>

class SplineEdge {

    std::vector<vec2f> spline_point;
    std::vector<vec4f> spline_colour;

    void drawBeam(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, float radius, bool first);
public:
    SplineEdge();
    SplineEdge(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos);

    void drawShadow();
    void draw();
};

#endif
