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

#ifndef RUSER_H
#define RUSER_H

#include "gource_settings.h"

#include "pawn.h"
#include "action.h"
#include "file.h"

#include "sys/stat.h"

#include <list>

class RAction;
class RFile;

class RUser : public Pawn {
    void assignIcon();

    std::list<RAction*> actions;
    std::list<RAction*> activeActions;
    size_t actionCount;
    size_t activeCount;

    float action_interval;
    float action_dist;

    float last_action;

    float min_units_ps;

    std::string name;
    vec3f usercol;

    bool highlighted;

    bool nameVisible() const;

    void updateFont();
    const vec3f& getNameColour() const;
    void drawNameText(float alpha) const;
public:
    RUser(std::string name, vec2f pos, int tagid);

    vec3f getColour() const;

    std::string getName();
    std::string getUsername();

    void fileRemoved(RFile* f);
    void addAction(RAction* action);

    bool isIdle();
    bool isFading();
    bool isInactive();

    void setSelected(bool selected);
    void setHighlighted(bool highlighted);

    int getActionCount();
    int getPendingActionCount();

    float getAlpha() const;

    void applyForceToActions();
    void applyForceAction(RAction* action);
    void applyForceUser(RUser* u);
    void logic(float t, float dt);

    void drawActions(float dt);
    void draw(float dt);
};


class UserForceFunctor : public VisitFunctor<QuadItem>{

  private:
    RUser * this_user;
    std::set<RUser*> seen;
    int loopCount;

  public:
    UserForceFunctor(RUser * user) : this_user(user), seen(), loopCount(0){}
    int getLoopCount() const{ return loopCount; }
    void operator()(QuadItem * user){

       std::set<RUser*>::iterator seentest;

       RUser * b = (RUser*) (user);

       if(b==this_user) return;

       if(b->node_count != 1) {
           if((seentest = seen.find(b)) != seen.end())
               return;

           seen.insert(b);
       }

       this_user->applyForceUser(b);
       loopCount++;

    }

};

#endif
