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

#include "dirnode.h"

float gGourceMinDirSize   = 15.0;

float gGourceForceGravity = 10.0;
float gGourceDirPadding   = 1.5;

bool  gGourceNodeDebug    = false;
bool  gGourceGravity      = true;

//debugging
int  gGourceDirNodeInnerLoops = 0;
int  gGourceFileInnerLoops = 0;

std::map<std::string, RDirNode*> gGourceDirMap;

RDirNode::RDirNode(RDirNode* parent, const std::string & abspath) {

    changePath(abspath);

    parent = 0;
    setParent(parent);

    accel = spos = prev_accel = vel = vec2(0.0f);

    //NOTE: parent is always being set to 0 so this never gets called ...

    //figure out starting position
    if(parent !=0) {
        vec2 parentPos = parent->getPos();
        vec2 offset;

        pos = parentPos;
    } else {
        pos = vec2(0.0f, 0.0f);
    }

    float padded_file_radius  = gGourceFileDiameter * 0.5;

    file_area  = padded_file_radius * padded_file_radius * PI;

    visible_count = 0;

    visible = false;
    position_initialized = false;

    since_node_visible = 0.0;
    since_last_file_change = 0.0;
    since_last_node_change = 0.0;

    calcRadius();
    calcColour();
}

void RDirNode::changePath(const std::string & abspath) {
    //fix up path

    gGourceDirMap.erase(this->abspath);
    this->abspath = abspath;

    if(abspath.empty() || abspath[abspath.size()-1] != '/') {
        this->abspath += std::string("/");
    }

    //debugLog("new dirnode %s\n", abspath.c_str());

    gGourceDirMap[this->abspath] = this;
}

RDirNode::~RDirNode() {
    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        delete (*it);
    }

    gGourceDirMap.erase(abspath);
}

int RDirNode::getTokenOffset() const{
    return path_token_offset;
}

void RDirNode::fileUpdated(bool userInitiated) {
    calcRadius();

    since_last_file_change = 0.0;

    nodeUpdated(userInitiated);
}

void RDirNode::nodeUpdated(bool userInitiated) {
    if(userInitiated) since_last_node_change = 0.0;

    calcRadius();
    updateFilePositions();
    if(visible && noDirs() && noFiles()) visible = false;
    if(parent !=0) parent->nodeUpdated(true);
}

void RDirNode::rotate(float s, float c) {
    pos  = rotate_vec2(pos,  s, c);
    spos = rotate_vec2(spos, s, c);

    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = (*it);
        child->rotate(s, c);
    }
}

void RDirNode::rotate(float s, float c, const vec2& centre) {

    pos  = rotate_vec2(pos - centre,  s, c) + centre;
    spos = rotate_vec2(spos - centre, s, c) + centre;

    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = (*it);
        child->rotate(s, c, centre);
    }
}

void RDirNode::setPos(const vec2 & pos) {
    this->pos = pos;
}

//returns true if supplied path prefixes the nodes path
bool RDirNode::prefixedBy(const std::string & path) const {
    if(path.empty()) return false;

    if(path[path.size()-1] != '/')
        return abspath.find(path + std::string("/")) == 0;
    else
        return abspath.find(path) == 0;
}

const std::string & RDirNode::getPath() const{
    return abspath;
}

RDirNode* RDirNode::getParent() const{
    return parent;
}


bool RDirNode::isDir(const std::string& path) const {

    if(prefixedBy(path)) return true;

    if(path.find(abspath) != 0) return false;

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        if((*it)->isDir(path)) return true;
    }

    return false;
}

//finds directories closest to the root directory prefixed by path (eg foo/ may match just foo/ or could also match foo/bar1, foo/bar2, ... if foo/ doesn't exist).
void RDirNode::findDirs(const std::string& path, std::list<RDirNode*>& dirs) {

    if(prefixedBy(path)) {
        dirs.push_back(this);
        return;
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        (*it)->findDirs(path, dirs);
    }
}

