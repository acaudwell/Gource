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

#ifndef RDIRNODE_H
#define RDIRNODE_H

#include "core/sdlapp.h"
#include "core/bounds.h"
#include "core/quadtree.h"
#include "core/pi.h"

#include "spline.h"
#include "file.h"

#include <list>
#include <set>

class RFile;

class RDirNode : public QuadItem {
    std::string abspath;

    std::string path_token;
    int         path_token_offset;

    RDirNode* parent;
    std::list<RDirNode*> children;
    std::list<RFile*> files;

    std::map<RDirNode*, SplineEdge> splines;

    vec4f col;

    vec2f spos;

    vec2f projected_pos;
    vec2f projected_spos;

    vec2f pos;
    vec2f vel;
    vec2f accel, prev_accel;

    float dir_area;

    bool visible;
    bool position_initialized;

    float since_node_visible;
    float since_last_file_change;
    float since_last_node_change;


    float file_area;
    float dir_radius;
    float dir_radius_sqrt;

    int depth;

    int visible_count;

    void calcArea();
    void calcRadius();
    void calcColour();

    std::string commonPathPrefix(std::string& str);

    vec2f node_normal;

    void changePath(std::string abspath);

    void calcProjectedPos();

    void setInitialPosition();

    void drawBeam(vec2f pos_src, vec4f col_src, vec2f pos_dest, vec4f col_dest, float beam_radius);
    void drawEdge(RDirNode* child);
    void updateSpline(float dt);
    void move(float dt);

    vec2f calcFileDest(int layer_no, int file_no);
    void updateFilePositions();

    void adjustPath();
    void drawDirName(FXFont& dirfont);
public:
    RDirNode(RDirNode* parent, std::string abspath);
    ~RDirNode();

    bool empty();

    bool isAnchor(RDirNode* node);

    RDirNode* getRoot();

    void fileUpdated(bool userInitiated);
    void nodeUpdated(bool userInitiated);

    void addVisible();
    bool isVisible();

    float getArea();

    int totalDirCount();
    int totalFileCount();

    int getTokenOffset();

    int dirCount();
    int fileCount();
    int visibleFileCount();

    bool prefixedBy(std::string path);

    std::string getPath();

    vec2f getNodeNormal();

    bool isParentOf(RDirNode* node);

    bool addFile(RFile* f);
    bool removeFile(RFile* f);

    int getDepth();

    std::list<RDirNode*>* getChildren();

    void updateQuadItemBounds();

    float getRadius();
    float getRadiusSqrt();

    vec3f averageFileColour();

    vec4f getColour();

    RDirNode* getParent();

    vec2f getPos();

    void calcEdges();

    vec2f getProjectedPos();
    vec2f getSPos();

    void setPos(vec2f pos);

    void setParent(RDirNode* parent);

    float distanceTo(RDirNode* node);

    void addNode(RDirNode* node);

    void debug(int indent=0);

    void applyForceDir(RDirNode* dir);

    void applyForces(QuadTree &quadtree);

    void logic(float dt, Bounds2D& bounds);

    void drawEdges(float dt);
    void drawEdgeShadows(float dt);

    void drawShadows(Frustum& frustum, float dt);
    void drawFiles(Frustum& frustum, float dt);
    void drawSimple(Frustum& frustum, float dt);
    void drawNames(FXFont& dirfont, Frustum& frustum);

    void nodeCount();
};

extern int gGourceDirNodeInnerLoops;
extern int gGourceFileInnerLoops;

extern float gGourcePointSize;
extern bool  gGourceNodeDebug;
extern bool  gGourceGravity;
extern float gGourceForceGravity;
extern bool  gGourceDrawDirName;

extern float gGourceElasticity;

extern std::map<std::string, RDirNode*> gGourceDirMap;

#endif
