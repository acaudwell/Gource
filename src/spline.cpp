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

void SplineEdge::update(const vec2f& pos1, const vec4f& col1, const vec2f& pos2, const vec4f& col2, const vec2f& spos) {

    vec2f pt_last;
    vec4f col_last;

    vec2f mid = (pos1 - pos2) * 0.5;
    vec2f to  = vec2f(pos1 - spos);

    float dp = std::min(1.0f, to.normal().dot(mid.normal()));

    float ang = acos(dp) / PI;

    int edge_detail = std::min(10, (int) (ang * 100.0));

    if(edge_detail<1) edge_detail = 1;

    spline_point.clear();
    spline_colour.clear();

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

    midpoint = pos1 * 0.25 + pos2 * 0.25 + spos * 0.5;
}

const vec2f& SplineEdge::getMidPoint() const {
    return midpoint;
}

void SplineEdge::drawToVBO(quadbuf& buffer) const {

    int edges_count = spline_point.size() - 1;

    for(int i=0; i < edges_count; i++) {

        vec2f perp = (spline_point[i] - spline_point[i+1]).perpendicular().normal() * 2.5f;

        quadbuf_vertex v1(spline_point[i]   + perp, spline_colour[i],   vec2f(1.0f, 0.0f));
        quadbuf_vertex v2(spline_point[i]   - perp, spline_colour[i],   vec2f(0.0f, 0.0f));
        quadbuf_vertex v3(spline_point[i+1] - perp, spline_colour[i+1], vec2f(0.0f, 0.0f));
        quadbuf_vertex v4(spline_point[i+1] + perp, spline_colour[i+1], vec2f(1.0f, 0.0f));

        buffer.add(0, v1, v2, v3, v4);
    }
}

void SplineEdge::drawBeam(const vec2f & pos1, const vec4f & col1, const vec2f & pos2, const vec4f & col2, float radius, bool first) const{

    vec2f perp = (pos1 - pos2).perpendicular().normal() * radius;

    // src point
    if(first) {
        glColor4fv(col1);
        glTexCoord2f(1.0,0.0);
        glVertex2f(pos1.x + perp.x, pos1.y + perp.y);
        glTexCoord2f(0.0,0.0);
        glVertex2f(pos1.x - perp.x, pos1.y - perp.y);
    }

    // dest point
    glColor4fv(col2);
    glTexCoord2f(1.0,0.0);
    glVertex2f(pos2.x + perp.x, pos2.y + perp.y);
    glTexCoord2f(0.0,0.0);
    glVertex2f(pos2.x - perp.x, pos2.y - perp.y);
}

void SplineEdge::drawShadow() const{

    int edges_count = spline_point.size() - 1;

    vec2f offset(2.0, 2.0);

    glBegin(GL_QUAD_STRIP);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i] + offset, vec4f(0.0, 0.0, 0.0, gGourceShadowStrength), spline_point[i+1] + offset, vec4f(0.0, 0.0, 0.0, gGourceShadowStrength), 2.5, i==0);
    }

    glEnd();
}

void SplineEdge::draw() const{

    int edges_count = spline_point.size() - 1;

    glBegin(GL_QUAD_STRIP);

    for(int i=0;i<edges_count;i++) {
        drawBeam(spline_point[i], spline_colour[i], spline_point[i+1], spline_colour[i+1], 2.5, i==0);
    }

    glEnd();
}
