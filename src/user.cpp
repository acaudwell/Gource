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

float gGourceBeamDist          = 100.0;
float gGourceActionDist        = 50.0;
float gGourcePersonalSpaceDist = 100.0;

RUser::RUser(const std::string& name, vec2 pos, int tagid) : Pawn(name,pos,tagid) {

    this->name = name;

    speed = gGourceSettings.max_user_speed;
    size = 20.0 * gGourceSettings.user_scale;

    shadowOffset = vec2(2.0, 2.0) * gGourceSettings.user_scale;

    shadow = true;

    highlighted=false;

    assignUserImage();

    setSelected(false);

    last_action = 0.0;
    action_interval = 0.2;
    name_interval = 5.0;

    min_units_ps = 100.0;

    actionCount = activeCount = 0;
}

void RUser::addAction(RAction* action) {

    if(action->source != this) return;

    if(isIdle()) showName();
    //name_interval = name_interval > 0.0 ? std::max(name_interval,nametime-1.0f) : nametime;

    actions.push_back(action);
    actionCount++;
}

// remove references to this file
void RUser::fileRemoved(RFile* f) {

    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end(); ) {
        RAction* a = *it;
        if(a->target == f) {
            it = actions.erase(it);
            delete a;
            actionCount--;
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

    vec2 u_pos = u->getPos();

    vec2 dir = u_pos - pos;

    float dist = glm::length(dir);

    //different repelling force depending on how busy the user is
    float desired_dist = getActionCount() == 0 ?
        gGourcePersonalSpaceDist : (!actions.empty() && activeActions.empty()) ?
            gGourcePersonalSpaceDist * 0.1 : gGourcePersonalSpaceDist * 0.5;

    //resolve overlap
    if(dist < 0.001) {

        accel += 1.0f * normalise(vec2( (rand() % 100) - 50, (rand() % 100) - 50));

        return;
    }

    //repelling force
    if(dist < desired_dist) {
        accel -= (desired_dist-dist) * normalise(dir);
    }
}

void RUser::applyForceAction(RAction* action) {
    RFile* f = action->target;

    vec2 f_pos = f->getAbsolutePos();
    vec2 dir = f_pos - pos;
    float dist = glm::length(dir);

    float desired_dist = gGourceActionDist;

    //resolve overlap
    if(dist < 0.001) {
        accel += normalise(vec2( (rand() % 100) - 50, (rand() % 100) - 50));
        return;
    }

    //repelling force
    if(dist < desired_dist) {
        accel -= (desired_dist - dist) * normalise(dir);
        return;
    }

    if(dist > gGourceBeamDist) {
        accel += (dist-gGourceBeamDist) * normalise(dir);
    }
}

void RUser::applyForceToActions() {
    if(activeActions.empty() && actions.empty()) return;

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

    if(!activeActions.empty()) return;

    //if no actions being worked on, move towards one pending action
    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end(); it++) {
        RAction* action = *it;

        applyForceAction(action);

        break;
    }


}

void RUser::colourize() {
    usercol = colourHash(name);
}

void RUser::assignUserImage() {
    colourize();

    TextureResource* graphic = 0;

    if(gGourceSettings.user_image_dir.size() > 0) {

        //try thier username
        // TODO: replace with map of name -> image of all pngs and jpgs in directory
        //gGourceSettings.user_image_dir + name + std::string(".jpg");

        std::map<std::string, std::string>::iterator findimage;

        findimage = gGourceSettings.user_image_map.find(name);

        //do we have this image
        if(findimage != gGourceSettings.user_image_map.end()) {
            std::string imagefile = findimage->second;

            if(!gGourceSettings.colour_user_images) usercol = vec3(1.0, 1.0, 1.0);

            graphic = texturemanager.grabFile(imagefile, true, GL_CLAMP_TO_EDGE);
        }
    }


    //TODO: trilinear probably should be an attribute of the texture
    //      perhaps the mipmap option should be an enum: eg TEX_MIPMAP_TRILINEAR

    //nope
    if(!graphic) {
        if(gGourceSettings.default_user_image.size() > 0) {
            if(!gGourceSettings.colour_user_images) usercol = vec3(1.0, 1.0, 1.0);
            graphic = texturemanager.grabFile(gGourceSettings.default_user_image, true, GL_CLAMP_TO_EDGE);
        } else {
            graphic = texturemanager.grab("user.png", true, GL_CLAMP_TO_EDGE);
        }
    }

    setGraphic(graphic);

    usercol = usercol * 0.6f + vec3(1.0f) * 0.4f;
    usercol *= 0.9f;
}

int RUser::getActionCount() {
    return actionCount + activeCount;
}

int RUser::getPendingActionCount() {
    return actionCount;
}

