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

#include "quadtree.h"

// QUAD NODE

// allow more items in node if
// - has no children and one of any of:
//    * maximum depth reached
//    * item count is less than desired capacity


bool QuadNode::allowMoreItems() {
    return (children.size() == 0 && (depth >= tree->max_node_depth || items.size() < tree->max_node_items ) );
}


void QuadNode::addItem(QuadItem* item) {

    if(allowMoreItems()) {
        tree->item_count++;

        items.push_back(item);
        return;
    }

    if(children.size() != 0) {
        addToChild(item);
        return;
    }

    vec2f average = bounds.centre();

    vec2f middle = average - bounds.min;

    vec2f relmax = bounds.max-bounds.min;

    Bounds2D newbounds;

    children.reserve(4);

    //top left
    newbounds = Bounds2D( bounds.min + vec2f(0.0, 0.0), bounds.min + middle );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //top right
    newbounds = Bounds2D( bounds.min + vec2f(middle.x, 0.0), bounds.min + vec2f(relmax.x,middle.y) );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //bottom left
    newbounds = Bounds2D( bounds.min + vec2f(0.0, middle.y), bounds.min + vec2f(middle.x,relmax.y) );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //bottom right
    newbounds = Bounds2D( bounds.min + middle, bounds.max );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
        QuadItem* oi = *it;
        tree->item_count--;

        addToChild(oi);
    }

    items.clear();
}


void QuadNode::addToChild(QuadItem* item) {
    if(children.size()==0) return;

    for(int i=0;i<4;i++) {
        if(children[i]->bounds.overlaps(item->quadItemBounds)) {
            children[i]->addItem(item);
        }
    }
}


void QuadNode::getLeavesInFrustum(std::vector<QuadNode*>& nodevec, Frustum& frustum) {

    if(items.size()>0) {
        nodevec.push_back(this);
        return;
    }

    if(children.size()==0) return;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && frustum.boundsInFrustum(children[i]->bounds)) {
            children[i]->getLeavesInFrustum(nodevec, frustum);
        }
    }

    return;
}


int QuadNode::getItemsInFrustum(std::vector<QuadItem*>& itemvec, Frustum& frustum) {

    if(items.size()>0) {
        int items_added = 0;
        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);

            if(oi!=0) {
                itemvec.push_back(oi);
                items_added++;
            }
        }
        return items_added;
    }

    if(children.size()==0) return 0;

    int count = 0;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && frustum.boundsInFrustum(children[i]->bounds)) {
            count += children[i]->getItemsInFrustum(itemvec, frustum);
        }
    }

    return count;
}


int QuadNode::getItemsInBounds(std::vector<QuadItem*>& itemvec, Bounds2D& bounds) {

    if(items.size()>0) {
        int items_added = 0;

        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            itemvec.push_back(oi);
            items_added++;
        }

        return items_added;
    }

    if(children.size()==0) return 0;

    int count = 0;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && bounds.overlaps(children[i]->bounds)) {
            count += children[i]->getItemsInBounds(itemvec, bounds);
        }
    }

    return count;
}




int QuadNode::getItemsAt(std::vector<QuadItem*>& itemvec, vec2f pos) {

    if(items.size()>0) {
        int items_added = 0;
        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            if(oi!=0) {
                itemvec.push_back(oi);
                items_added++;
            }
        }
        return items_added;
    }

    if(children.size()==0) return 0;

    int index = getChildIndex(pos);

    if(index == -1) return 0;

    return children[index]->getItemsAt(itemvec, pos);
}


bool QuadNode::empty() {
    return (items.size() == 0 && children.size()==0);
}


int QuadNode::getChildIndex(vec2f pos) {

    if(children.size()==0) return -1;

    for(int i=0;i<4;i++) {
        if(children[i]->bounds.contains(pos)) {
            return i;
        }
    }

    //debugLog("(%.3f, %.3f) is outside of bounds of node (%.3f, %.3f) to (%.3f, %.3f) \n", pos.x, pos.y, bounds.min.x,  bounds.min.y, bounds.max.x,  bounds.max.y);

    return -1;
}



QuadNode::QuadNode(QuadTree* tree, QuadNode* parent, Bounds2D bounds, int parent_depth) {
    //debugLog("new QuadNode from (%.3f, %.3f) to (%.3f, %.3f) \n",  bounds.min.x,  bounds.min.y, bounds.max.x,  bounds.max.y);

    this->parent = parent;
    this->tree   = tree;
    this->bounds = bounds;
    this->depth  = parent_depth + 1;

    listid = 0;

    tree->node_count++;
}


QuadNode::~QuadNode() {

    if(listid) glDeleteLists(listid, 1);

    if(children.size()>0) {
        for(int i=0;i<4;i++) {
            delete children[i];
        }
    }

    tree->item_count -= items.size();

    items.clear();

    tree->node_count--;
}


int QuadNode::usedChildren() {
    int populated = 0;

    if(children.size()>0) {
        for(int i=0;i<4;i++) {
            if(!children[i]->empty()) populated++;
        }
    }

    return populated;
}


int QuadNode::draw(Frustum& frustum) {

    if(listid && items.size()) {
        glPushMatrix();
            glCallList(listid);
        glPopMatrix();
        return 1;
    }

    int drawn = 0;

    if(children.size() > 0) {
        for(int i=0;i<4;i++) {
            QuadNode* c = children[i];
            if(!c->empty() && frustum.boundsInFrustum(c->bounds)) {
                drawn += c->draw(frustum);
            }
        }
    }

    return drawn;
}


void QuadNode::generateLists() {

    if(items.size() > 0) {
        if(!listid) listid = glGenLists(1);

        glNewList(listid, GL_COMPILE);

        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            oi->drawQuadItem();
        }

        glEndList();
        return;
    }

    if(children.size() > 0) {
        for(int i=0;i<4;i++) {
            QuadNode* c = children[i];
            if(!c->empty()) {
                c->generateLists();
            }
        }
    }
}


void QuadNode::outline() {
    bounds.draw();

    if(children.size()==0) return;

    for(int i=0;i<4;i++) {
        QuadNode* c = children[i];
        if(c!=0) {
            c->outline();
        }
    }
}

//Quad TREE


QuadTree::QuadTree(Bounds2D bounds, int max_node_depth, int max_node_items) {
    item_count        = 0;
    node_count        = 0;
    unique_item_count = 0;

    this->max_node_depth = max_node_depth;
    this->max_node_items = max_node_items;

    root = new QuadNode(this, 0, bounds, 0);
}


QuadTree::~QuadTree() {
    delete root;
}


int QuadTree::getItemsAt(std::vector<QuadItem*>& itemvec, vec2f pos) {
    return root->getItemsAt(itemvec, pos);
}


int QuadTree::getItemsInFrustum(std::vector<QuadItem*>& itemvec, Frustum& frustum) {
    return root->getItemsInFrustum(itemvec, frustum);
}


int QuadTree::getItemsInBounds(std::vector<QuadItem*>& itemvec, Bounds2D& bounds) {
    return root->getItemsInBounds(itemvec, bounds);
}


void QuadTree::getLeavesInFrustum(std::vector<QuadNode*>& nodevec, Frustum& frustum) {
    return root->getLeavesInFrustum(nodevec, frustum);
}


void QuadTree::addItem(QuadItem* item) {
    root->addItem(item);
    unique_item_count++;
}


int QuadTree::drawNodesInFrustum(Frustum& frustum) {
    return root->draw(frustum);
}


void QuadTree::generateLists() {
    root->generateLists();
}


void QuadTree::outline() {
    root->outline();
}