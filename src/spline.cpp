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

#include "spline.h"

SplineEdge::SplineEdge() {
}

SplineEdge::SplineEdge(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, vec2f spos) {

    vec2f pt_last;
    vec4f col_last;

    vec2f mid = (pos1 - pos2) * 0.5;
    vec2f to  = vec2f(pos1 - spos);

    float dp = std::min(1.0f, to.normal().dot(mid.normal()));

    float ang = acos(dp) / PI;

    // max 10,
    int max_detail = 10;

    int edge_detail = std::min(max_detail, (int) (ang * 100.0));
    if(edge_detail<1.0) edge_detail = 1.0;

    spline_point.reserve(edge_detail+1);
    spline_colour.reserve(edge_detail+1);

    //calculate positions
    for(int i=0; i <= edge_detail; i++) {
        float t = (float)i/edge_detail;
        float tt = 1.0f-t;

        vec2f p0 = pos1 * t + spos * tt;
        vec2f p1 = spos * t + pos2 * tt;

        vec2f pt = p0 * t + p1 * tt;

        vec4f coln = col1 * t + col2 * tt;

        spline_point.push_back(pt);
        spline_colour.push_back(coln);
    }
}

void SplineEdge::drawBeam(vec2f pos1, vec4f col1, vec2f pos2, vec4f col2, float radius) {

    vec2f perp = (pos1 - pos2).perpendicular().normal() * radius;

    // src point
    glColor4fv(col1);
    glTexCoord2f(0.0,0.0);
    glVertex2f(pos1.x - perp.x, pos1.y - perp.y);
    glTexCoord2f(1.0,0.0);
    glVertex2f(pos1.x + perp.x, pos1.y + perp.y);

    // dest point
    glColor4fv(col2);
    glTexCoord2f(1.0,0.0);
    glVertex2f(pos2.x + perp.x, pos2.y + perp.y);
    glTexCoord2f(0.0,0.0);
    glVertex2f(pos2.x - perp.x, pos2.y - perp.y);
}

void SplineEdge::drawShadow() {

    int edges_count = spline_point.size() - 1;

    vec2f offset(2.0, 2.0);

    glBegin(GL_QUADS);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i] + offset, vec4f(0.0, 0.0, 0.0, gGourceShadowStrength), spline_point[i+1] + offset, vec4f(0.0, 0.0, 0.0, gGourceShadowStrength), 2.5);
    }

    glEnd();
}

void SplineEdge::draw() {

    int edges_count = spline_point.size() - 1;

    glBegin(GL_QUADS);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i], spline_colour[i], spline_point[i+1], spline_colour[i+1], 2.5);
    }

    glEnd();
}