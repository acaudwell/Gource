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

#include "user.h"

float gGourceBeamDist = 100.0;
float gGourceActionDist = 50.0;
float gGourceMaxUserIdle = 3.0;
float gGourcePersonalSpaceDist = 100.0;
float gGourceMaxFileLagSeconds = 5.0;
float gGourceMaxUserSpeed      = 500.0;
float gGourceUserFriction      = 1.0;

bool gGourceColourUserImages = false;

std::map<std::string, std::string> gGourceUserImageMap;
std::string gGourceUserImageDir;
std::string gGourceDefaultUserImage;

//fractions of days per second
float gGourceDaysPerSecond = 0.1;

bool gGourceHideUsers = false;

RUser::RUser(std::string name, vec2f pos, int tagid) : Pawn(name,pos,tagid) {

    this->name = name;

    speed = gGourceMaxUserSpeed;
    size = 20.0;

    shadow = true;

    highlighted=false;

    assignIcon();

    setSelected(false);

    last_action = 0.0;
    action_interval = 0.2;
    name_interval = 5.0;

    min_units_ps = 100.0;
}

void RUser::addAction(RAction* action) {

    if(action->source != this) return;

    actions.push_back(action);
}

// remove references to this file
void RUser::fileRemoved(RFile* f) {

    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end(); ) {
        RAction* a = *it;
        if(a->target == f) {
            it = actions.erase(it);
            delete a;
            continue;
        }

        it++;
    }

    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); ) {
        RAction* a = *it;
        if(a->target == f) {
            it = activeActions.erase(it);
            delete a;
            continue;
        }

        it++;
    }
}

void RUser::applyForceUser(RUser* u) {

    if(u==this) return;

    vec2f u_pos = u->getPos();

    vec2f dir = u_pos - pos;

    float dist = dir.length();

    //different repelling force depending on how busy the user is
    float desired_dist = getActionCount() == 0 ?
        gGourcePersonalSpaceDist : (actions.size()>0 && activeActions.size()==0) ?
            gGourcePersonalSpaceDist * 0.1 : gGourcePersonalSpaceDist * 0.5;

    //resolve overlap
    if(dist < 0.001) {

        accel += 1.0f * vec2f( (rand() % 100) - 50, (rand() % 100) - 50).normal();

        return;
    }

    //repelling force
    if(dist < desired_dist) {
        accel -= (desired_dist-dist) * dir.normal();
    }
}

void RUser::applyForceAction(RAction* action) {
    RFile* f = action->target;

    vec2f f_pos = f->getAbsolutePos();
    vec2f dir = f_pos - pos;
    float dist = dir.length();

    float desired_dist = gGourceActionDist;

    //resolve overlap
    if(dist < 0.001) {
        accel += vec2f( (rand() % 100) - 50, (rand() % 100) - 50).normal();
        return;
    }

    //repelling force
    if(dist < desired_dist) {
        accel -= (desired_dist - dist) * dir.normal();
        return;
    }

    if(dist > gGourceBeamDist) {
        accel += (dist-gGourceBeamDist) * dir.normal();
    }
}

void RUser::applyForceToActions() {
    if(activeActions.size()==0 && actions.size()==0) return;

    last_action = elapsed;

    int action_influence = 0;
    int max_influence    = 3;

    // move towards actions being worked on

    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); it++) {
        RAction* action = *it;

        applyForceAction(action);

        action_influence++;
        if(action_influence >= max_influence) break;
    }

    if(activeActions.size()!=0) return;

    //if no actions being worked on, move towards one pending action
    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end(); it++) {
        RAction* action = *it;

        applyForceAction(action);

        break;
    }


}

void RUser::assignIcon() {
    struct stat fileinfo;

    usercol = colourHash(name);

    bool image_assigned = false;

    if(gGourceUserImageDir.size() > 0) {

        //try thier username
        // TODO: replace with map of name -> image of all pngs and jpgs in directory
        //gGourceUserImageDir + name + std::string(".jpg");

        std::map<std::string, std::string>::iterator findimage;

        findimage = gGourceUserImageMap.find(name);

        //do we have this image
        if(findimage != gGourceUserImageMap.end()) {
            std::string imagefile = findimage->second;

            if(!gGourceColourUserImages) usercol = vec3f(1.0, 1.0, 1.0);

            icon = texturemanager.grab(imagefile, 1, 1, 0, true);

            setHighlighted(true);

            image_assigned = true;
        }
    }

    //nope
    if(!image_assigned) {
        if(gGourceDefaultUserImage.size() > 0) {
            if(!gGourceColourUserImages) usercol = vec3f(1.0, 1.0, 1.0);
            icon = texturemanager.grab(gGourceDefaultUserImage, 1, 1, 0, true);
        } else {
            icon = texturemanager.grab("no_photo.png");
        }
    }

    usercol = usercol * 0.6 + vec3f(1.0, 1.0, 1.0) * 0.4;
    usercol *= 0.9;
}