void RDirNode::getFilesRecursive(std::list<RFile*>& files) const {

    //add this dirs files
    files.insert(files.begin(), this->files.begin(), this->files.end());

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        (*it)->getFilesRecursive(files);
    }
}

int RDirNode::getDepth() const{
    return depth;
}

void RDirNode::adjustPath() {

    //update display path
    int parent_token_offset = 0;

    path_token_offset = abspath.size();

    if(parent != 0) {
        parent_token_offset  = parent->getTokenOffset();

        //debugLog("abspath.substr arguments: %d %d %s size = %d\n", parent_token_offset, abspath.size()-parent_token_offset-1, abspath.c_str(), abspath.size());

        path_token        = abspath.substr(parent_token_offset, abspath.size()-parent_token_offset-1);
        path_token_offset = abspath.size();

        //debugLog("new token %s\n", path_token.c_str());
    }
}

void RDirNode::setParent(RDirNode* parent) {
    if(parent != 0 && this->parent == parent) return;

    this->parent = parent;

    adjustPath();
    adjustDepth();
}

void RDirNode::adjustDepth() {

    if(parent == 0) {
        depth = 1;
    } else {
        depth = parent->getDepth() + 1;
    }

    for(RDirNode* child : children) {
        child->adjustDepth();
    }
}

void RDirNode::addNode(RDirNode* node) {
    // does this node prefix any other nodes, if so, add them to it

    std::vector<RDirNode*> matches;
    std::string path = node->getPath();

    //debugLog("adding node %s to %s\n", path.c_str(), abspath.c_str());

    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); ) {
        RDirNode* child = (*it);

        if(child->prefixedBy(path)) {
            it = children.erase(it);
            node->addNode(child);
        } else {
            it++;
        }
    }

    // add to this node
    children.push_back(node);
    node->setParent(this);

    //debugLog("added node %s to %s\n", node->getPath().c_str(), getPath().c_str());

    nodeUpdated(false);
}

RDirNode* RDirNode::getRoot() {
    if(parent==0) return this;
    return parent->getRoot();
}

// note - you still need to delete the file yourself
bool RDirNode::removeFile(RFile* f) {
    //doesnt match this path at all
    if(f->path.find(abspath) != 0) {
        return false;
    }

    //is this dir - add to this node
    if(f->path.compare(abspath) == 0) {

        for(std::list<RFile*>::iterator it = files.begin(); it != files.end(); it++) {
            if((*it)==f) {
                files.erase(it);
                if(!f->isHidden()) visible_count--;

                fileUpdated(false);

                return true;
            }
        }

        return false;
    }

    //does this belong to one of the children ?
    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        bool removed = node->removeFile(f);

        if(removed) {
            //fprintf(stderr, "%s file removed from a child. child file count=%d, dir count =%d\n", getPath().c_str(), node->fileCount(), node->dirCount());
            //node->printFiles();

            //node is now empty, reap!
            if(node->noFiles() && node->noDirs()) {
                children.erase(it);
                //fprintf(stderr, "deleting node %s from %s\n", node->getPath().c_str(), getPath().c_str());
                delete node;
                nodeUpdated(false);
            }

            return true;
        }
    }

    return false;
}


void RDirNode::printFiles() {
    for(std::list<RFile*>::iterator it = files.begin(); it != files.end(); it++) {
        RFile* file = (*it);
        fprintf(stderr, "%s: %s %s\n", getPath().c_str(), file->fullpath.c_str() , file->isHidden() ? "hidden " : "");
    }
}


void RDirNode::addVisible() {
    visible_count++;
    visible = true;
}

bool RDirNode::isVisible() {

    if(visible) return true;

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        if((*it)->isVisible()) {
            visible = true;
            return true;
        }
    }

    return false;
}

int RDirNode::visibleFileCount() const{
    return visible_count;
}

int RDirNode::fileCount() const{
    return files.size();
}

bool RDirNode::noFiles() const{
    return files.empty();
}

