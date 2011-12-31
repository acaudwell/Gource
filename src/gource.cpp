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

#include "gource.h"

bool  gGourceDrawBackground  = true;
bool  gGourceQuadTreeDebug   = false;
int   gGourceMaxQuadTreeDepth = 6;

int gGourceUserInnerLoops = 0;

Gource::Gource(FrameExporter* exporter) {

    this->logfile = gGourceSettings.path;
    commitlog = 0;

    //disable OpenGL 2.0 functions if not supported
    if(!GLEW_VERSION_2_0) gGourceSettings.ffp = true;

    if(!gGourceSettings.file_graphic) {
        gGourceSettings.file_graphic = texturemanager.grab("file.png", true, true, true);
    }

    fontlarge = fontmanager.grab("FreeSans.ttf", 42);
    fontlarge.dropShadow(true);
    fontlarge.roundCoordinates(true);

    fontmedium = fontmanager.grab("FreeSans.ttf", gGourceSettings.font_size);
    fontmedium.dropShadow(true);
    fontmedium.roundCoordinates(false);

    font = fontmanager.grab("FreeSans.ttf", 14);
    font.dropShadow(true);
    font.roundCoordinates(true);

    //only use bloom with alpha channel if transparent due to artifacts on some video cards
    std::string bloom_tga = gGourceSettings.transparent ? "bloom_alpha.tga" : "bloom.tga";

    bloomtex = texturemanager.grab(bloom_tga);
    beamtex  = texturemanager.grab("beam.png");
    usertex  = texturemanager.grab("user.png", true, true, true);

    shadow_shader = text_shader = bloom_shader = 0;

    if(!gGourceSettings.ffp) {
        shadow_shader      = shadermanager.grab("shadow");
        bloom_shader       = shadermanager.grab("bloom");
        text_shader        = shadermanager.grab("text");
    }

    //calculate once
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    font_texel_size = 1.0f / (float) std::min( 512, max_texture_size );

    logotex = 0;
    backgroundtex = 0;

    //load logo
    if(gGourceSettings.logo.size() > 0) {
        logotex = texturemanager.grabFile(gGourceSettings.logo);
    }

    //load background image
    if(gGourceSettings.background_image.size() > 0) {
        backgroundtex = texturemanager.grabFile(gGourceSettings.background_image);
    }

    stop_position_reached=false;

    paused       = false;
    first_read   = true;
    draw_loading = true;

    grab_mouse   = false;
    mousemoved   = false;
    mousedragged = false;
    mouseclicked = false;

    cursor.setCursorTexture(texturemanager.grab("cursor.png"));
    cursor.useSystemCursor(false);

    if(gGourceSettings.hide_mouse) {
        cursor.showCursor(false);
    }

    splash = -1.0;

    debug = false;
    trace_debug = false;

    frameExporter = 0;

    dirNodeTree = 0;
    userTree = 0;

    selectedFile = 0;
    hoverFile = 0;
    selectedUser = 0;
    hoverUser = 0;

    date_x_offset = 0;

    textbox = TextBox(fontmanager.grab("FreeSans.ttf", 18));
    textbox.setBrightness(0.5f);
    textbox.show();

    file_key = FileKey(1.0f);

    camera = ZoomCamera(vec3f(0,0, -300), vec3f(0.0, 0.0, 0.0), gGourceSettings.camera_zoom_default, gGourceSettings.camera_zoom_max);
    camera.setPadding(gGourceSettings.padding);

    setCameraMode(gGourceSettings.camera_mode);

    root = 0;

    //min phsyics rate 60fps (ie maximum allowed delta 1.0/60)
    max_tick_rate = 1.0 / 60.0;
    runtime = 0.0f;
    frameskip = 0;
    framecount = 0;

    reset();

    if(exporter!=0) setFrameExporter(exporter, gGourceSettings.output_framerate);

    //if recording a video or in demo mode, or multiple repos, the slider is initially hidden
    if(exporter==0 && gGourceSettings.repo_count==1) slider.show();
}

void Gource::writeCustomLog(const std::string& logfile, const std::string& output_file) {

    RCommitLog* commitlog = determineFormat(logfile);

    if(!commitlog) return;

    RCommit commit;

    FILE* fh = stdout;

    if(output_file != "-") {
        fh = fopen(output_file.c_str(), "w");

        if(!fh) return;
    }

    while(!commitlog->isFinished()) {

        RCommit commit;

        if(!commitlog->nextCommit(commit)) {
             if(!commitlog->isSeekable()) {
                 break;
             }
            continue;
        }

        for(std::list<RCommitFile>::iterator it = commit.files.begin(); it != commit.files.end(); it++) {
            RCommitFile& cf = *it;
            fprintf(fh, "%lld|%s|%s|%s\n", (long long int) commit.timestamp, commit.username.c_str(), cf.action.c_str(), cf.filename.c_str());
        }

        commit.files.clear();
    }

    if(output_file != "-") fclose(fh);
}

bool Gource::findRepository(boost::filesystem::path& dir, std::string& log_format) {

    dir = absolute(dir);

    //fprintf(stderr, "find repository from initial path: %s\n", dir.string().c_str());

    while(is_directory(dir)) {

             if(is_directory(dir / ".git")) log_format = "git";
        else if(is_directory(dir / ".hg"))  log_format = "hg";
        else if(is_directory(dir / ".bzr")) log_format = "bzr";
        else if(is_directory(dir / ".svn")) log_format = "svn";

        if(!log_format.empty()) {
            //fprintf(stderr, "found '%s' repository at: %s\n", log_format.c_str(), dir.string().c_str());
            return true;
        }

        if(!dir.has_parent_path()) return false;

        dir = dir.parent_path();
    }

    return false;
}