int RUser::getActionCount() {
    return actions.size() + activeActions.size();
}

int RUser::getPendingActionCount() {
    return actions.size();
}

void RUser::logic(float t, float dt) {
    Pawn::logic(dt);

    action_interval -= dt;

    bool find_nearby_action = false;

    if(actions.size() && action_interval <= 0) {
        find_nearby_action = true;
    }

    //add next active action, if it is in range
    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end();) {
        RAction* action = *it;

        //add all files which are too old
        if(gGourceMaxFileLagSeconds>=0.0 && action->addedtime < t - gGourceMaxFileLagSeconds) {
            it = actions.erase(it);
            action->rate = 2.0;
            activeActions.push_back(action);
            continue;
        }

        if(!find_nearby_action) break;

        float action_dist = (action->target->getAbsolutePos() - pos).length();

        //queue first action in range
        if(action_dist < gGourceBeamDist) {
            it = actions.erase(it);
            activeActions.push_back(action);
            break;
        }

        it++;
    }

    //reset action interval
    if(action_interval <= 0) {
        int total_actions = actions.size() + activeActions.size();

        action_interval = total_actions ? (1.0 / (float)total_actions) : 1.0;
    }

    //update actions
    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); ) {

        RAction* action = *it;

        action->logic(dt);

        if(action->isFinished()) {
            it = activeActions.erase(it);
            delete action;
            continue;
        }

        it++;
    }

    if(accel.length2() > speed * speed) {
        accel = accel.normal() * speed;
    }

    pos += accel * dt;

    accel = accel * std::max(0.0f, (1.0f - gGourceUserFriction*dt));

    //ensure characters dont crawl
//     float accel_amount = accel.length();
//     if(actions.size()>0 && accel_amount > 0.0 && accel_amount < min_units_ps) {
//         accel = accel.normal() * min_units_ps;
//     }

//    move(dt);
}

void RUser::updateFont() {
    if(selected) {
        font = fontmanager.grab("FreeSans.ttf", 18);
        font.dropShadow(true);
    } else {
        font = fontmanager.grab("FreeSans.ttf", 14);
        font.dropShadow(true);
    }

    namewidth = font.getWidth(name);
}

void RUser::setHighlighted(bool highlight) {
    this->highlighted = highlight;

    updateFont();
}

void RUser::setSelected(bool selected) {
    Pawn::setSelected(selected);
    updateFont();
}

vec3f RUser::getNameColour() {
    return (selected||highlighted) ? vec3f(1.0, 1.0, 0.3) : namecol;
}

vec3f RUser::getColour() {
    if(selected) return vec3f(1.0, 1.0, 1.0);

    return usercol;
}

std::string RUser::getName() {
    return name;
}

float RUser::getAlpha() {

    float alpha = Pawn::getAlpha();
    //user fades out if not doing anything
    if(elapsed - last_action > gGourceMaxUserIdle) {
        alpha = 1.0 - std::min(elapsed - last_action - gGourceMaxUserIdle, 1.0f);
    }

    return alpha;
}

bool RUser::isIdle() {
    return (activeActions.size()==0 && actions.size()==0);
}

bool RUser::isFading() {
    return isIdle() && (elapsed - last_action) > gGourceMaxUserIdle;
}

bool RUser::isInactive() {
    return isIdle() && (elapsed - last_action) > 10.0;
}

bool RUser::nameVisible() {
    return (Pawn::nameVisible() || highlighted) ? true : false;
}

void RUser::drawNameText(float alpha) {
    if(gGourceHideUsers) return;

    float user_alpha = getAlpha();

    if(highlighted || selected || alpha>0.0) {
        vec3f nameCol = getNameColour();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);

        vec3f drawpos = vec3f(pos.x, pos.y, 0.0);

        vec3f screenpos = display.project(drawpos);

        screenpos.x -= namewidth * 0.5;
        screenpos.y -= size * 2.0;

        glColor4f(nameCol.x, nameCol.y, nameCol.z, (selected||highlighted) ? user_alpha : alpha);

        glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
                glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

         glMatrixMode(GL_MODELVIEW);
             glPushMatrix();
                glLoadIdentity();

         font.draw(screenpos.x, screenpos.y, name); // above player

         glMatrixMode(GL_PROJECTION);
            glPopMatrix();

         glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

        //font.draw(pos.x - (namewidth/(float)2.0), pos.y - (float)size*1.4, name.c_str()); // above player
    }
}

void RUser::drawActions(float dt) {
    if(gGourceHideUsers) return;

    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); it++) {
        RAction* action = *it;
        action->draw(dt);
    }
}

void RUser::draw(float dt) {
    if(gGourceHideUsers) return;

    Pawn::draw(dt);
}