std::string RDirNode::commonPathPrefix(const std::string & str) const{
    size_t c = 0;
    int slash = -1;

    while(c<abspath.size() && c<str.size() && abspath[c] == str[c]) {
        if(abspath[c] == '/') {
            slash = c;
        }
        c++;
    }

    if(slash==-1) return "";
    return str.substr(0,slash+1);
}

bool RDirNode::addFile(RFile* f) {

    //doesnt match this path at all
    if(f->path.find(abspath) != 0) {

        if(parent != 0) return false;

        //if this is the root node (ie no parent), we fork it
        //if we encounter a file with a non matching path to the
        //current root path. the calling process then checks if
        //the root now has a parent node, and changes the pointer.

        RDirNode* newparent;

        std::string common = commonPathPrefix(f->path);
        if(common.size()==0) common = "/";

        newparent = new RDirNode(0, common);
        newparent->addNode(this);
        return newparent->addFile(f);
    }

    //simply change path of node and add this to it
    if(   parent==0 && abspath == "/"
       && f->path.compare(abspath) != 0 && noFiles() && noDirs()) {
        debugLog("modifying root path to %s", f->path.c_str());
        changePath(f->path);
    }

    //is this dir - add to this node
    if(f->path.compare(abspath) == 0) {
        //debugLog("addFile %s to %s\n", f->fullpath.c_str(), abspath.c_str());

        files.push_back(f);
        if(!f->isHidden()) visible_count++;
        f->setDir(this);

        fileUpdated(false);

        return true;
    }

    bool added = false;

    //does this belong to one of the children ?
    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child =  (*it);

        added = child->addFile(f);

        if(added) break;
    }

    if(added && parent != 0) return true;

    //do we have a file in this directory thats fullpath is a prefix of this file, if so
    //that file is actually a directory - the file should be removed, and a directory with that path added
    //if this is the root node we do this regardless of if the file was added to a child node

    for(std::list<RFile*>::const_iterator it = files.begin(); it != files.end(); it++) {
        RFile* file = (*it);

        if(f->path.find(file->fullpath) == 0) {
            //fprintf(stderr, "removing %s as is actually the directory of %s\n", file->fullpath.c_str(), f->fullpath.c_str());
            file->remove();
            break;
        }
    }

    if(added) return true;

    //add new child, add it to that
    //if commonpath is longer than abspath, add intermediate node, else just add at the files path
    RDirNode* node = new RDirNode(this, f->path);

    node->addFile(f);

    addNode(node);

    // do we have dir nodes, with a common path element greater than abspath,
    // if so create another node, and move those nodes there

     std::string commonpath;
     vec2 commonPos;
     for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
         RDirNode* child =  (*it);

         std::string common = child->commonPathPrefix(f->path);
         if(common.size() > abspath.size() && common != f->path) {
            commonpath = common;
            commonPos = child->getPos();
            break;
         }
     }

    // redistribute to new common node
    if(commonpath.size() > abspath.size()) {
        //debugLog("common path %s\n", commonpath.c_str());

        RDirNode* cnode = new RDirNode(this, commonpath);
        cnode->setPos(commonPos);

        for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end();) {
            RDirNode* child =  (*it);

            if(child->prefixedBy(commonpath)) {
                //debugLog("this path = %s, commonpath = %s, path = %s\n", abspath.c_str(), commonpath.c_str(), child->getPath().c_str());
                it = children.erase(it);
                cnode->addNode(child);
                continue;
            }

            it++;
        }

        addNode(cnode);
    }

    return true;
}

float RDirNode::getParentRadius() const{
    return parent_radius;
}

float RDirNode::getRadius() const{
    return dir_radius;
}

float RDirNode::getRadiusSqrt() const{
    return dir_radius_sqrt;
}

vec3 RDirNode::averageFileColour() const{

    vec3 av;
    int count = 0;

    for(std::list<RFile*>::const_iterator it = files.begin(); it != files.end(); it++) {
        RFile* file = (*it);

        if(file->isHidden()) continue;

        av += file->getColour();

        count++;
    }

    if(count>0) av *= (1.0f/(float)count);

    count = 0;

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end();it++) {
            RDirNode* child =  (*it);

            av += child->averageFileColour();
            count++;
    }

    if(count>0) av *= (1.0f/(float)count);

    return av;
}