RCommitLog* Gource::determineFormat(std::string logfile) {
    debugLog("determineFormat(%s)\n", logfile.c_str());

    RCommitLog* clog = 0;

    std::string log_format = gGourceSettings.log_format;

    //if the log format is not specified and 'logfile' is a directory, recursively look for a version control repository.
    //this method allows for something strange like someone who having an svn repository inside a git repository
    //(in which case it would pick the svn directory as it would encounter that first)

    if(log_format.empty() && logfile != "-") {

        try {
            boost::filesystem::path repo_path(logfile);

            if(is_directory(repo_path)) {
                if(findRepository(repo_path, log_format)) {
                    logfile = repo_path.string();
                }
            }
        } catch(boost::filesystem3::filesystem_error& error) {
        }
    }

    //we've been told what format to use
    if(log_format.size() > 0) {
        debugLog("log-format = %s\n", log_format.c_str());

        if(log_format == "git") {
            clog = new GitCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;

            clog = new GitRawCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "hg") {
            clog = new MercurialLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }
        if(log_format == "bzr") {
            clog = new BazaarLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "cvs") {
            clog = new CVSEXPCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "custom") {
            clog = new CustomLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "apache") {
            clog = new ApacheCombinedLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "svn") {
            clog = new SVNCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "cvs2cl") {
            clog = new CVS2CLCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        return 0;
    }

    // try different formats until one works

    //git
    debugLog("trying git...\n");
    clog = new GitCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //mercurial
    debugLog("trying mercurial...\n");
    clog = new MercurialLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //bzr
    debugLog("trying bzr...\n");
    clog = new BazaarLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //git raw
    debugLog("trying git raw...\n");
    clog = new GitRawCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //cvs exp
    debugLog("trying cvs-exp...\n");
    clog = new CVSEXPCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //svn
    debugLog("trying svn...\n");
    clog = new SVNCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //cvs2cl
    debugLog("trying cvs2cl...\n");
    clog = new CVS2CLCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //custom
    debugLog("trying custom...\n");
    clog = new CustomLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //apache
    debugLog("trying apache combined...\n");
    clog = new ApacheCombinedLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    return 0;
}

Gource::~Gource() {
    reset();

    if(commitlog!=0) delete commitlog;
    if(root!=0) delete root;

    //reset settings
    gGourceSettings.setGourceDefaults();
}

void Gource::init() {
}

void Gource::update(float t, float dt) {

    float scaled_dt = std::min(dt, max_tick_rate);

    //if exporting a video use a fixed tick rate rather than time based
    if(frameExporter != 0) {
        scaled_dt = max_tick_rate;
    }

    //apply time scaling
    scaled_dt *= gGourceSettings.time_scale;

    //have to manage runtime internally as we're messing with dt
    if(!paused) runtime += scaled_dt;

    if(gGourceSettings.stop_at_time > 0.0 && runtime >= gGourceSettings.stop_at_time) stop_position_reached = true;

    logic_time = SDL_GetTicks();

    logic(runtime, scaled_dt);

    logic_time = SDL_GetTicks() - logic_time;

    draw(runtime, scaled_dt);

    //extract frames based on frameskip setting if frameExporter defined
    if(frameExporter != 0) {
        if(framecount % (frameskip+1) == 0) {
            frameExporter->dump();
        }
    }

    if(!gGourceSettings.hide_mouse) {
        //note: cursor uses real dt
        cursor.logic(dt);
        cursor.draw();
    }

    framecount++;
}

//peek at the date under the mouse pointer on the slider
std::string Gource::dateAtPosition(float percent) {

    RCommit commit;
    std::string date;

    if(percent<1.0 && commitlog->getCommitAt(percent, commit)) {
        //display date
        char datestr[256];

        // TODO: memory leak ??
        struct tm* timeinfo = localtime ( &(commit.timestamp) );
        strftime(datestr, 256, "%A, %d %B, %Y", timeinfo);

        date = std::string(datestr);
    }

    return date;
}

void Gource::mouseMove(SDL_MouseMotionEvent *e) {
    if(commitlog==0) return;
    if(gGourceSettings.hide_mouse) return;

    bool rightmouse = cursor.rightButtonPressed();

    if(grab_mouse) {
         int warp_x = display.width/2;
         int warp_y = display.height/2;

         //this is an even we generated by warping the mouse below
         if(e->x == warp_x && e->y == warp_y) return;

         SDL_WarpMouse(warp_x, warp_y);
    }

    //move camera in direction the user dragged the mouse
    if(mousedragged || rightmouse) {
        vec2f mag( e->xrel, e->yrel );

        //if right mouse button is held while dragging, rotate tree instead of
        //moving camera

        if(rightmouse) {
            manual_rotate = true;
            if(fabs(mag.x) > fabs(mag.y)) {
                rotate_angle = std::min(1.0f, (float) fabs(mag.x) / 10.0f) * 5.0f * DEGREES_TO_RADIANS;
                if(mag.x < 0.0f) rotate_angle = -rotate_angle;
            } else {
                rotate_angle = std::min(1.0f, (float) fabs(mag.y) / 10.0f) * 5.0f * DEGREES_TO_RADIANS;
                if(mag.y < 0.0f) rotate_angle = -rotate_angle;
            }

            return;
        }

        cursor_move = mag;

        return;
    }

    if(grab_mouse) return;

    mousepos = vec2f(e->x, e->y);
    mousemoved=true;

    cursor.updatePos(mousepos);

    float pos;

    if(!gGourceSettings.hide_progress && slider.mouseOver(mousepos, &pos)) {
        std::string date = dateAtPosition(pos);
        slider.setCaption(date);
    }

}

void Gource::zoom(bool zoomin) {

    manual_zoom = true;

    float zoom_multi = 1.1;

    float distance = -camera.getDest().z;

    if(zoomin) {
        distance /= zoom_multi;

        if(distance < gGourceSettings.camera_zoom_min) distance = gGourceSettings.camera_zoom_min;
    } else {
        distance *= zoom_multi;

        if(distance > gGourceSettings.camera_zoom_max) distance = gGourceSettings.camera_zoom_max;
    }

    camera.setDistance(distance);
}

void Gource::mouseClick(SDL_MouseButtonEvent *e) {
    if(commitlog==0) return;
    if(gGourceSettings.hide_mouse) return;

    //mouse click should stop the cursor being idle
    cursor.updatePos(mousepos);

    if(e->type == SDL_MOUSEBUTTONUP) {

        if(e->button == SDL_BUTTON_LEFT) {
            //stop dragging mouse, return the mouse to where
            //the user started dragging.
            mousedragged=false;
        }

        if(e->button == SDL_BUTTON_LEFT || e->button == SDL_BUTTON_RIGHT) {
            if(!cursor.buttonPressed()) {
                cursor.showCursor(true);
                grab_mouse=false;
                SDL_WarpMouse(mousepos.x, mousepos.y);
            }
        }
    }

    if(e->type != SDL_MOUSEBUTTONDOWN) return;

    //wheel up
    if(e->button == SDL_BUTTON_WHEELUP) {
        zoom(true);
        return;
    }

    //wheel down
    if(e->button == SDL_BUTTON_WHEELDOWN) {
        zoom(false);
        return;
    }

    if(e->button == SDL_BUTTON_MIDDLE) {
        toggleCameraMode();
        return;
    }


    if(e->button == SDL_BUTTON_RIGHT) {
        cursor.showCursor(false);
        grab_mouse=true;
        return;
    }

    if(e->button == SDL_BUTTON_LEFT) {

        //mousepos = vec2f(e->x, e->y);
        mouseclicked=true;

        if(canSeek()) {
            float position;
            if(slider.click(mousepos, &position)) {
                seekTo(position);
            }
        }
    }
}

void Gource::showSplash() {
    splash = 15.0;
}

void Gource::setFrameExporter(FrameExporter* exporter, int video_framerate) {

    int gource_framerate = video_framerate;


    this->framecount = 0;
    this->frameskip  = 0;

    //calculate appropriate tick rate for video frame rate
    while(gource_framerate<60) {
        gource_framerate += video_framerate;
        this->frameskip++;
    }

    this->max_tick_rate = 1.0f / ((float) gource_framerate);

    this->frameExporter = exporter;
}

void Gource::setCameraMode(const std::string& mode) {
    setCameraMode(mode == "track");
}

void Gource::setCameraMode(bool track_users) {
    manual_rotate   = false;
    manual_zoom     = false;

    this->track_users = track_users;
    if(selectedUser!=0) camera.lockOn(track_users);

    manual_camera = false;

    gGourceSettings.camera_mode = track_users ? "track" : "overview";
}

void Gource::toggleCameraMode() {
    setCameraMode(!track_users);
}

//trace click of mouse on background
void Gource::selectBackground() {
    //is the left mouse button down?

    if(!cursor.leftButtonPressed()) return;

    selectUser(0);

    manual_camera = true;

    cursor.showCursor(false);
    grab_mouse=true;
    mousedragged=true;
}

//select a user, deselect current file/user
void Gource::selectUser(RUser* user) {
    //already selected do nothing
    if(user!=0 && selectedUser==user) return;

    if(selectedFile != 0) {
        selectedFile->setSelected(false);
        selectedFile = 0;
    }

    // deselect current user
    if(selectedUser != 0) {
        selectedUser->setSelected(false);
        selectedUser = 0;
    }

    //if no user return
    if(user == 0) {
        camera.lockOn(false);
        return;
    }

    selectedUser = user;

    //select user, lock on camera
    selectedUser->setSelected(true);

    if(track_users) camera.lockOn(true);
}

//select a file, deselect current file/user
void Gource::selectFile(RFile* file) {

    //already selected do nothing
    if(file!=0 && selectedFile==file) return;

    if(selectedUser != 0) {
        selectedUser->setSelected(false);
        selectedUser = 0;
    }

    // deselect current file
    if(selectedFile != 0) {
        selectedFile->setSelected(false);
        selectedFile = 0;
    }

    //if no file return
    if(file == 0) {
        return;
    }

    selectedFile = file;

    //select user, lock on camera
    selectedFile->setSelected(true);
}


void Gource::selectNextUser() {
    //debugLog("selectNextUser()\n");

    int currTagId = -1;

    if(selectedUser != 0) {
        currTagId = selectedUser->getTagID();
    }

    RUser* newSelectedUser = 0;

    // find next user after this user
    for(std::map<int,RUser*>::iterator it = tagusermap.begin(); it != tagusermap.end(); it++) {
        RUser* user = it->second;

        if(!user->isInactive() && user->getTagID() > currTagId && user->getAlpha() >= 1.0) {
            newSelectedUser = user;
            break;
        }
    }

    // just get first user
    if(newSelectedUser == 0) {

        for(std::map<int,RUser*>::iterator it = tagusermap.begin(); it != tagusermap.end(); it++) {
            RUser* user = it->second;

            if(!user->isInactive() && user->getAlpha() >= 1.0) {
                newSelectedUser = user;
                break;
            }
        }
    }

    selectUser(newSelectedUser);
}

void Gource::keyPress(SDL_KeyboardEvent *e) {
    if (e->type == SDL_KEYUP) return;

    if (e->type == SDL_KEYDOWN) {
        if (e->keysym.unicode == SDLK_ESCAPE) {
            appFinished=true;
        }

        if(commitlog==0) return;

        if(e->keysym.sym == SDLK_F12) {
            screenshot();
        }

        if (e->keysym.sym == SDLK_q) {
            debug = !debug;
        }

        if (e->keysym.sym == SDLK_w) {
            trace_debug = !trace_debug;
        }

        if (e->keysym.sym == SDLK_m) {

            //toggle mouse visiblity unless mouse clicked/pressed/dragged
            if(!(mousedragged || mouseclicked || cursor.leftButtonPressed() )) {

                if(!cursor.isHidden()) {
                    cursor.showCursor(false);
                    gGourceSettings.hide_mouse = true;
                } else {
                    cursor.showCursor(true);
                    gGourceSettings.hide_mouse = false;
                }
            }
        }

        if (e->keysym.sym == SDLK_n) {
            idle_time = gGourceSettings.auto_skip_seconds;
        }

        if (e->keysym.sym == SDLK_t) {
            gGourceQuadTreeDebug = !gGourceQuadTreeDebug;
        }

        if (e->keysym.sym == SDLK_y) {
            gGourceSettings.hide_tree = !gGourceSettings.hide_tree;
        }

        if (e->keysym.sym == SDLK_g) {
            gGourceSettings.hide_users = !gGourceSettings.hide_users;
        }

        if (e->keysym.sym == SDLK_u) {

            if(gGourceSettings.hide_usernames && !gGourceSettings.highlight_all_users) {
                gGourceSettings.hide_usernames      = false;
                gGourceSettings.highlight_all_users = true;

            } else if (gGourceSettings.highlight_all_users && !gGourceSettings.hide_usernames) {
                gGourceSettings.hide_usernames      = false;
                gGourceSettings.highlight_all_users = false;
            } else {
                gGourceSettings.hide_usernames      = true;
                gGourceSettings.highlight_all_users = false;
            }

        }

        if (e->keysym.sym == SDLK_d) {
            if(gGourceSettings.hide_dirnames && !gGourceSettings.highlight_dirs) {

                gGourceSettings.hide_dirnames  = false;
                gGourceSettings.highlight_dirs = true;

            } else if(gGourceSettings.highlight_dirs && !gGourceSettings.hide_dirnames) {

                gGourceSettings.hide_dirnames  = false;
                gGourceSettings.highlight_dirs = false;

            } else {
                gGourceSettings.hide_dirnames  = true;
                gGourceSettings.highlight_dirs = false;
            }
        }

        if (e->keysym.sym == SDLK_f) {

            if(gGourceSettings.hide_filenames && !gGourceSettings.file_extensions) {
                gGourceSettings.hide_filenames  = false;
            } else if(!gGourceSettings.hide_filenames && gGourceSettings.file_extensions) {
                gGourceSettings.file_extensions = false;
                gGourceSettings.hide_filenames = true;
            } else {
                gGourceSettings.file_extensions = true;
                gGourceSettings.hide_filenames  = false;
            }

            update_file_labels = true;
        }

        if (e->keysym.sym == SDLK_r) {
            gGourceSettings.hide_root = !gGourceSettings.hide_root;
        }

        if (e->keysym.sym == SDLK_k) {
            gGourceSettings.show_key = !gGourceSettings.show_key;
        }

        if(e->keysym.sym == SDLK_c) {
            splash = 15.0f;
        }

        if (e->keysym.sym == SDLK_v) {
            toggleCameraMode();
        }

        if (e->keysym.sym == SDLK_p) {
            if(GLEW_VERSION_2_0 && bloom_shader != 0) {
                gGourceSettings.ffp = !gGourceSettings.ffp;
            }
        }

        if (e->keysym.sym == SDLK_z) {
            gGourceGravity = !gGourceGravity;
        }

        if (e->keysym.sym == SDLK_s) {
            recolour=true;
        }

        if(e->keysym.unicode == SDLK_TAB) {
            selectNextUser();
        }

        if (e->keysym.unicode == SDLK_SPACE) {
            paused = !paused;
        }

        if (e->keysym.unicode == SDLK_EQUALS || e->keysym.unicode == SDLK_PLUS) {
            if(gGourceSettings.days_per_second>=1.0) {
                gGourceSettings.days_per_second = std::min(30.0f, floorf(gGourceSettings.days_per_second) + 1.0f);
            } else {
                gGourceSettings.days_per_second = std::min(1.0f, gGourceSettings.days_per_second * 2.0f);
            }
        }

        if (e->keysym.unicode == SDLK_MINUS) {
            if(gGourceSettings.days_per_second>1.0) {
                gGourceSettings.days_per_second = std::max(0.0f, floorf(gGourceSettings.days_per_second) - 1.0f);
            } else {
                gGourceSettings.days_per_second = std::max(0.0f, gGourceSettings.days_per_second * 0.5f);
            }
        }

        if(e->keysym.sym == SDLK_KP_MINUS) {
            zoom(true);
        }

        if(e->keysym.sym == SDLK_KP_PLUS) {
            zoom(false);
        }

        if(e->keysym.unicode == SDLK_LEFTBRACKET) {
            gGourceForceGravity /= 1.1;
        }

        if(e->keysym.unicode == SDLK_RIGHTBRACKET) {
            gGourceForceGravity *= 1.1;
        }

        if(e->keysym.unicode == SDLK_PERIOD) {

            if(gGourceSettings.time_scale>=1.0) {
                gGourceSettings.time_scale = std::min(4.0f, floorf(gGourceSettings.time_scale) + 1.0f);
            } else {
                gGourceSettings.time_scale = std::min(1.0f, gGourceSettings.time_scale * 2.0f);
            }
        }

        if(e->keysym.unicode == SDLK_COMMA) {

            if(gGourceSettings.time_scale>1.0) {
                gGourceSettings.time_scale = std::max(0.0f, floorf(gGourceSettings.time_scale) - 1.0f);
            } else {
                gGourceSettings.time_scale = std::max(0.25f, gGourceSettings.time_scale * 0.5f);
            }
        }

        if(e->keysym.unicode == SDLK_SLASH) {
            gGourceSettings.time_scale = 1.0f;
        }
    }
}

void Gource::reset() {
    camera.reset();
    user_bounds.reset();
    active_user_bounds.reset();
    dir_bounds.reset();
    commitqueue.clear();
    tagfilemap.clear();
    tagusermap.clear();
    gGourceRemovedFiles.clear();

    if(userTree!=0) delete userTree;
    if(dirNodeTree!=0) delete dirNodeTree;

    recolour = false;
    update_file_labels = false;

    userTree = 0;
    dirNodeTree = 0;

    selectedFile = 0;
    hoverFile = 0;

    use_selection_bounds = false;
    selection_bounds.reset();

    manual_rotate   = false;
    manual_zoom     = false;
    rotation_remaining_angle = 0.0f;

    message_timer = 0.0f;

    cursor_move = vec2f(0.0f, 0.0f);

    selectedUser = 0;
    hoverUser = 0;

    manual_camera = false;

    grab_mouse = false;

    mouseclicked=false;
    mousemoved=false;
    mousedragged = false;

    rotate_angle = 0.0f;

    if(root!=0) delete root;
    root = new RDirNode(0, "/");

    //delete users
    for(std::map<std::string,RUser*>::iterator it = users.begin(); it != users.end(); it++) {
        delete it->second;
    }

    users.clear();

    //delete
    for(std::map<std::string,RFile*>::iterator it = files.begin(); it != files.end(); it++) {
        delete it->second;
    }

    last_percent = 0.0;

    files.clear();

    file_key.clear();

    idle_time=0;
    currtime=0;
    lasttime=0;
    subseconds=0.0;
    tag_seq = 1;
    commit_seq = 1;
}

void Gource::deleteFile(RFile* file) {
    //debugLog("removing file %s\n", file->fullpath.c_str());

    root->removeFile(file);

    if(hoverFile == file) {
        hoverFile = 0;
    }

    if(selectedFile == file) {
        selectFile(0);
    }

    //remove from any users with actions against this file - wrong way around? meh
    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        RUser* user = it->second;

        user->fileRemoved(file);
    }

    files.erase(file->fullpath);
    tagfilemap.erase(file->getTagID());
    file_key.dec(file);

    //debugLog("removed file %s\n", file->fullpath.c_str());

    delete file;
}


