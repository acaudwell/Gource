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

#ifdef _WIN32
#include "windows.h"
#endif

#include <deque>
#include <fstream>

#include "core/display.h"
#include "core/shader.h"
#include "core/sdlapp.h"
#include "core/fxfont.h"
#include "core/bounds.h"
#include "core/seeklog.h"
#include "core/frustum.h"
#include "core/regex.h"
#include "core/ppm.h"
#include "core/mousecursor.h"

#include "gource_settings.h"

#include "git.h"
#include "hg.h"
#include "bzr.h"
#include "gitraw.h"
#include "cvs2cl.h"
#include "cvs-exp.h"
#include "custom.h"
#include "apache.h"
#include "svn.h"

#include "core/vbo.h"
#include "bloom.h"
#include "slider.h"
#include "textbox.h"
#include "action.h"
#include "file.h"
#include "user.h"
#include "dirnode.h"
#include "zoomcamera.h"
#include "key.h"

class Gource : public SDLApp {
    std::string logfile;

    FrameExporter* frameExporter;

    RCommitLog* commitlog;
    PositionSlider slider;
    ZoomCamera camera;

    FileKey file_key;

    bool debug, trace_debug;

    bool manual_zoom;
    bool manual_rotate;
    bool manual_camera;

    float rotation_remaining_angle;

    MouseCursor cursor;

    bool grab_mouse;
    bool mousemoved;
    bool mouseclicked;
    bool mousedragged;

    vec2f cursor_move;

    bool recolour;

    bool update_file_labels;

    bool use_selection_bounds;
    Bounds2D selection_bounds;

    float rotate_angle;

    vec2f mousepos;

    float last_percent;

    bool stop_position_reached;

    int tag_seq, commit_seq;

    GLint mouse_hits;

    RFile* hoverFile;
    RFile* selectedFile;

    RUser* hoverUser;
    RUser* selectedUser;

    quadbuf  file_vbo;
    quadbuf  user_vbo;
    quadbuf  edge_vbo;
    quadbuf  action_vbo;

    bloombuf bloom_vbo;

    GLuint selectionDepth;

    RDirNode* root;

    std::string displaydate;
    int date_x_offset;

    TextureResource* bloomtex;
    TextureResource* beamtex;
    TextureResource* logotex;
    TextureResource* backgroundtex;
    TextureResource* usertex;

    Shader*          shadow_shader;
    Shader*          text_shader;
    Shader*          bloom_shader;

    float font_texel_size;

    TextBox textbox;

    FXFont font, fontlarge, fontmedium;

    bool first_read;
    bool draw_loading;
    bool paused;

    float max_tick_rate;
    int frameskip;
    int framecount;

    time_t currtime;
    time_t lasttime;
    float runtime;
    float subseconds;

    float splash;

    float idle_time;

    Uint32 screen_project_time;
    Uint32 draw_edges_time;
    Uint32 draw_shadows_time;
    Uint32 draw_actions_time;
    Uint32 draw_files_time;
    Uint32 draw_users_time;
    Uint32 draw_bloom_time;
    Uint32 update_vbos_time;
    Uint32 update_dir_tree_time;
    Uint32 update_user_tree_time;
    Uint32 draw_scene_time;
    Uint32 logic_time;
    Uint32 trace_time;
    Uint32 text_time;
    Uint32 text_update_time;
    Uint32 text_vbo_commit_time;
    Uint32 text_vbo_draw_time;

    bool track_users;

    Bounds2D dir_bounds;
    Bounds2D user_bounds;
    Bounds2D active_user_bounds;

    std::deque<RCommit> commitqueue;
    std::map<std::string, RUser*> users;
    std::map<std::string, RFile*> files;
    std::map<int, RFile*> tagfilemap;
    std::map<int, RUser*> tagusermap;

    QuadTree* dirNodeTree;
    QuadTree* userTree;

    std::string message;
    float message_timer;

    void setMessage(const char* str, ...);

    void reset();

    RUser* addUser(const std::string& username);
    RFile* addFile(const RCommitFile& cf);

    void deleteUser(RUser* user);
    void deleteFile(RFile* file);

    void selectBackground();
    void selectUser(RUser* user);
    void selectFile(RFile* file);
    void selectNextUser();

    void readLog();

    void processCommit(RCommit& commit, float t);
    void addFileAction(const std::string& username, const RCommitFile& cf, RFile* file, float t);

    std::string dateAtPosition(float percent);

    void toggleCameraMode();

    static RCommitLog* determineFormat(const std::string& logfile);

    void updateCamera(float dt);

    void updateUsers(float t, float dt);
    void updateDirs(float dt);

    void interactUsers();
    void interactDirs();

    void updateBounds();

    void updateTime(time_t display_time);

    void mousetrace(float dt);

    bool canSeek();
    void seekTo(float percent);

    void zoom(bool zoomin);

    void loadingScreen();
    void drawBackground(float dt);

    void drawScene(float dt);
    
    void updateVBOs(float dt);

    void updateAndDrawEdges();
    
    void drawFileShadows(float dt);
    void drawUserShadows(float dt);
    void drawActions(float dt);
    void drawFiles(float dt);
    void drawUsers(float dt);
    void drawBloom(float dt);

    void screenshot();

    void changeColours();
public:
    Gource(FrameExporter* frameExporter = 0);
    ~Gource();

    static void writeCustomLog(const std::string& logfile, const std::string& output_file);

    void setCameraMode(const std::string& mode);
    void setCameraMode(bool track_users);
    void setFrameExporter(FrameExporter* exporter, int video_framerate);

    void showSplash();

    void logic(float t, float dt);
    void draw(float t, float dt);

    void init();
    void update(float t, float dt);
    void keyPress(SDL_KeyboardEvent *e);
    void mouseMove(SDL_MouseMotionEvent *e);
    void mouseClick(SDL_MouseButtonEvent *e);
};

#endif