const vec4 & RDirNode::getColour() const{
    return col;
}

void RDirNode::calcColour() {

    // make branch brighter if recently accessed
    float brightness = std::max(0.6f, 1.0f - std::min(1.0f, since_last_node_change / 3.0f));

    col = vec4(brightness, brightness, brightness, 1.0);

    int fcount = 0;

    for(std::list<RFile*>::iterator it = files.begin(); it != files.end(); it++) {
        RFile* file = (*it);

        if(file->isHidden()) continue;;

        vec3 filecol = file->getColour() * brightness;
        float a       = file->getAlpha();

        col += vec4(filecol.x, filecol.y, filecol.z, a);

        fcount++;
    }

    this->col /= (float) fcount + 1.0;
}

float RDirNode::getArea() const{
    return dir_area;
}

void RDirNode::calcRadius() {

    float total_file_area = file_area * visible_count;

    dir_area = total_file_area;

    //float parent_circ        = 0.0;

    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);

        dir_area += node->getArea();
    //    parent_circ += node->getRadiusSqrt();
    }

    this->dir_radius = std::max(1.0f, (float)sqrt(dir_area)) * gGourceDirPadding;
    //this->dir_radius_sqrt = sqrt(dir_radius); //dir_radius_sqrt is not used

//    this->parent_radius = std::max(1.0, parent_circ / PI);
    this->parent_radius = std::max(1.0f, (float) sqrt(total_file_area) * gGourceDirPadding);
}

float RDirNode::distanceToParent() const{

    float posd     = glm::length(parent->getPos() - pos);
    float distance = posd - (dir_radius + parent->getParentRadius());

    return distance;
}

void RDirNode::applyForceDir(RDirNode* node) {
    if(node == this) return;

    vec2 dir = node->getPos() - pos;

    float posd2       = glm::length2(dir);
    float myradius    = getRadius();
    float your_radius = node->getRadius();

    float sumradius = (myradius + your_radius);

    float distance2 = posd2 - sumradius*sumradius;

    if(distance2>0.0) return;

    float posd = sqrt(posd2);

    float distance = posd - myradius - your_radius;

    //resolve overlap
    if(posd < 0.00001) {
        accel += normalise(vec2( (rand() % 100) - 50, (rand() % 100) - 50));
        return;
    }

    accel += distance * normalise(dir);
}

const vec2 & RDirNode::getPos() const{
    return pos;
}

bool RDirNode::isParent(RDirNode* node) const {
    if(node==parent) return true;
    if(parent==0) return false;

    return parent->isParent(node);
}

bool RDirNode::empty() const{
    return (visible_count==0 && noDirs()) ? true : false;
}

void RDirNode::applyForces(QuadTree & quadtree) {

    //child nodes
    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);

        node->applyForces(quadtree);
    }

    if(parent == 0) return;

    DirForceFunctor dff(this);
    quadtree.visitItemsInBounds(quadItemBounds, dff);
    gGourceDirNodeInnerLoops += dff.getLoopCount();

    //always call on parent no matter how far away
    applyForceDir(parent);

    //pull towards parent
    float parent_dist = distanceToParent();

    //  * dirs should attract to sit on the radius of the parent dir ie:
    //    should attract to distance_to_parent * normal_to_parent



    accel += gGourceForceGravity * parent_dist * normalise(parent->getPos() - pos);

    //  * dirs should be pushed along the parent_parent to parent normal by a force smaller than the parent radius force
    RDirNode* pparent = parent->getParent();

    if(pparent != 0) {
        vec2 parent_edge = (parent->getPos() - pparent->getPos());
        vec2 parent_edge_normal = normalise(parent_edge);

        vec2 dest = (parent->getPos() + (parent->getRadius() + getRadius()) * parent_edge_normal) - pos;

        accel += dest;
    }

    //  * dirs should repulse from other dirs of this parent
    const std::list<RDirNode*> & siblings = parent->getChildren();
    if(!siblings.empty()) {
        vec2 sib_accel;

        int visible = 1;

        for(std::list<RDirNode*>::const_iterator it = siblings.begin(); it != siblings.end(); it++) {
            RDirNode* node = (*it);

            if(node == this) continue;
            if(!node->isVisible()) continue;

            visible++;

            sib_accel -= normalise(node->getPos() - pos);
        }

        //parent circumfrence divided by the number of visible child nodes
        if(visible>1) {
            float slice_size = (parent->getRadius() * PI) / (float) (visible+1);
            sib_accel *= slice_size;

            accel += sib_accel;
        }
    }

}