RFile* Gource::addFile(const RCommitFile& cf) {

    //if we already have max files in circulation
    //we cant add any more
    if(gGourceSettings.max_files > 0 && files.size() >= gGourceSettings.max_files) return 0;

    //see if this is a directory
    std::string file_as_dir = cf.filename;
    if(file_as_dir[file_as_dir.size()-1] != '/') file_as_dir.append("/");

    if(root->isDir(file_as_dir)) return 0;

    int tagid = tag_seq++;

    RFile* file = new RFile(cf.filename, cf.colour, vec2f(0.0,0.0), tagid);

    files[cf.filename] = file;
    tagfilemap[tagid]  = file;

    root->addFile(file);

    file_key.inc(file);

    while(root->getParent() != 0) {
        debugLog("parent changed to %s\n", root->getPath().c_str());
        root = root->getParent();
    }

    return file;
}

RUser* Gource::addUser(const std::string& username) {

    vec2f pos;

    if(dir_bounds.area() > 0) {
        pos = dir_bounds.centre();
    } else {
        pos = vec2f(0,0);
    }

    int tagid = tag_seq++;

    RUser* user = new RUser(username, pos, tagid);

    users[username]   = user;
    tagusermap[tagid] = user;

    //debugLog("added user %s, tagid = %d\n", username.c_str(), tagid);

    return user;
}

