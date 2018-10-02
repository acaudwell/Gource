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
#include "core/vbo.h"

#include "gource_settings.h"

#include "spline.h"
#include "file.h"
#include "bloom.h"

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

    SplineEdge spline;

    vec4 col;

    vec2 spos;

    vec2 projected_pos;
    vec2 projected_spos;

    vec2 pos;
    vec2 vel;
    vec2 accel, prev_accel;

    float dir_area;

    bool visible;
    bool in_frustum;
    bool position_initialized;

    float since_node_visible;
    float since_last_file_change;
    float since_last_node_change;

    float file_area;
    float dir_radius;
    float dir_radius_sqrt;
    float parent_radius;

    int depth;

    int visible_count;

    vec3 screenpos;
    vec2 node_normal;

    void calcRadius();
    void calcColour();

    std::string commonPathPrefix(const std::string& str) const;

    void changePath(const std::string & abspath);

    void setInitialPosition();

    void drawEdge(RDirNode* child) const;
    void updateSplinePoint(float dt);
    void move(float dt);

    vec2 calcFileDest(int layer_no, int file_no);
    void updateFilePositions();

    void adjustDepth();
    void adjustPath();
    void drawDirName(FXFont& dirfont) const;
public:
    RDirNode(RDirNode* parent, const std::string & abspath);
    ~RDirNode();

    void printFiles();

    bool empty() const;

    bool isAnchor(RDirNode* node) const;

    RDirNode* getRoot();

    void fileUpdated(bool userInitiated);
    void nodeUpdated(bool userInitiated);

    void addVisible();
    bool isVisible();

    float getArea() const;

    int totalDirCount() const;
    int totalFileCount() const;

    int getTokenOffset() const;

    int dirCount() const;
    int fileCount() const;
    int visibleFileCount() const;
    bool noDirs() const;
    bool noFiles() const;

    bool prefixedBy(const std::string & path) const;

    const std::string & getPath() const;

    const vec2 & getNodeNormal() const;

    bool isParent(RDirNode* node) const;

    bool addFile(RFile* f);
    bool removeFile(RFile* f);

    int getDepth() const;

    const std::list<RDirNode*> & getChildren() const;

    void updateQuadItemBounds();

    float getParentRadius() const;
    float getRadius() const;
    float getRadiusSqrt() const;

    const std::list<RFile*>* getFiles() const { return &files; };
    void getFilesRecursive(std::list<RFile*>& files) const;

    vec3 averageFileColour() const;

    const vec4 & getColour() const;

    RDirNode* getParent() const;

    bool isDir(const std::string& path) const;
    void findDirs(const std::string& path, std::list<RDirNode*>& dirs);

    const vec2 & getPos() const;

    void calcEdges();

    const vec2 & getProjectedPos() const;
    const vec2 & getSPos() const;

    void setPos(const vec2 & pos);

    void rotate(float s, float c);
    void rotate(float s, float c, const vec2& centre);

    void setParent(RDirNode* parent);

    float distanceToParent() const;

    void addNode(RDirNode* node);

    void debug(int indent=0) const;

    void applyForceDir(RDirNode* dir);
    void applyForces(QuadTree &quadtree);

    void logic(float dt);

    void updateEdgeVBO(quadbuf& buffer) const;
    
    void drawEdges() const;
    void drawEdgeShadows() const;

    void checkFrustum(const Frustum & frustum);

    void updateFilesVBO(quadbuf& buffer, float dt) const;
    void updateBloomVBO(bloombuf& buffer, float dt);

    void drawShadows(float dt) const;
    void drawFiles(float dt) const;
    void drawBloom(float dt);

    void drawNames(FXFont& dirfont);

    void calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection);

    void nodeCount() const;
};

class DirForceFunctor : public VisitFunctor<QuadItem>{
  private:
    RDirNode * this_dir;
    std::set<RDirNode*> seen;
    size_t loopCount;

  public:
    DirForceFunctor(RDirNode * dir) : this_dir(dir), seen(), loopCount(0){}
    int getLoopCount() const{ return loopCount; }
    void operator()(QuadItem * item){

        std::set<RDirNode*>::iterator seentest;
        RDirNode* d = (RDirNode*) (item);

        if(d==this_dir) return;
        if(d==this_dir->getParent()) return;
        if(d->getParent() == this_dir) return;
        if(this_dir->isParent(d)) return;
        if(d->isParent(this_dir)) return;

        if(d->node_count != 1) {
            if((seentest = seen.find(d)) != seen.end())
                return;

            seen.insert(d);
        }

        this_dir->applyForceDir(d);

        loopCount++;

    }

};

extern int gGourceDirNodeInnerLoops;
extern int gGourceFileInnerLoops;

extern float gGourcePointSize;
extern bool  gGourceNodeDebug;
extern bool  gGourceGravity;
extern float gGourceForceGravity;

extern std::map<std::string, RDirNode*> gGourceDirMap;

#endif