void RUser::logic(float t, float dt) {
    Pawn::logic(dt);

    action_interval -= dt;

    bool find_nearby_action = false;

    if(!actions.empty() && action_interval <= 0) {
        find_nearby_action = true;
    }

    //add next active action, if it is in range
    for(std::list<RAction*>::iterator it = actions.begin(); it != actions.end();) {
        RAction* action = *it;

        //add all files which are too old
        if(gGourceSettings.max_file_lag>=0.0 && action->t < t - gGourceSettings.max_file_lag) {
            it = actions.erase(it);
            actionCount--;
            action->rate = 2.0;
            activeActions.push_back(action);
            activeCount++;
            continue;
        }

        if(!find_nearby_action) break;

        float action_dist = glm::length(action->target->getAbsolutePos() - pos);

        //queue first action in range
        if(action_dist < gGourceBeamDist) {
            it = actions.erase(it);
            activeActions.push_back(action);
            actionCount--; activeCount++;
            break;
        }

        it++;
    }

    //reset action interval
    if(action_interval <= 0) {
        int total_actions = actionCount + activeCount;

        action_interval = total_actions ? (1.0 / (float)total_actions) : 1.0;
    }

    //update actions
    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); ) {

        RAction* action = *it;

        action->logic(dt);

        if(action->isFinished()) {
            it = activeActions.erase(it);
            delete action;
            activeCount--;
            continue;
        }

        it++;
    }

    if(glm::length2(accel) > speed * speed) {
        accel = normalise(accel) * speed;
    }

    pos += accel * dt;

    accel = accel * std::max(0.0f, (1.0f - gGourceSettings.user_friction*dt));
}

void RUser::updateFont() {
    if(selected) {
        font = fontmanager.grab(gGourceSettings.font_file, 18);
        font.dropShadow(true);
    } else {
        font = fontmanager.grab(gGourceSettings.font_file, gGourceSettings.user_font_size);
        font.dropShadow(true);
    }

    font.alignTop(false);

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

const vec3& RUser::getNameColour() const {
    return selected ? gGourceSettings.selection_colour : highlighted ? gGourceSettings.highlight_colour : namecol;
}

vec3 RUser::getColour() const{
    if(selected) return vec3(1.0, 1.0, 1.0);

    return usercol;
}

const std::string& RUser::getName() const {
    return name;
}

float RUser::getAlpha() const {
    float alpha = Pawn::getAlpha();
    //user fades out if not doing anything
    if(elapsed - last_action > gGourceSettings.user_idle_time) {
        alpha = 1.0 - std::min(elapsed - last_action - gGourceSettings.user_idle_time, 1.0f);
    }

    return alpha;
}

bool RUser::isIdle() {
    return (activeActions.empty() && actions.empty());
}

bool RUser::isFading() {
    return isIdle() && (elapsed - last_action) > gGourceSettings.user_idle_time;
}

bool RUser::isInactive() {
    return isIdle() && (elapsed - last_action) > 10.0;
}

bool RUser::nameVisible() const {
    return (Pawn::nameVisible() || gGourceSettings.highlight_all_users || highlighted) ? true : false;
}

void RUser::calcScreenPos(GLint* viewport, GLdouble* modelview, GLdouble* projection) {

    static GLdouble screen_x, screen_y, screen_z;

    vec2 text_pos = pos;
    text_pos.y -= dims.y * 0.5f;

    gluProject( text_pos.x, text_pos.y, 0.0f, modelview, projection, viewport, &screen_x, &screen_y, &screen_z);

    screen_y = (float)viewport[3] - screen_y;

    screenpos.x = screen_x - namewidth * 0.5;
    screenpos.y = screen_y - font.getMaxHeight();
}

void RUser::drawNameText(float alpha) {
    float user_alpha = getAlpha();

    if(gGourceSettings.highlight_all_users || highlighted || selected || alpha>0.0) {
        vec3 name_col  = getNameColour();
        float name_alpha = (selected||highlighted||gGourceSettings.highlight_all_users) ? user_alpha : alpha;

        font.setColour(vec4(name_col.x, name_col.y, name_col.z, name_alpha));
        font.draw(screenpos.x, screenpos.y, name);
    }
}

void RUser::updateActionsVBO(quadbuf& buffer) {

    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); it++) {
        RAction* action = *it;
        action->drawToVBO(buffer);
    }
}

void RUser::drawActions(float dt) {

    for(std::list<RAction*>::iterator it = activeActions.begin(); it != activeActions.end(); it++) {
        RAction* action = *it;
        action->draw(dt);
    }
}

void RUser::draw(float dt) {
    if(gGourceSettings.hide_users) return;

    Pawn::draw(dt);
}