void Gource::deleteUser(RUser* user) {

    if(hoverUser == user) {
        hoverUser = 0;
    }

    if(selectedUser == user) {
        selectUser(0);
    }

    users.erase(user->getName());
    tagusermap.erase(user->getTagID());

    //debugLog("deleted user %s, tagid = %d\n", user->getName().c_str(), user->getTagID());

    delete user;
}

bool Gource::canSeek() {
    if(gGourceSettings.hide_progress || commitlog == 0 || !commitlog->isSeekable()) return false;

    return true;
}

void Gource::seekTo(float percent) {
    //debugLog("seekTo(%.2f)\n", percent);

    if(commitlog == 0 || !commitlog->isSeekable()) return;

    // end pause
    if(paused) paused = false;

    reset();

    commitlog->seekTo(percent);
}

void Gource::readLog() {
    if(stop_position_reached) return;

    //debugLog("readLog()\n");

    while(!commitlog->isFinished() && commitqueue.size() < 1) {

        RCommit commit;

        if(!commitlog->nextCommit(commit)) {
            if(!commitlog->isSeekable()) {
                break;
            }
            continue;
        }

        //ignore blank commits
        if(commit.files.size() > 0) {
            commitqueue.push_back(commit);
        }
    }

    if(first_read && commitqueue.size()==0) {
        throw SDLAppException("no commits found");
    }

    first_read = false;

    if(!commitlog->isFinished() && commitlog->isSeekable()) {
        last_percent = commitlog->getPercent();
        slider.setPercent(last_percent);
    }

    bool is_finished = commitlog->isFinished();

    if(   gGourceSettings.stop_at_end && is_finished
       || gGourceSettings.stop_position > 0.0 && commitlog->isSeekable() && (is_finished || last_percent >= gGourceSettings.stop_position)) {
        stop_position_reached = true;
    }

    // useful to figure out where we have crashes
    //debugLog("current date: %s\n", displaydate.c_str());
}

void Gource::processCommit(RCommit& commit, float t) {

    //check user against filters, if found, discard commit
    if(!gGourceSettings.user_filters.empty()) {
        for(std::vector<Regex*>::iterator ri = gGourceSettings.user_filters.begin(); ri != gGourceSettings.user_filters.end(); ri++) {
            Regex* r = *ri;

            if(r->match(commit.username)) {
                return;
            }
        }
    }

    //find files of this commit or create it
    for(std::list<RCommitFile>::iterator it = commit.files.begin(); it != commit.files.end(); it++) {

        RCommitFile& cf = *it;
        RFile* file = 0;

        //check filename against filters
        if(!gGourceSettings.file_filters.empty()) {

            bool filtered_filename = false;

            for(std::vector<Regex*>::iterator ri = gGourceSettings.file_filters.begin(); ri != gGourceSettings.file_filters.end(); ri++) {
                Regex* r = *ri;

                if(r->match(cf.filename)) {
                    filtered_filename = true;
                    break;
                }
            }

            if(filtered_filename) continue;
        }

        //is this a directory (ends in slash)
        //deleting a directory - find directory: then for each file, remove each file

        if(!cf.filename.empty() && cf.filename[cf.filename.size()-1] == '/') {

            //ignore unless it is a delete: we cannot 'add' or 'modify' a directory
            //as its not a physical entity in Gource, only files are.

            if(cf.action != "D") continue;

            std::list<RDirNode*> dirs;

            root->findDirs(cf.filename, dirs);

            for(std::list<RDirNode*>::iterator it = dirs.begin(); it != dirs.end(); it++) {

                RDirNode* dir = (*it);

                //fprintf(stderr, "deleting everything under %s because of %s\n", dir->getPath().c_str(), cf.filename.c_str());

                //foreach dir files
                std::list<RFile*> dir_files;

                dir->getFilesRecursive(dir_files);

                for(std::list<RFile*>::iterator it = dir_files.begin(); it != dir_files.end(); it++) {
                    RFile* file = *it;

                    addFileAction(commit.username, cf.action, file, t);
                }
            }

            return;
        }

        std::map<std::string, RFile*>::iterator seen_file = files.find(cf.filename);
        if(seen_file != files.end()) file = seen_file->second;

        if(file == 0) {

            file = addFile(cf);

            if(!file) continue;
        }

        addFileAction(commit.username, cf.action, file, t);
    }
}

void Gource::addFileAction(const std::string& username, const std::string& action, RFile* file, float t) {
    //create user if havent yet. do it here to ensure at least one of there files
    //was added (incase we hit gGourceSettings.max_files)

    //find user of this commit or create them
    RUser* user = 0;

    //see if user already exists
    std::map<std::string, RUser*>::iterator seen_user = users.find(username);
    if(seen_user != users.end()) user = seen_user->second;

    if(user == 0) {
        user = addUser(username);

        // set the highlighted flag if name matches a highlighted user
        for(std::vector<std::string>::iterator hi = gGourceSettings.highlight_users.begin(); hi != gGourceSettings.highlight_users.end(); hi++) {
            std::string highlight = *hi;

            if(highlight.size() && user->getName() == highlight) {
                user->setHighlighted(true);
                break;
            }
        }
    }

    //create action

    RAction* userAction = 0;

    int commitNo = commit_seq++;

    if(action == "D") {
        userAction = new RemoveAction(user, file, t);
    } else {
        if(action == "A") {
            userAction = new CreateAction(user, file, t);
        } else {
            userAction = new ModifyAction(user, file, t);
        }
    }

    user->addAction(userAction);
}

void Gource::interactUsers() {


    // update quad tree
    Bounds2D quadtreebounds = user_bounds;

    quadtreebounds.min -= vec2f(1.0f, 1.0f);
    quadtreebounds.max += vec2f(1.0f, 1.0f);

    update_user_tree_time = SDL_GetTicks();

    if(userTree != 0) delete userTree;

    int max_depth = 1;

    //dont use deep quad tree initially when all the nodes are in one place
    if(dir_bounds.area() > 10000.0) {
        max_depth = gGourceMaxQuadTreeDepth;
    }

    userTree = new QuadTree(quadtreebounds, max_depth, 1);

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        RUser* user = it->second;

        userTree->addItem(user);
    }

    //move users - interact with other users and files
    for(std::map<std::string,RUser*>::iterator ait = users.begin(); ait!=users.end(); ait++) {

        RUser* a = ait->second;

        UserForceFunctor uff(a);
        userTree->visitItemsInBounds(a->quadItemBounds, uff);
        gGourceUserInnerLoops += uff.getLoopCount();

        a->applyForceToActions();
    }

    update_user_tree_time = SDL_GetTicks() - update_user_tree_time;
}

