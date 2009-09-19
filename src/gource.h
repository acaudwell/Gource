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

#ifndef GOURCE_H
#define GOURCE_H

#define GOURCE_VERSION "0.12"

#ifdef _WIN32
#include "windows.h"
#endif

#include <deque>
#include <dirent.h>

#include "core/display.h"
#include "core/sdlapp.h"
#include "core/fxfont.h"
#include "core/bounds.h"
#include "core/seeklog.h"
#include "core/frustum.h"
#include "core/regex.h"

#include "git.h"
#include "cvs-exp.h"
#include "custom.h"

#include "slider.h"

#include "action.h"
#include "file.h"
#include "user.h"
#include "dirnode.h"
#include "zoomcamera.h"

void gource_help(std::string error);

class Gource : public SDLApp {
    std::string logfile;

    std::vector<std::string> follow_users;
    std::vector<std::string> highlight_users;
    std::vector<Regex*> filters;

    RCommitLog* commitlog;
    PositionSlider slider;
    ZoomCamera camera;

    vec3f background_colour;

    bool debug, trace_debug;

    bool mousemoved;
    bool mouseclicked;
    vec2f mousepos;

    float start_position;

    int tag_seq, commit_seq;

    GLint mouse_hits;

    RFile* hoverFile;
    RFile* selectedFile;

    RUser* hoverUser;
    RUser* selectedUser;

    GLuint selectionDepth;

    RDirNode* root;

    std::string displaydate;
    int date_x_offset;

    FXFont font, fontlarge, fontmedium;

    bool first_read;
    bool draw_loading;
    bool paused;

    long starttime;
    long currtime;

    float splash;

    float idle_time;
    float elapsed_time;

    Uint32 draw_tree_time;
    Uint32 update_dir_tree_time;
    Uint32 update_user_tree_time;
    Uint32 draw_time;
    Uint32 logic_time;
    Uint32 trace_time;

    bool track_users;

    Bounds2D dir_bounds;
    Bounds2D user_bounds;

    std::deque<RCommit> commitqueue;
    std::map<std::string, RUser*> users;
    std::map<std::string, RFile*> files;
    std::map<int, RFile*> tagfilemap;
    std::map<int, RUser*> tagusermap;

    QuadTree* dirNodeTree;
    QuadTree* userTree;

    void reset();
    void findUserImages();

    void deleteUser(RUser* user);
    void deleteFile(RFile* file);

    void selectUser(RUser* user);
    void selectFile(RFile* file);
    void selectNextUser();

    void readLog();
    void processCommit(RCommit& commit);

    std::string dateAtPosition(float percent);

    void toggleCameraMode();

    RCommitLog* determineFormat(std::string logfile);

    void interactUsers();
    void interactDirs();
    void updateUsers(float dt);
    void updateDirs(float dt);

    void updateTime();

    void mousetrace(Frustum& frustum, float dt);

    bool canSeek();
    void seekTo(float percent);

    void zoom(bool zoomin);

    void loadingScreen();
public:
    Gource(std::string logfile);
    ~Gource();

    void setBackground(vec3f background);
    void setCameraMode(bool track_users);
    void setStartPosition(float percent);
    void showSplash();

    void addFollowUser(std::string user);
    void addHighlightUser(std::string user);
    void addFilter(Regex* filter);

    void logic(float t, float dt);
    void draw(float t, float dt);

    void init();
    void update(float t, float dt);
    void keyPress(SDL_KeyboardEvent *e);
    void mouseMove(SDL_MouseMotionEvent *e);
    void mouseClick(SDL_MouseButtonEvent *e);
};

extern float gGourceAutoSkipSeconds;
extern bool  gGourceHideUsernames;
extern bool  gGourceHideDate;
extern bool  gGourceDisableProgress;
extern bool  gGourceFileLoop;
extern int   gGourceMaxFiles;

extern bool  gGourceHighlightAllUsers;

#endif