void RDirNode::debug(int indent) const{
    std::string indentstr;
    while(indentstr.size() < indent) indentstr += " ";

    debugLog("%s%s", indentstr.c_str(), abspath.c_str());

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->debug(indent+1);
    }
}

int RDirNode::totalFileCount() const{
    int total = visibleFileCount();

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        total += node->visibleFileCount();
    }

    return total;
}

int RDirNode::totalDirCount() const{
    int total = 1;

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        total += node->totalDirCount();
    }

    return total;
}

int RDirNode::dirCount() const{
    return children.size();
}

bool RDirNode::noDirs() const{
    return children.empty();
}

const std::list<RDirNode*> & RDirNode::getChildren() const{
    return children;
}

void RDirNode::updateSplinePoint(float dt) {
    if(parent == 0) return;

    //update the spline point
    vec2 td = (parent->getPos() - pos) * 0.5f;

    vec2 mid = pos + td;// - td.perpendicular() * pos.normal();// * 10.0;

    vec2 delta = (mid - spos);

    //dont let spos get more than half the length of the distance behind
    if(glm::length2(delta) > glm::length2(td)) {
        spos += normalise(delta) * (glm::length(delta) - glm::length(td));
    }

    spos += delta * std::min(1.0f, dt * 2.0f);
}

void RDirNode::setInitialPosition() {
    RDirNode* parentP = parent->getParent();

    pos = parent->getPos();

    //offset position by some pseudo-randomness
    if(parentP != 0) {
        //pos += ((parent->getPos() - parentP->getPos()).normal() * 2.0 + vec2Hash(abspath)).normal();
        pos += normalise(normalise(parent->getPos() - parentP->getPos()) * 2.0f + vec2Hash(abspath));

    }  else {
        pos += vec2Hash(abspath);
    }

    //the spline point
    spos = pos - (parent->getPos() - pos) * 0.5f;
    position_initialized=true;
}

void RDirNode::move(float dt) {

    //the root node is the centre of the world
    if(parent == 0) {
        return;
    }

    //initial position
    if(!empty() && !position_initialized) {
        setInitialPosition();
    }

    pos += accel * dt;

    if(gGourceSettings.elasticity>0.0) {
        vec2 diff = (accel - prev_accel);

        float m = dt * gGourceSettings.elasticity;

        vec2 accel3 = prev_accel * (1.0f-m) + diff * m;
        pos += accel3;
        prev_accel = accel3;
    }

    //accel = accel * std::max(0.0f, (1.0f - dt*10.0f));
    accel = vec2(0.0, 0.0);
}

const vec2 & RDirNode::getNodeNormal() const{
    return node_normal;
}

vec2 RDirNode::calcFileDest(int max_files, int file_no) {

    float arc   = 1.0/(float)max_files;

    float frac = arc * 0.5 + arc * file_no;

    vec2 dest = vec2(sinf(frac*PI*2.0), cosf(frac*PI*2.0));

    return dest;
}

void RDirNode::updateFilePositions() {

    int max_files = 1;
    int diameter  = 1;
    int file_no   = 0;
    float d = 0.0;

    int files_left = visible_count;

    for(std::list<RFile*>::iterator it = files.begin(); it!=files.end(); it++) {
        RFile* f = *it;

        if(f->isHidden()) {
            f->setDest(vec2(0.0,0.0));
            f->setDistance(0.0f);
            continue;
        }

        vec2 dest = calcFileDest(max_files, file_no);

        f->setDest(dest);
        f->setDistance(d);

        files_left--;
        file_no++;

        if(file_no>=max_files) {
            diameter++;
            d += gGourceFileDiameter;
            max_files = (int) std::max(1.0, diameter*PI);

            if(files_left<max_files) {
                max_files = files_left;
            }

            file_no=0;
        }
    }
}