void Gource::updateBounds() {

    user_bounds.reset();
    active_user_bounds.reset();

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        RUser* user = it->second;

        user->updateQuadItemBounds();
        user_bounds.update(user->quadItemBounds);

        if(!user->isIdle()) {
            active_user_bounds.update(user->quadItemBounds);
        }

    }

    dir_bounds.reset();

    for(std::map<std::string,RDirNode*>::iterator it = gGourceDirMap.begin(); it!=gGourceDirMap.end(); it++) {
        RDirNode* node = it->second;

        if(node->isVisible()) {
            node->updateQuadItemBounds();
            dir_bounds.update(node->quadItemBounds);
        }
    }
}


void Gource::updateUsers(float t, float dt) {
    std::vector<RUser*> inactiveUsers;

    size_t idle_users = 0;

    // move users
    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        RUser* u = it->second;

        u->logic(t, dt);

        //deselect user if fading out from inactivity
        if(u->isFading() && selectedUser == u) {
            selectUser(0);
        }

        if(u->isInactive()) {
            inactiveUsers.push_back(u);
        }

        if(u->isIdle()) {
            idle_users++;
        } else {
            //if nothing is selected, and this user is active and this user is the specified user to follow, select them
            if(selectedUser == 0 && selectedFile == 0) {
                for(std::vector<std::string>::iterator ui = gGourceSettings.follow_users.begin(); ui != gGourceSettings.follow_users.end(); ui++) {
                    std::string follow = *ui;

                    if(follow.size() && u->getName() == follow) {
                        selectUser(u);
                    }
                }
            }
        }
    }

    if(users.empty() && stop_position_reached) {
        appFinished = true;
    }

    //nothing is moving so increment idle
    if(idle_users==users.size()) {
        idle_time += dt;
    } else {
        idle_time = 0;
    }

    // delete inactive users
    for(std::vector<RUser*>::iterator it = inactiveUsers.begin(); it != inactiveUsers.end(); it++) {
        deleteUser(*it);
    }
}

void Gource::interactDirs() {

    // update quad tree
    Bounds2D quadtreebounds = dir_bounds;

    quadtreebounds.min -= vec2f(1.0f, 1.0f);
    quadtreebounds.max += vec2f(1.0f, 1.0f);

    update_dir_tree_time = SDL_GetTicks();

    if(dirNodeTree !=0) delete dirNodeTree;

    int max_depth = 1;

    //dont use deep quad tree initially when all the nodes are in one place
    if(dir_bounds.area() > 10000.0) {
        max_depth = gGourceMaxQuadTreeDepth;
    }

    dirNodeTree = new QuadTree(quadtreebounds, max_depth, 1);

    //apply forces with other directories
    for(std::map<std::string,RDirNode*>::iterator it = gGourceDirMap.begin(); it!=gGourceDirMap.end(); it++) {
        RDirNode* node = it->second;

        if(!node->empty()) {
            dirNodeTree->addItem(node);
        }
    }

    update_dir_tree_time = SDL_GetTicks() - update_dir_tree_time;
}

void Gource::updateDirs(float dt) {
    root->applyForces(*dirNodeTree);
    root->logic(dt);
}

void Gource::updateTime(time_t display_time) {

    if(display_time == 0) {
        displaydate = "";
        return;
    }

    //display date
    char datestr[256];
    char timestr[256];
    struct tm* timeinfo = localtime ( &display_time );

    strftime(datestr, 256, gGourceSettings.date_format.c_str(), timeinfo);
    displaydate = datestr;

    //avoid wobbling by only moving font if change is sufficient
    int date_offset = (int) fontmedium.getWidth(displaydate) * 0.5;
    if(abs(date_x_offset - date_offset) > 5) date_x_offset = date_offset;
}

void Gource::updateCamera(float dt) {

    //camera tracking

    bool auto_rotate = !manual_rotate && !gGourceSettings.disable_auto_rotate;


    if(manual_camera) {

        if(cursor_move.length2() > 0.0f) {

            float cam_rate = ( -camera.getPos().z ) / ( 5000.0f );

            vec3f cam_pos = camera.getPos();

            vec2f cursor_delta = cursor_move * cam_rate * 400.0f * dt;

            cam_pos.x += cursor_delta.x;
            cam_pos.y += cursor_delta.y;

            camera.setPos(cam_pos, true);
            camera.stop();

            auto_rotate = false;

            cursor_move = vec2f(0.0f, 0.0f);
        }

    } else {

        Bounds2D cambounds;

        if(track_users && (selectedFile !=0 || selectedUser !=0)) {
            Bounds2D focusbounds;

            vec3f camerapos = camera.getPos();

            if(selectedUser !=0) focusbounds.update(selectedUser->getPos());
            if(selectedFile !=0) focusbounds.update(selectedFile->getAbsolutePos());

            cambounds = focusbounds;
        } else {
            if(track_users && idle_time==0) cambounds = active_user_bounds;
            else cambounds = dir_bounds;
        }

        camera.adjust(cambounds, !manual_zoom);
    }

    camera.logic(dt);

    //automatically rotate camera
    if(auto_rotate) {

        if(rotation_remaining_angle > 0.0f) {

            //rotation through 90 degrees, speed peaks at half way
            float angle_rate = std::max(dt, (float) (1.0f - fabs((rotation_remaining_angle / 90.0f) - 0.5) * 2.0f)) * dt;

            rotate_angle = std::min(rotation_remaining_angle, 90.0f * angle_rate);
            rotation_remaining_angle -= rotate_angle;

            rotate_angle *= DEGREES_TO_RADIANS;

        } else if(!cursor.rightButtonPressed() && dir_bounds.area() > 10000.0f) {

            float ratio = dir_bounds.width() / dir_bounds.height();

            if(ratio < 0.67f) {
                rotation_remaining_angle = 90.0f;
            }
        }
    } else {
        rotation_remaining_angle = 0.0f;
    }
}

//change the string hashing seed and recolour files and users
void Gource::changeColours() {

    gStringHashSeed = (rand() % 10000) + 1;

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it != users.end(); it++) {
        it->second->colourize();
    }

    for(std::map<std::string,RFile*>::iterator it = files.begin(); it != files.end(); it++) {
        it->second->colourize();
    }

    file_key.colourize();
}

void Gource::logic(float t, float dt) {

    if(draw_loading) return;

    if(message_timer>0.0f) message_timer -= dt;
    if(splash>0.0f)        splash -= dt;

    //init log file
    if(commitlog == 0) {

        try {

            commitlog = determineFormat(logfile);

        } catch(SeekLogException& exception) {
            throw SDLAppException("unable to read log file");
        }

        if(commitlog == 0) {
            //if not in a git dir and no log file, show help
            if(logfile.size() == 0 || logfile == ".") {
                SDL_Quit();

                SDLAppException exception("");
                exception.setShowHelp(true);
                throw exception;
            } else if(SDLAppDirExists(logfile)) {
                throw SDLAppException("directory not supported");
            } else {
                throw SDLAppException("unsupported log format (you may need to regenerate your log file)");
            }
        }

        if(gGourceSettings.start_position>0.0) {
            seekTo(gGourceSettings.start_position);
        }
    }

    file_key.logic(dt);

    slider.logic(dt);

    Uint8 *keystate = SDL_GetKeyState(0);

    if(keystate[SDLK_RIGHT]) {
        cursor_move.x = 10.0;
        manual_camera = true;
    }

    if(keystate[SDLK_LEFT]) {
        cursor_move.x = -10.0;
        manual_camera = true;
    }

    if(keystate[SDLK_UP]) {
        cursor_move.y = -10.0;
        manual_camera = true;
    }

    if(keystate[SDLK_DOWN]) {
        cursor_move.y = 10.0;
        manual_camera = true;
    }

    //apply rotation
    if(rotate_angle != 0.0f) {

        float s = sinf(rotate_angle);
        float c = cosf(rotate_angle);

        root->rotate(s, c);

        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            RUser* user = it->second;

            vec2f userpos = user->getPos();

            user->setPos(userpos.rotate(s, c));
        }

        rotate_angle = 0.0f;
    }

    if(recolour) {
        changeColours();
        recolour = false;
    }

    if(update_file_labels) {
        for(std::map<std::string,RFile*>::iterator it = files.begin(); it != files.end(); it++) {
            it->second->updateLabel();
        }
        update_file_labels = false;
    }

    //still want to update camera while paused
    if(paused) {
        updateBounds();
        interactUsers();
        interactDirs();
        updateCamera(dt);
        return;
    }

    // get more entries
    if(commitqueue.size() == 0) {
        readLog();
    }

    //loop in attempt to find commits
    if(commitqueue.size()==0 && commitlog->isSeekable() && gGourceSettings.loop) {
        first_read=true;
        seekTo(0.0);
        readLog();
    }

    if(currtime==0 && commitqueue.size()) {
        currtime   = lasttime = commitqueue[0].timestamp;
        subseconds = 0.0;
    }

    //set current time
    float time_inc = (dt * 86400.0 * gGourceSettings.days_per_second);
    int seconds    = (int) time_inc;

    subseconds += time_inc - ((float) seconds);

    if(subseconds >= 1.0) {
        currtime   += (int) subseconds;
        subseconds -= (int) subseconds;
    }

    currtime += seconds;

    // delete files
    for(std::vector<RFile*>::iterator it = gGourceRemovedFiles.begin(); it != gGourceRemovedFiles.end(); it++) {
        deleteFile(*it);
    }

    gGourceRemovedFiles.clear();


    //add commits up until the current time
    while(commitqueue.size() > 0) {

        RCommit commit = commitqueue[0];

        //auto skip ahead, unless stop_position_reached
        if(gGourceSettings.auto_skip_seconds>=0.0 && idle_time >= gGourceSettings.auto_skip_seconds && !stop_position_reached) {
            currtime = lasttime = commit.timestamp;
            idle_time = 0.0;
        }

        if(commit.timestamp > currtime) break;

        processCommit(commit, t);

        currtime = lasttime = commit.timestamp;
        subseconds = 0.0;

        commitqueue.pop_front();
    }

    //reset loop counters
    gGourceUserInnerLoops = 0;
    gGourceDirNodeInnerLoops = 0;
    gGourceFileInnerLoops = 0;

    updateBounds();

    interactUsers();
    updateUsers(t, dt);

    interactDirs();
    updateDirs(dt);

    updateCamera(dt);

    updateTime(commitqueue.size() > 0 ? currtime : lasttime);
}

void Gource::mousetrace(float dt) {

    vec3f cam_pos = camera.getPos();

    vec2f projected_mouse = vec2f( -(mousepos.x * 2.0f - ((float)display.width)) / ((float)display.height),
                                   (1.0f - (2.0f * mousepos.y) / ((float)display.height)))
                                   * cam_pos.z;
    projected_mouse.x += cam_pos.x;
    projected_mouse.y += cam_pos.y;

    //find user/file under mouse

    RFile* fileSelection = 0;
    RUser* userSelection = 0;

    if(!gGourceSettings.hide_users) {

        std::set<QuadItem*> userset;

        userTree->getItemsAt(userset, projected_mouse);

        for(std::set<QuadItem*>::iterator it = userset.begin(); it != userset.end(); it++) {
            RUser* user = (RUser*) *it;
            if(!user->isFading() && user->quadItemBounds.contains(projected_mouse)) {
                userSelection = user;
                break;
            }
        }

    }

    if(!userSelection && !gGourceSettings.hide_files) {

        std::set<QuadItem*> dirset;

        dirNodeTree->getItemsAt(dirset, projected_mouse);

        for(std::set<QuadItem*>::iterator it = dirset.begin(); it != dirset.end(); it++) {

            RDirNode* dir = (RDirNode*) *it;

            const std::list<RFile*>* files = dir->getFiles();

            for(std::list<RFile*>::const_iterator fi = files->begin(); fi != files->end(); fi++) {

                RFile* file = *fi;

                if(!file->isHidden() && file->overlaps(projected_mouse)) {
                    fileSelection = file;
                    break;
                }
            }
        }
    }

    // is over a file
    if(fileSelection != 0) {
        // un hover a user
        if(hoverUser != 0) {
            hoverUser->setMouseOver(false);
            hoverUser = 0;
        }

        if(fileSelection != hoverFile) {
            //deselect previous selection
            if(hoverFile !=0) hoverFile->setMouseOver(false);

            //select new
            fileSelection->setMouseOver(true);
            hoverFile = fileSelection;
        }

    // is over a user
    } else if(userSelection != 0) {
        // un hover a file
        if(hoverFile != 0) {
            hoverFile->setMouseOver(false);
            hoverFile = 0;
        }

        if(userSelection != hoverUser) {
            //deselect previous selection
            if(hoverUser !=0) hoverUser->setMouseOver(false);

            //select new
            userSelection->setMouseOver(true);
            hoverUser = userSelection;
        }
    } else {
        if(hoverFile!=0) hoverFile->setMouseOver(false);
        if(hoverUser!=0) hoverUser->setMouseOver(false);
        hoverFile=0;
        hoverUser=0;
    }

    if(mouseclicked) {

        if(hoverUser!=0) {
            camera.lockOn(false);
            selectUser(hoverUser);
        } else if(hoverFile!=0) {
            camera.lockOn(false);
            selectFile(hoverFile);
        } else {
            selectBackground();
        }
    }

    //fprintf(stderr, "end trace\n");
}

void Gource::loadingScreen() {
    if(!gGourceDrawBackground) return;
    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    std::string loading_message("Reading Log...");
    int width = font.getWidth(loading_message);

    font.print(display.width/2 - width/2, display.height/2 - 10, "%s", loading_message.c_str());
}

void Gource::drawBackground(float dt) {
    if(!gGourceDrawBackground) return;

    display.setClearColour(vec4f(gGourceSettings.background_colour, gGourceSettings.transparent ? 0.0f : 1.0f));
    display.clear();

    if(backgroundtex!=0) {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);

        glColor4f(1.0, 1.0, 1.0, 1.0);

        glBindTexture(GL_TEXTURE_2D, backgroundtex->textureid);

        glPushMatrix();

            glTranslatef(display.width/2 - backgroundtex->w/2, display.height/2 - backgroundtex->h/2, 0.0f);

            glBegin(GL_QUADS);
                glTexCoord2f(0.0f,0.0f);
                glVertex2i(0, 0);

                glTexCoord2f(1.0f,0.0f);
                glVertex2i(backgroundtex->w, 0);

                glTexCoord2f(1.0f,1.0f);
                glVertex2i(backgroundtex->w, backgroundtex->h);

                glTexCoord2f(0.0f,1.0f);
                glVertex2i(0, backgroundtex->h);
            glEnd();

        glPopMatrix();
    }
}

void Gource::drawScene(float dt) {

    //draw edges

    draw_edges_time = SDL_GetTicks();

    updateAndDrawEdges();

    draw_edges_time = SDL_GetTicks() - draw_edges_time;

    //draw file shadows

    draw_shadows_time = SDL_GetTicks();

    drawFileShadows(dt);

    draw_shadows_time = SDL_GetTicks() - draw_shadows_time;

    //draw actions

    draw_actions_time = SDL_GetTicks();

    drawActions(dt);

    draw_actions_time = SDL_GetTicks() - draw_actions_time;

    //draw files

    draw_files_time = SDL_GetTicks();

    drawFiles(dt);

    draw_files_time = SDL_GetTicks() - draw_files_time;

    //draw users

    draw_users_time = SDL_GetTicks();

    drawUserShadows(dt);

    drawUsers(dt);

    draw_users_time = SDL_GetTicks() - draw_users_time;

    //draw bloom

    draw_bloom_time = SDL_GetTicks();

    drawBloom(dt);

    draw_bloom_time = SDL_GetTicks() - draw_bloom_time;

}