void RDirNode::calcEdges() {

    if(parent != 0) {
        spline.update(parent->getProjectedPos(), parent->getColour(), projected_pos, col, projected_spos);
    }

    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = *it;

        child->calcEdges();
    }
}

void RDirNode::logic(float dt) {

    //move
    move(dt);
    updateSplinePoint(dt);

    //update node normal
    if(parent != 0) {
        node_normal = normalise(pos - parent->getPos());
    }

    //update files
     for(std::list<RFile*>::iterator it = files.begin(); it!=files.end(); it++) {
         RFile* f = *it;

         f->logic(dt);
     }

    //update child nodes
    for(std::list<RDirNode*>::iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);

        node->logic(dt);
    }

    //update colour
    calcColour();

    //update tickers
    if(visible) since_node_visible += dt;

    since_last_file_change += dt;
    since_last_node_change += dt;
}

void RDirNode::drawDirName(FXFont& dirfont) const{
    if(parent==0) return;
    if(gGourceSettings.hide_dirnames) return;
    if(gGourceSettings.dir_name_depth > 0 && gGourceSettings.dir_name_depth < (depth-1)) return;

    if(!gGourceSettings.highlight_dirs && since_last_node_change > 5.0) return;

    float alpha = gGourceSettings.highlight_dirs ? 1.0 : std::max(0.0f, 5.0f - since_last_node_change) / 5.0f;

    vec2 label_pos = spline.getLabelPos();

    dirfont.setAlpha(alpha);
    dirfont.draw(label_pos.x, label_pos.y, path_token);
}

void RDirNode::calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection) {

    static GLdouble screen_x, screen_y, screen_z;

    gluProject( pos.x, pos.y, 0.0f, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);
    screen_y = (float)viewport[3] - screen_y;
    projected_pos.x = screen_x;
    projected_pos.y = screen_y;

    gluProject( spos.x, spos.y, 0.0f, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);
    screen_y = (float)viewport[3] - screen_y;
    projected_spos.x = screen_x;
    projected_spos.y = screen_y;

    static vec2 selected_offset(5.5f, -2.0f);
    static vec2 unselected_offset(5.5f, -1.0f);

    if(!gGourceSettings.hide_filenames) {

        //first pass - calculate positions of names
        for(std::list<RFile*>::const_iterator it = files.begin(); it!=files.end(); it++) {
            RFile* f = *it;
            f->calcScreenPos(viewport, modelview, projection);
        }
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->calcScreenPos(viewport, modelview, projection);
    }
}

void RDirNode::drawNames(FXFont& dirfont) {

    if(!gGourceSettings.hide_dirnames && isVisible()) {
        drawDirName(dirfont);
    }

    if(!gGourceSettings.hide_filenames) {

        if(!(gGourceSettings.hide_filenames || gGourceSettings.hide_files) && in_frustum) {
            for(std::list<RFile*>::const_iterator it = files.begin(); it!=files.end(); it++) {
                RFile* f = *it;
                if(!f->isSelected()) f->drawName();
            }
        }

    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->drawNames(dirfont);
    }
}

void RDirNode::checkFrustum(const Frustum& frustum) {

    in_frustum = frustum.intersects(quadItemBounds);

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->checkFrustum(frustum);
    }
}

void RDirNode::drawShadows(float dt) const{

    if(in_frustum) {

        glPushMatrix();
        glTranslatef(pos.x, pos.y, 0.0);

        //draw files
        for(std::list<RFile*>::const_iterator it = files.begin(); it!=files.end(); it++) {
            RFile* f = *it;
            if(f->isHidden()) continue;

            f->drawShadow(dt);
        }

        glPopMatrix();
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->drawShadows(dt);
    }
}