void Gource::updateAndDrawEdges() {
    if(gGourceSettings.hide_tree) return;

    root->calcEdges();

    //switch to 2d
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, display.width, display.height, 0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, beamtex->textureid);

    if(!gGourceSettings.ffp) {

        edge_vbo.reset();

        root->updateEdgeVBO(edge_vbo);

        edge_vbo.update();

        shadow_shader->use();
        shadow_shader->setFloat("shadow_strength", 0.5);

        vec2f shadow_offset = vec2f(2.0, 2.0);

        glPushMatrix();
            glTranslatef(shadow_offset.x, shadow_offset.y, 0.0f);
            edge_vbo.draw();
        glPopMatrix();

        glUseProgramObjectARB(0);

        edge_vbo.draw();

    } else {
        root->drawEdgeShadows();
        root->drawEdges();
    }

    //switch back
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Gource::drawActions(float dt) {
    if(gGourceSettings.hide_users) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, beamtex->textureid);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!gGourceSettings.ffp) {

        action_vbo.draw();

    } else {

        //draw actions
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            it->second->drawActions(dt);
        }
    }
}

void Gource::drawBloom(float dt) {
    if(gGourceSettings.hide_bloom) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);

    if(!gGourceSettings.ffp) {

        bloom_shader->use();

        bloom_vbo.draw();

        glUseProgramObjectARB(0);

    } else {

        //draw 'gourceian blur' around dirnodes
        glBindTexture(GL_TEXTURE_2D, bloomtex->textureid);

        root->drawBloom(dt);
    }
}

void Gource::setMessage(const char* str, ...) {

    char msgbuff[1024];

    va_list vl;

    va_start(vl, str);
        vsnprintf(msgbuff, 1024, str, vl);
    va_end(vl);

    message = std::string(msgbuff);
    message_timer = 5.0;
}

void Gource::screenshot() {

    char* screenbuff = new char[display.width * display.height * 4];

    glReadPixels(0, 0, display.width, display.height,
                 GL_RGBA, GL_UNSIGNED_BYTE, screenbuff);

    const char tga_header[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    short width           = display.width;
    short height          = display.height;
    char  bitsperpixel    = 32;
    char  imagedescriptor = 8;

    //get next free recording name
    char tganame[256];
    struct stat finfo;
    int tgano = 1;

    while(tgano < 10000) {
        snprintf(tganame, 256, "gource-%04d.tga", tgano);
        if(stat(tganame, &finfo) != 0) break;
        tgano++;
    }

    //write tga
    std::string filename(tganame);

    std::ofstream tga;
    tga.open(filename.c_str(), std::ios::out | std::ios::binary );

    if(!tga.is_open()) return;

    tga.write(tga_header, 12);
    tga.write((char*)&width, sizeof(short));
    tga.write((char*)&height, sizeof(short));
    tga.write(&bitsperpixel, 1);
    tga.write(&imagedescriptor, 1);

    tga.write(screenbuff, display.width * display.height * 4);
    tga.close();

    delete[] screenbuff;

    setMessage("Wrote screenshot %s", tganame);
}

void Gource::updateVBOs(float dt) {
    if(gGourceSettings.ffp) return;

    if(!gGourceSettings.hide_users) {

        user_vbo.reset();
        action_vbo.reset();

        //use a separate vbo for each user texture
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            RUser* user = it->second;

            float alpha = user->getAlpha();
            vec3f col   = user->getColour();

            user_vbo.add(user->graphic->textureid, user->getPos() - user->dims*0.5f, user->dims, vec4f(col.x, col.y, col.z, alpha));

            //draw actions
            user->updateActionsVBO(action_vbo);
        }

        user_vbo.update();
        action_vbo.update();
    }

    if(!gGourceSettings.hide_bloom) {
        bloom_vbo.reset();
        root->updateBloomVBO(bloom_vbo, dt);
        bloom_vbo.update();
    }

    if(!gGourceSettings.hide_files) {
        file_vbo.reset();
        root->updateFilesVBO(file_vbo, dt);
        file_vbo.update();
    }
}

void Gource::drawFileShadows(float dt) {
    if(gGourceSettings.hide_files) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!gGourceSettings.ffp) {

        shadow_shader->use();
        shadow_shader->setFloat("shadow_strength", 0.5);

        glBindTexture(GL_TEXTURE_2D, gGourceSettings.file_graphic->textureid);

        glPushMatrix();
            glTranslatef(2.0f, 2.0f, 0.0f);
            file_vbo.draw();
        glPopMatrix();

        glUseProgramObjectARB(0);
    } else {
        root->drawShadows(dt);
    }
}

void Gource::drawUserShadows(float dt) {
    if(gGourceSettings.hide_users) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!gGourceSettings.ffp) {

        shadow_shader->use();
        shadow_shader->setFloat("shadow_strength", 0.5);

        vec2f shadow_offset = vec2f(2.0, 2.0) * gGourceSettings.user_scale;

        glPushMatrix();
            glTranslatef(shadow_offset.x, shadow_offset.y, 0.0f);

            user_vbo.draw();

        glPopMatrix();

        glUseProgramObjectARB(0);
    } else {
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            it->second->drawShadow(dt);
        }
    }
}

void Gource::drawFiles(float dt) {
    if(gGourceSettings.hide_files) return;


    if(trace_debug) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
    }

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!gGourceSettings.ffp) {
        glBindTexture(GL_TEXTURE_2D, gGourceSettings.file_graphic->textureid);

        file_vbo.draw();
    } else {
        root->drawFiles(dt);
    }
}

void Gource::drawUsers(float dt) {
    if(gGourceSettings.hide_users) return;

    if(trace_debug) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
    }

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!gGourceSettings.ffp) {

        user_vbo.draw();

    } else {

        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            it->second->draw(dt);
        }
    }

}

void Gource::draw(float t, float dt) {

    display.mode2D();

    drawBackground(dt);

    if(draw_loading) {
        loadingScreen();
        draw_loading = false;
        return;
    }

    Frustum frustum(camera);

    trace_time = SDL_GetTicks();

    if(!gGourceSettings.hide_mouse && cursor.isVisible()) {
        mousetrace(dt);
    } else {
        if(hoverUser) {
            hoverUser->setMouseOver(false);
            hoverUser = 0;
        }
        if(hoverFile) {
            hoverFile->setMouseOver(false);
            hoverFile = 0;
        }
    }

    trace_time = SDL_GetTicks() - trace_time;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.focus();

    //check visibility
    root->checkFrustum(frustum);

    screen_project_time = SDL_GetTicks();

    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];

    glGetIntegerv( GL_VIEWPORT, viewport );
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );

    root->calcScreenPos(viewport, modelview, projection);

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        it->second->calcScreenPos(viewport, modelview, projection);
    }

    //need to calc screen pos of selected file if hiding other
    //file names
    if(selectedFile!=0 && gGourceSettings.hide_filenames) {
        selectedFile->calcScreenPos(viewport, modelview, projection);
    }

    screen_project_time = SDL_GetTicks() - screen_project_time;

    //update file and user vbos

    update_vbos_time = SDL_GetTicks();

    updateVBOs(dt);

    update_vbos_time = SDL_GetTicks() - update_vbos_time;

    //draw scene

    draw_scene_time = SDL_GetTicks();

    drawScene(dt);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    draw_scene_time = SDL_GetTicks() - draw_scene_time;

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    text_time = text_update_time = SDL_GetTicks();

    //switch to 2D, preserve current state
    display.push2D();

    if(!gGourceSettings.ffp) {
        fontmanager.startBuffer();
    }

    font.roundCoordinates(false);

    root->drawNames(font);

   if(!(gGourceSettings.hide_usernames || gGourceSettings.hide_users)) {
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            RUser* user = it->second;
            if(!user->isSelected()) {
                user->drawName();
            }
        }
    }

    text_update_time = SDL_GetTicks() - text_update_time;

    text_vbo_commit_time = 0;
    text_vbo_draw_time   = 0;

    if(!gGourceSettings.ffp) {

        text_vbo_commit_time = SDL_GetTicks();

        fontmanager.commitBuffer();

        text_vbo_commit_time = SDL_GetTicks() - text_vbo_commit_time;

        text_vbo_draw_time = SDL_GetTicks();

        text_shader->use();
        text_shader->setFloat("shadow_strength", 0.7);
        text_shader->setFloat("texel_size", font_texel_size);

        fontmanager.drawBuffer();

        glUseProgramObjectARB(0);

        text_vbo_draw_time = SDL_GetTicks() - text_vbo_draw_time;
    }

    //draw selected item names again so they are over the top
    if(selectedUser !=0) selectedUser->drawName();

    if(selectedFile !=0) {
            selectedFile->drawName();
    }

    //switch back
    display.pop2D();

    text_time = SDL_GetTicks() - text_time;

    if(debug) {
        glDisable(GL_TEXTURE_2D);
        glLineWidth(2.0);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        track_users ? active_user_bounds.draw() : dir_bounds.draw();
    }

    if(gGourceQuadTreeDebug) {
        glDisable(GL_TEXTURE_2D);

        glLineWidth(1.0);

        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        dirNodeTree->outline();

        glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

        userTree->outline();

        glColor4f(0.0f, 1.0f, 0.5f, 1.0f);

        userTree->outlineItems();
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    vec3f campos = camera.getPos();

    if(logotex!=0) {
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);

        glColor4f(1.0, 1.0, 1.0, 1.0);

        glBindTexture(GL_TEXTURE_2D, logotex->textureid);

        vec2f logopos = vec2f(display.width, display.height) - vec2f(logotex->w, logotex->h) - gGourceSettings.logo_offset;

        glPushMatrix();

            glTranslatef(logopos.x, logopos.y, 0.0);

            glBegin(GL_QUADS);
                glTexCoord2f(0.0f,0.0f);
                glVertex2i(0, 0);

                glTexCoord2f(1.0f,0.0f);
                glVertex2i(logotex->w, 0);

                glTexCoord2f(1.0f,1.0f);
                glVertex2i(logotex->w, logotex->h);

                glTexCoord2f(0.0f,1.0f);
                glVertex2i(0, logotex->h);
            glEnd();

        glPopMatrix();
    }

    font.roundCoordinates(true);

    if(splash>0.0f) {
        int logowidth = fontlarge.getWidth("Gource");
        int logoheight = 100;
        int cwidth    = font.getWidth("Software Version Control Visualization");
        int awidth    = font.getWidth("(C) 2009 Andrew Caudwell");

        vec2f corner(display.width/2 - logowidth/2 - 30.0f, display.height/2 - 40);

        glDisable(GL_TEXTURE_2D);
        glColor4f(0.0f, 0.5f, 1.0f, splash * 0.015f);
        glBegin(GL_QUADS);
            glVertex2f(0.0f,                 corner.y);
            glVertex2f(0.0f,                 corner.y + logoheight);
            glVertex2f(display.width, corner.y + logoheight);
            glVertex2f(display.width, corner.y);
        glEnd();

        glEnable(GL_TEXTURE_2D);

        glColor4f(1.0,1.0,1.0,1.0);
        fontlarge.draw(display.width/2 - logowidth/2,display.height/2 - 30, "Gource");
        font.draw(display.width/2 - cwidth/2,display.height/2 + 10, "Software Version Control Visualization");
        font.draw(display.width/2 - awidth/2,display.height/2 + 30, "(C) 2009 Andrew Caudwell");
    }

    // text using the specified font goes here

    fontmedium.setColour(vec4f(gGourceSettings.font_colour, 1.0f));

    if(!gGourceSettings.hide_date) {
        fontmedium.draw(display.width/2 - date_x_offset, 20, displaydate);
    }

    if(gGourceSettings.title.size()>0) {
        fontmedium.alignTop(false);
        fontmedium.draw(10, display.height - 10, gGourceSettings.title);
        fontmedium.alignTop(true);
    }

    if(message_timer>0.0f) {
         fontmedium.draw(1, 3, message);
    }

    //file key
    file_key.draw();
    file_key.setShow(gGourceSettings.show_key);

    //slider
    if(canSeek()) {
        slider.draw(dt);
    }

    //text box
    if(hoverFile && hoverFile != selectedFile) {

        std::string display_path = hoverFile->path;
        display_path.erase(0,1);

        textbox.setText(hoverFile->getName());
        if(display_path.size()) textbox.addLine(display_path);
        textbox.setColour(hoverFile->getColour());

        textbox.setPos(mousepos, true);
        textbox.draw();
    } else if(hoverUser && hoverUser != selectedUser) {

        textbox.setText(hoverUser->getName());
        textbox.setColour(hoverUser->getColour());

        textbox.setPos(mousepos, true);
        textbox.draw();
    }

    //debug info

    if(debug) {
        font.setAlpha(1.0f);

        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);


        font.print(1,20, "FPS: %.2f", fps);
        font.print(1,40,"Days Per Second: %.2f",
            gGourceSettings.days_per_second);
        font.print(1,60,"Time Scale: %.2f", gGourceSettings.time_scale);
        font.print(1,80,"Users: %d", users.size());
        font.print(1,100,"Files: %d", files.size());
        font.print(1,120,"Dirs: %d",  gGourceDirMap.size());

        font.print(1,140,"Log Position: %.4f", commitlog->getPercent());
        font.print(1,160,"Camera: (%.2f, %.2f, %.2f)", campos.x, campos.y, campos.z);
        font.print(1,180,"Gravity: %.2f", gGourceForceGravity);
        font.print(1,200,"Update Tree: %u ms", update_dir_tree_time);
        font.print(1,220,"Update VBOs: %u ms", update_vbos_time);
        font.print(1,240,"Projection: %u ms",  screen_project_time);

        font.print(1,260,"Draw Scene: %u ms",  draw_scene_time);
        font.print(1,280," - Edges: %u ms",   draw_edges_time);
        font.print(1,300," - Shadows: %u ms", draw_shadows_time);
        font.print(1,320," - Actions: %u ms", draw_actions_time);
        font.print(1,340," - Files: %u ms",   draw_files_time);
        font.print(1,360," - Users: %u ms",   draw_users_time);
        font.print(1,380," - Bloom: %u ms",   draw_bloom_time);
        font.print(1,400,"Text: %u ms",       text_time);
        font.print(1,420,"- Update: %u ms",   text_update_time);
        font.print(1,440,"- VBO Commit: %u ms", text_vbo_commit_time);
        font.print(1,460,"- VBO Draw: %u ms",   text_vbo_draw_time);
        font.print(1,480,"Mouse Trace: %u ms", trace_time);
        font.print(1,500,"Logic Time: %u ms", logic_time);
        font.print(1,520,"File Inner Loops: %d", gGourceFileInnerLoops);
        font.print(1,540,"User Inner Loops: %d", gGourceUserInnerLoops);

        font.print(1,560,"Dir Inner Loops: %d (QTree items = %d, nodes = %d, max node depth = %d)", gGourceDirNodeInnerLoops,
            dirNodeTree->item_count, dirNodeTree->node_count, dirNodeTree->max_node_depth);

        font.print(1,580,"Dir Bounds Ratio: %.2f, %.5f", dir_bounds.width() / dir_bounds.height(), rotation_remaining_angle);
        font.print(1,600,"String Hash Seed: %d", gStringHashSeed);

        if(!gGourceSettings.ffp) {
            font.print(1,620,"Text VBO: %d/%d vertices, %d texture changes", fontmanager.font_vbo.vertices(), fontmanager.font_vbo.capacity(), fontmanager.font_vbo.texture_changes());
            font.print(1,640,"File VBO: %d/%d vertices, %d texture changes", file_vbo.vertices(), file_vbo.capacity(), file_vbo.texture_changes());
            font.print(1,660,"User VBO: %d/%d vertices, %d texture changes", user_vbo.vertices(), user_vbo.capacity(), user_vbo.texture_changes());
            font.print(1,680,"Action VBO: %d/%d vertices", action_vbo.vertices(), action_vbo.capacity());
            font.print(1,700,"Bloom VBO: %d/%d vertices", bloom_vbo.vertices(), bloom_vbo.capacity());
            font.print(1,720,"Edge VBO: %d/%d vertices",  edge_vbo.vertices(), edge_vbo.capacity());
        }

        if(selectedUser != 0) {

        }

        if(selectedFile != 0) {
            font.print(1,740,"%s: %d files (%d visible)", selectedFile->getDir()->getPath().c_str(),
                    selectedFile->getDir()->fileCount(), selectedFile->getDir()->visibleFileCount());
        }
    }

    mousemoved=false;
    mouseclicked=false;
}