void RDirNode::updateFilesVBO(quadbuf& buffer, float dt) const{

    if(in_frustum) {

        for(std::list<RFile*>::const_iterator it = files.begin(); it!=files.end(); it++) {
            RFile* f = *it;

            if(f->isHidden()) continue;

            vec3 col   = f->getColour();
            float alpha = f->getAlpha();

            buffer.add(f->graphic->textureid, f->getAbsolutePos() - f->dims*0.5f, f->dims, vec4(col.x, col.y, col.z, alpha));
        }
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->updateFilesVBO(buffer,dt);
    }
}

void RDirNode::updateBloomVBO(bloombuf& buffer, float dt) {

    if(in_frustum && isVisible()) {

        float bloom_radius   = dir_radius * 2.0 * gGourceSettings.bloom_multiplier;
        float bloom_diameter = bloom_radius * 2.0;
        vec4 bloom_col      = col * gGourceSettings.bloom_intensity;

        vec4 bloom_texcoords(bloom_radius, pos.x, pos.y, 0.0f);

        vec2 bloom_dims(bloom_diameter, bloom_diameter);

        buffer.add(0, pos - bloom_dims*0.5f,bloom_dims, vec4(bloom_col.x, bloom_col.y, bloom_col.z, 1.0f), bloom_texcoords);
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->updateBloomVBO(buffer,dt);
    }
}

void RDirNode::drawFiles(float dt) const{

    if(in_frustum) {
        glPushMatrix();
            glTranslatef(pos.x, pos.y, 0.0);

            //draw files

            for(std::list<RFile*>::const_iterator it = files.begin(); it!=files.end(); it++) {
                RFile* f = *it;
                if(f->isHidden()) continue;

                f->draw(dt);
            }

        glPopMatrix();
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->drawFiles(dt);
    }
}

const vec2 & RDirNode::getSPos() const{
    return projected_spos;
}

const vec2 & RDirNode::getProjectedPos() const{
    return projected_pos;
}

void RDirNode::updateEdgeVBO(quadbuf& buffer) const {

    if(parent!=0 && (!gGourceSettings.hide_root || parent->parent !=0)) spline.drawToVBO(buffer);

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = (*it);

        if(child->isVisible()) {
            child->updateEdgeVBO(buffer);
        }
    }
}

void RDirNode::drawEdgeShadows() const{

    if(parent!=0 && (!gGourceSettings.hide_root || parent->parent !=0)) spline.drawShadow();

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = (*it);

        //draw edge - assumes calcEdges() called before hand so spline will exist
        if(child->isVisible()) {
           child->drawEdgeShadows();
        }
    }
}

void RDirNode::drawEdges() const{

   if(parent!=0 && (!gGourceSettings.hide_root || parent->parent !=0)) spline.draw();

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* child = (*it);

        //draw edge - assumes calcEdges() called before hand so spline will exist
        if(child->isVisible()) {
           child->drawEdges();
        }
    }
}

void RDirNode::drawBloom(float dt){

    if(in_frustum && isVisible()) {

        float bloom_radius = dir_radius * 2.0 * gGourceSettings.bloom_multiplier;

        vec4 bloom_col = col * gGourceSettings.bloom_intensity;

        glColor4f(bloom_col.x, bloom_col.y, bloom_col.z, 1.0);

        glPushMatrix();
            glTranslatef(pos.x, pos.y, 0.0);

            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2f(bloom_radius,bloom_radius);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(bloom_radius,-bloom_radius);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(-bloom_radius,-bloom_radius);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2f(-bloom_radius,bloom_radius);
            glEnd();
        glPopMatrix();
    }

    for(std::list<RDirNode*>::const_iterator it = children.begin(); it != children.end(); it++) {
        RDirNode* node = (*it);
        node->drawBloom(dt);
    }
}

void RDirNode::updateQuadItemBounds() {
    float radius = getRadius();

    vec2 radoffset(radius, radius);

    //set bounds
    quadItemBounds.set(pos - radoffset, pos + radoffset);
}

