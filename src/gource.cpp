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

float gGourceAutoSkipSeconds = 3.0;
bool  gGourceDrawBackground  = true;
bool  gGourceFileLoop        = false;
bool  gGourceHideTree        = false;
bool  gGourceHideUsernames   = false;
bool  gGourceHideDate        = false;
bool  gGourceDisableBloom    = false;
bool  gGourceDisableProgress = false;
bool  gGourceQuadTreeDebug   = false;

std::string gGourceLogFormat;
std::string gGourceDateFormat("%A, %d %B, %Y %X");

bool  gGourceHighlightAllUsers = false;

int gGourceMaxFiles = 1000;

int gGourceUserInnerLoops = 0;

void gource_info(std::string msg) {
    SDLAppInfo(msg);
}

void gource_quit(std::string error) {
    SDLAppQuit(error);
}

//display help message
void gource_help() {

#ifdef _WIN32
    SDLAppCreateWindowsConsole();

    //resize window to fit help message
    SDLAppResizeWindowsConsole(950);
#endif

    printf("Gource v%s\n", GOURCE_VERSION);

    printf("Usage: gource [OPTIONS] [PATH]\n");
    printf("\nOptions:\n");
    printf("  -h, --help                       Help\n\n");
    printf("  -WIDTHxHEIGHT                    Set window size\n");
    printf("  -f                               Fullscreen\n\n");
    printf("  -p, --start-position POSITION    Begin at some position in the log (0.0-1.0)\n");
    printf("      --stop-position  POSITION    Stop at some position\n");
    printf("      --stop-on-idle               Stop on break in activity\n");
    printf("      --stop-at-end                Stop at end of the log\n");
    printf("      --loop                       Loop at the end of the log\n\n");

    printf("  -a, --auto-skip-seconds SECONDS  Auto skip to next entry if nothing happens\n");
    printf("                                   for a number of seconds (default: 3)\n");
    printf("  -s, --seconds-per-day SECONDS    Speed in seconds per day (default: 4)\n");
    printf("      --realtime                   Realtime playback speed\n");
    printf("  -i, --file-idle-time SECONDS     Time files remain idle (default: 60)\n");
    printf("  -e, --elasticity FLOAT           Elasticity of nodes\n");
    printf("  -b, --background FFFFFF          Background colour in hex\n\n");

    printf("  --user-image-dir DIRECTORY       Dir containing images to use as avatars\n");
    printf("  --default-user-image IMAGE       Default user image file\n");
    printf("  --colour-images                  Colourize user images\n\n");

    printf("  --date-format FORMAT     Specify display date string (strftime format)\n");
    printf("  --log-format  FORMAT     Specify format of log (git,cvs,hg,bzr,custom)\n");
    printf("  --git-branch             Get the git log of a particular branch\n");
    printf("  --git-log-command        Show git log command used by gource\n");
    printf("  --cvs-exp-command        Show cvs-exp.pl log command used by gource\n");
    printf("  --hg-log-command         Show hg log (Mercurial) command used by gource\n");
    printf("  --bzr-log-command        Show bzr log (Bazaar) command used by gource\n\n");

    printf("  --multi-sampling         Enable multi-sampling\n");
    printf("  --crop AXIS              Crop view on an axis (vertical,horizontal)\n\n");

    printf("  --bloom-multiplier       Adjust the amount of bloom (default: 1.0)\n");
    printf("  --bloom-intensity        Adjust the intensity of the bloom (default: 0.75)\n\n");

    printf("  --disable-auto-skip      Disable auto skipping\n");
    printf("  --disable-progress       Disable the progress bar\n");
    printf("  --disable-bloom          Disable bloom effect\n\n");

    printf("  --hide DISPLAY_ELEMENT   date,users,files,tree,usernames,filenames,dirnames\n\n");

    printf("  --max-files NUMBER       Max number of active files (default: 1000)\n");
    printf("  --max-file-lag SECONDS   Max time files of a commit can take to appear\n\n");

    printf("  --max-user-speed UNITS   Speed users can travel per second (default: 500)\n\n");
    printf("  --user-friction SECONDS  Time users come to a complete hault (default: 0.67)\n");
    printf("  --user-scale SCALE       Change scale of users (default: 1.0)\n\n");

    printf("  --camera-mode MODE       Camera mode (overview,track)\n\n");

    printf("  --follow-user USER       Camera will automatically follow this user\n");
    printf("  --highlight-user USER    Highlight the names of a particular user\n");
    printf("  --highlight-all-users    Highlight the names of all users\n");
    printf("  --file-filter REGEX      Ignore files matching this regexe\n\n");

    printf("  --output-ppm-stream FILE Write frames as PPM to a file ('-' for STDOUT)\n");
    printf("  --output-framerate FPS   Framerate of output (25,30,60)\n\n");

    printf("PATH may be a Git, Bazaar or Mercurial dir, a log file or '-' to read STDIN.\n");
    printf("If ommited, gource will attempt to generate a log from the current directory.\n\n");

#ifdef _WIN32
    printf("Press Enter\n");
    getchar();
#endif

    exit(0);
}

RCommitLog* Gource::determineFormat(std::string logfile) {
    debugLog("determineFormat(%s)\n", logfile.c_str());

    RCommitLog* clog = 0;

    //we've been told what format to use
    if(gGourceLogFormat.size() > 0) {
        debugLog("--log-format = %s\n", gGourceLogFormat.c_str());

        if(gGourceLogFormat == "git") {
            clog = new GitCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;

            clog = new GitRawCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(gGourceLogFormat == "hg") {
            clog = new MercurialLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }
        if(gGourceLogFormat == "bzr") {
            clog = new BazaarLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(gGourceLogFormat == "cvs") {
            clog = new CVSEXPCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(gGourceLogFormat == "custom") {
            clog = new CustomLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(gGourceLogFormat == "apache") {
            clog = new ApacheCombinedLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        return 0;
    }

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

Gource::Gource(std::string logfile) {

    this->logfile = logfile;

    commitlog = 0;

    fontlarge = fontmanager.grab("FreeSans.ttf", 42);
    fontlarge.dropShadow(true);
    fontlarge.roundCoordinates(true);

    fontmedium = fontmanager.grab("FreeSans.ttf", 16);
    fontmedium.dropShadow(true);
    fontmedium.roundCoordinates(false);

    font = fontmanager.grab("FreeSans.ttf", 14);
    font.dropShadow(true);
    font.roundCoordinates(true);

    bloomtex = texturemanager.grab("bloom.tga");
    beamtex  = texturemanager.grab("beam.png");

    start_position = 0.0;
    stop_position = 0.0;

    stop_on_idle=false;
    stop_position_reached=false;
    stop_at_end=false;

    paused     = false;
    first_read = true;
    draw_loading = true;

    mousemoved = false;
    mousedragged = false;
    mouseclicked = false;

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

    camera = ZoomCamera(vec3f(0,0, -300), vec3f(0.0, 0.0, 0.0), 250.0, 5000.0);

    setCameraMode(false);

    root = 0;

    background_colour = vec3f(0.0, 0.0, 0.0);

    //min phsyics rate 60fps (ie maximum allowed delta 1.0/60)
    max_tick_rate = 1.0 / 60.0;
    runtime = 0.0f;
    frameskip = 0;
    framecount = 0;

    findUserImages();

    reset();
}

Gource::~Gource() {
    reset();

    if(commitlog!=0) delete commitlog;
    if(root!=0) delete root;

    //delete filters
    for(std::vector<Regex*>::iterator it = filters.begin(); it != filters.end(); it++) {
        delete (*it);
    }
    filters.clear();
}

void Gource::init() {
}

void Gource::update(float t, float dt) {

    dt = std::min(dt, max_tick_rate);

    //if exporting a video use a fixed tick rate rather than time based
    if(frameExporter != 0) {
        dt = max_tick_rate;
    }

    dt *= time_scale;

    //have to manage runtime internally as we're messing with dt
    if(!paused) runtime += dt;

    logic_time = SDL_GetTicks();

    logic(runtime, dt);

    logic_time = SDL_GetTicks() - logic_time;

    draw_time = SDL_GetTicks();

    draw(runtime, dt);

    //extract frames based on frameskip setting if frameExporter defined
    if(frameExporter != 0) {
        if(framecount % (frameskip+1) == 0) {
            frameExporter->dump();
        }
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

    //move camera in direction the user dragged the mouse
    if(mousedragged) {
        vec2f mag( e->xrel, e->yrel );

        if(mag.length()>100.0) return;

        backgroundPos += mag;

        return;
    }

    mousepos = vec2f(e->x, e->y);
    mousemoved=true;

    float pos;

    if(!gGourceDisableProgress && slider.mouseOver(mousepos, &pos)) {
        std::string date = dateAtPosition(pos);
        slider.setCaption(date);
    }

}

void Gource::zoom(bool zoomin) {

    float min_distance = camera.getMinDistance();
    float max_distance = camera.getMaxDistance();

    float zoom_multi = 1.1;

    if(zoomin) {
        min_distance /= zoom_multi;
        if(min_distance < 100.0) min_distance = 100.0;

        camera.setMinDistance(min_distance);
    } else {
        min_distance *= zoom_multi;
        if(min_distance > 1000.0) min_distance = 1000.0;

        camera.setMinDistance(min_distance);
    }
}

void Gource::mouseClick(SDL_MouseButtonEvent *e) {
    if(commitlog==0) return;

    if(e->type == SDL_MOUSEBUTTONUP) {

        if(e->button == SDL_BUTTON_LEFT) {

            //stop dragging mouse, return the mouse to where
            //the user started dragging.
            if(mousedragged) {
                SDL_WM_GrabInput(SDL_GRAB_OFF);
                SDL_ShowCursor(true);
                SDL_WarpMouse(mousepos.x, mousepos.y);
                mousedragged=false;
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

    if(e->button == SDL_BUTTON_RIGHT) {
        toggleCameraMode();
        return;
    }

    if(e->button == SDL_BUTTON_LEFT) {

        mousepos = vec2f(e->x, e->y);
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

void Gource::setBackground(vec3f background) {
    background_colour = background;
}

void Gource::setCameraMode(bool track_users) {
    this->track_users = track_users;
    if(selectedUser!=0) camera.lockOn(track_users);
    backgroundSelected=false;
}

void Gource::toggleCameraMode() {
    setCameraMode(!track_users);
}

//trace click of mouse on background
void Gource::selectBackground() {
    //is the left mouse button down?

    Uint8 ms = SDL_GetMouseState(0,0);

    if(!(ms & SDL_BUTTON(SDL_BUTTON_LEFT))) return;

    selectUser(0);

    backgroundSelected = true;

    backgroundPos = camera.getPos().truncate();

    SDL_ShowCursor(false);
    SDL_WM_GrabInput(SDL_GRAB_ON);

    mousedragged=true;
}

//select a user, deselect current file/user
void Gource::selectUser(RUser* user) {
    //already selected do nothing
    if(user!=0 && selectedUser==user) return;

    backgroundSelected=false;

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

    backgroundSelected=false;

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
    debugLog("selectNextUser()\n");

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
        if (e->keysym.sym == SDLK_ESCAPE) {
            appFinished=true;
        }

        if(commitlog==0) return;

        if (e->keysym.sym == SDLK_q) {
            debug = !debug;
        }

        if (e->keysym.sym == SDLK_w) {
            trace_debug = !trace_debug;
        }

        if (e->keysym.sym == SDLK_m) {

            Uint8 ms = SDL_GetMouseState(0,0);

            //toggle mouse visiblity unless mouse clicked/pressed/dragged
            if(!(mousedragged || mouseclicked || ms & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                if(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) {
                    SDL_ShowCursor(false);
                } else {
                    SDL_ShowCursor(true);
                }
            }
        }

        if (e->keysym.sym == SDLK_n) {
            idle_time = gGourceAutoSkipSeconds;
        }

        if (e->keysym.sym == SDLK_t) {
            gGourceQuadTreeDebug = !gGourceQuadTreeDebug;
        }

        if (e->keysym.sym == SDLK_g) {
            gGourceHideUsers = !gGourceHideUsers;
        }

        if (e->keysym.sym == SDLK_u) {
            gGourceHideUsernames = !gGourceHideUsernames;
        }

        if (e->keysym.sym == SDLK_d) {
            gGourceDrawDirName = !gGourceDrawDirName;
        }

        if (e->keysym.sym == SDLK_f) {
            gGourceHideFilenames = !gGourceHideFilenames;
        }

        if(e->keysym.sym == SDLK_c) {
            splash = 15.0f;
        }

        if (e->keysym.sym == SDLK_v) {
            toggleCameraMode();
        }

        if (e->keysym.sym == SDLK_z) {
            gGourceGravity = !gGourceGravity;
        }

        if(e->keysym.sym == SDLK_TAB) {
            selectNextUser();
        }

        if (e->keysym.sym == SDLK_SPACE) {
            paused = !paused;
        }

        if (e->keysym.sym == SDLK_EQUALS) {
            if(gGourceDaysPerSecond>=1.0) {
                gGourceDaysPerSecond = std::min(30.0f, floorf(gGourceDaysPerSecond) + 1.0f);
            } else {
                gGourceDaysPerSecond = std::min(1.0f, gGourceDaysPerSecond * 2.0f);
            }
        }

        if (e->keysym.sym == SDLK_MINUS) {
            if(gGourceDaysPerSecond>1.0) {
                gGourceDaysPerSecond = std::max(0.0f, floorf(gGourceDaysPerSecond) - 1.0f);
            } else {
                gGourceDaysPerSecond = std::max(0.0f, gGourceDaysPerSecond * 0.5f);
            }
        }

        if(e->keysym.sym == SDLK_UP) {
            zoom(true);
        }

        if(e->keysym.sym == SDLK_DOWN) {
            zoom(false);
        }

        if(e->keysym.sym == SDLK_LEFTBRACKET) {
            gGourceForceGravity /= 1.1;
        }

        if(e->keysym.sym == SDLK_RIGHTBRACKET) {
            gGourceForceGravity *= 1.1;
        }

        if(e->keysym.sym == SDLK_PERIOD) {

            if(time_scale>=1.0) {
                time_scale = std::min(4.0f, floorf(time_scale) + 1.0f);
            } else {
                time_scale = std::min(1.0f, time_scale * 2.0f);
            }
        }

        if(e->keysym.sym == SDLK_COMMA) {

            if(time_scale>1.0) {
                time_scale = std::max(0.0f, floorf(time_scale) - 1.0f);
            } else {
                time_scale = std::max(0.25f, time_scale * 0.5f);
            }
        }

        if(e->keysym.sym == SDLK_SLASH) {
            time_scale = 1.0f;
        }
    }
}

void Gource::findUserImages() {
    if(!gGourceUserImageDir.size()) return;

    //get jpg and png images in dir
    DIR *dp;
    struct dirent *dirp;

    if((dp = opendir(gGourceUserImageDir.c_str())) == 0) return;

    while ((dirp = readdir(dp)) != 0) {
        std::string dirfile = std::string(dirp->d_name);

        int extpos = 0;

        if(   (extpos=dirfile.rfind(".jpg"))  == std::string::npos
           && (extpos=dirfile.rfind(".jpeg")) == std::string::npos
           && (extpos=dirfile.rfind(".png"))  == std::string::npos) continue;


        std::string image_path = gGourceUserImageDir + dirfile;
        std::string name       = dirfile.substr(0,extpos);

        debugLog("%s => %s\n", name.c_str(), image_path.c_str());

        gGourceUserImageMap[name] = image_path;
    }

    closedir(dp);
}

void Gource::reset() {
    camera.reset();
    user_bounds.reset();
    dir_bounds.reset();
    commitqueue.clear();
    tagfilemap.clear();
    tagusermap.clear();
    gGourceRemovedFiles.clear();

    if(userTree!=0) delete userTree;
    if(dirNodeTree!=0) delete dirNodeTree;

    userTree = 0;
    dirNodeTree = 0;

    selectedFile = 0;
    hoverFile = 0;

    selectedUser = 0;
    hoverUser = 0;
    backgroundSelected=false;

    mouseclicked=false;
    mousemoved=false;
    mousedragged = false;

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

    time_scale = 1.0f;
    idle_time=0;
    currtime=0;
    subseconds=0.0;
    tag_seq = 1;
    commit_seq = 1;
}

void Gource::deleteFile(RFile* file) {
    debugLog("removing file %s\n", file->getFullPath().c_str());

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

    files.erase(file->getFullPath());
    tagfilemap.erase(file->getTagID());

    debugLog("removed file %s\n", file->getFullPath().c_str());

    delete file;
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

    debugLog("deleted user %s, tagid = %d\n", user->getName().c_str(), user->getTagID());

    delete user;
}

void Gource::addFollowUser(std::string user) {
    follow_users.push_back(user);
}

void Gource::addHighlightUser(std::string user) {
    highlight_users.push_back(user);
}

void Gource::addFilter(Regex* filter) {
    filters.push_back(filter);
}

void Gource::setStartPosition(float percent) {
    start_position = percent;
}

void Gource::setStopAtEnd(bool stop_at_end) {
    this->stop_at_end = stop_at_end;
}

void Gource::setStopOnIdle(bool stop_on_idle) {
    this->stop_on_idle = stop_on_idle;
}

void Gource::setStopPosition(float percent) {
    stop_position = percent;
}

bool Gource::canSeek() {
    if(gGourceDisableProgress || commitlog == 0 || !commitlog->isSeekable()) return false;

    return true;
}

void Gource::seekTo(float percent) {
    debugLog("seekTo(%.2f)\n", percent);

    if(commitlog == 0 || !commitlog->isSeekable()) return;

    // end pause
    if(paused) paused = false;

    reset();

    commitlog->seekTo(percent);
}

void Gource::readLog() {
    //debugLog("readLog()\n");

    while(!commitlog->isFinished() && commitqueue.size() < 1) {

        RCommit commit;

        //ignore blank commits
        if(commitlog->nextCommit(commit) && commit.files.size() > 0) {
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

    //see if we have reached the end and should exit
    //the next time all users are idle
    if(   stop_at_end && is_finished
       || stop_position > 0.0 && commitlog->isSeekable() && (is_finished || last_percent >= stop_position)) {

        stop_position_reached = true;

        //if not stopping on idle exit immediately
        if(!stop_on_idle) appFinished = true;
    }

    // useful to figure out where we have crashes
    //debugLog("current date: %s\n", displaydate.c_str());
}

void Gource::processCommit(RCommit& commit, float t) {

    //find user of this commit or create them
    RUser* user = 0;

    //see if user already exists but if not wait until
    //we actually use one of their files before adding them
    std::map<std::string, RUser*>::iterator seen_user = users.find(commit.username);
    if(seen_user != users.end()) user = seen_user->second;

    //find files of this commit or create it
    for(std::list<RCommitFile>::iterator it = commit.files.begin(); it != commit.files.end(); it++) {

        RCommitFile& cf = *it;

        bool filtered_filename = false;

        //check filename against filters
        for(std::vector<Regex*>::iterator ri = filters.begin(); ri != filters.end(); ri++) {
            Regex* r = *ri;

            if(r->match(cf.filename)) {
                filtered_filename = true;
                break;
            }
        }

        if(filtered_filename) continue;

        std::map<std::string, RFile*>::iterator seen_file;

        seen_file = files.find(cf.filename);

        RFile* file = 0;

        if(seen_file != files.end()) {

            file = seen_file->second;

        } else {

            //if we already have max files in circulation
            //we cant add any more
            if(files.size() >= gGourceMaxFiles) continue;

            int tagid = tag_seq++;

            file = new RFile(cf.filename, cf.colour, vec2f(0.0,0.0), tagid);

            files[cf.filename] = file;
            tagfilemap[tagid]  = file;

            root->addFile(file);

            while(root->getParent() != 0) {
                debugLog("parent changed to %s\n", root->getPath().c_str());
                root = root->getParent();
            }
        }

        //create user if havent yet. do it here to ensure at least one of there files
        //was added (incase we hit gGourceMaxFiles!)

        if(user == 0) {
            vec2f pos;

            if(dir_bounds.area() > 0) {
                pos = dir_bounds.centre();
            } else {
                pos = vec2f(0,0);
            }

            int tagid = tag_seq++;

            user = new RUser(commit.username, pos, tagid);

            users[commit.username] = user;
            tagusermap[tagid]     = user;

            if(gGourceHighlightAllUsers) {
                user->setHighlighted(true);
            } else {

                // set the highlighted flag if name matches a highlighted user
                for(std::vector<std::string>::iterator hi = highlight_users.begin(); hi != highlight_users.end(); hi++) {
                    std::string highlight = *hi;

                    if(highlight.size() && user->getName() == highlight) {
                        user->setHighlighted(true);
                        break;
                    }
                }
            }

            debugLog("added user %s, tagid = %d\n", commit.username.c_str(), tagid);
        }

        //create action

        RAction* action = 0;

        int commitNo = commit_seq++;

        if(cf.action == "D") {
            action = new RemoveAction(user, file, t);
        } else {
            if(cf.action == "A") {
                action = new CreateAction(user, file, t);
            } else {
                action = new ModifyAction(user, file, t);
            }
        }

        user->addAction(action);
    }
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
        max_depth = 6;
    }

    userTree = new QuadTree(quadtreebounds, max_depth, 1);

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        RUser* user = it->second;

        user->updateQuadItemBounds();
        userTree->addItem(user);
    }

    //move users - interact with other users and files
    for(std::map<std::string,RUser*>::iterator ait = users.begin(); ait!=users.end(); ait++) {

        RUser* a = ait->second;

        std::set<int> seen;
        std::set<int>::iterator seentest;

        std::vector<QuadItem*> inbounds;

        int found = userTree->getItemsInBounds(inbounds, a->quadItemBounds);

        for(std::vector<QuadItem*>::iterator it = inbounds.begin(); it != inbounds.end(); it++) {

            RUser* b = (RUser*) (*it);

            if(b==a) continue;

            if((seentest = seen.find(b->getTagID())) != seen.end()) {
                continue;
            }

            seen.insert(b->getTagID());

            a->applyForceUser(b);
            gGourceUserInnerLoops++;
        }

        a->applyForceToActions();
    }

    update_user_tree_time = SDL_GetTicks() - update_user_tree_time;
}


void Gource::updateUsers(float t, float dt) {
    std::vector<RUser*> inactiveUsers;

    int idle_users = 0;

    //recalc the user bounds
    user_bounds.reset();

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
            user_bounds.update(u->getPos());

            //if nothing is selected, and this user is active and this user is the specified user to follow, select them
            if(selectedUser == 0 && selectedFile == 0) {
                for(std::vector<std::string>::iterator ui = follow_users.begin(); ui != follow_users.end(); ui++) {
                    std::string follow = *ui;

                    if(follow.size() && u->getName() == follow) {
                        selectUser(u);
                    }
                }
            }
        }
    }

    //nothing is moving so increment idle
    if(idle_users==users.size()) {
        idle_time += dt;

        //if stop_on_idle is set and either no stop condition is set or the condition has been reached, exit
        if(stop_on_idle && (stop_position == 0.0f && !stop_at_end || stop_position_reached)) appFinished = true;

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
        max_depth = 6;
    }

    dirNodeTree = new QuadTree(quadtreebounds, max_depth, 1);

    //apply forces with other directories
    for(std::map<std::string,RDirNode*>::iterator it = gGourceDirMap.begin(); it!=gGourceDirMap.end(); it++) {
        RDirNode* node = it->second;

        if(!node->empty()) {
            node->updateQuadItemBounds();
            dirNodeTree->addItem(node);
        }
    }

    update_dir_tree_time = SDL_GetTicks() - update_dir_tree_time;

    root->applyForces(*dirNodeTree);
}

void Gource::updateDirs(float dt) {
    //recalc the directory bounds
    dir_bounds.reset();

    root->logic(dt, dir_bounds);
}

void Gource::updateTime() {
    //display date
    char datestr[256];
    char timestr[256];
    struct tm* timeinfo = localtime ( &currtime );

    strftime(datestr, 256, gGourceDateFormat.c_str(), timeinfo);
    displaydate = datestr;

    //avoid wobbling by only moving font if change is sufficient
    int date_offset = (int) fontmedium.getWidth(displaydate) * 0.5;
    if(abs(date_x_offset - date_offset) > 5) date_x_offset = date_offset;
}

void Gource::updateCamera(float dt) {

    //camera tracking

    if(backgroundSelected) {
        Bounds2D mousebounds;
        mousebounds.update(backgroundPos);

        camera.adjust(mousebounds);

    } else if(track_users && (selectedFile !=0 || selectedUser !=0)) {
        Bounds2D focusbounds;

        vec3f camerapos = camera.getPos();

        if(selectedUser !=0) focusbounds.update(selectedUser->getPos());
        if(selectedFile !=0) focusbounds.update(selectedFile->getAbsolutePos());

        camera.adjust(focusbounds);
    } else {
        if(track_users && idle_time==0) camera.adjust(user_bounds);
        else camera.adjust(dir_bounds);
    }

    camera.logic(dt);
}

void Gource::logic(float t, float dt) {

    if(draw_loading) return;

    if(splash>0.0) splash -= dt;

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
                throw SDLAppException("", true);
            } else if(SDLAppDirExists(logfile)) {
                throw SDLAppException("directory not supported");
            } else {
                throw SDLAppException("unsupported log format (you may need to regenerate your log file)");
            }
        }

        if(start_position>0.0) {
            seekTo(start_position);
        }
    }

    slider.logic(dt);

    //still want to update camera while paused
    if(paused) {
        updateCamera(dt);
        return;
    }

    // get more entries
    if(commitqueue.size() == 0) {
        readLog();
    }

    //loop in attempt to find commits
    if(commitqueue.size()==0 && commitlog->isSeekable() && gGourceFileLoop) {
        first_read=true;
        seekTo(0.0);
        readLog();
    }

    if(currtime==0 && commitqueue.size()) {
        currtime   = commitqueue[0].timestamp;
        subseconds = 0.0;
    }

    //set current time
    float time_inc = (dt * 86400.0 * gGourceDaysPerSecond);
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

        if(gGourceAutoSkipSeconds>=0.0 && idle_time >= gGourceAutoSkipSeconds) {
            currtime = commit.timestamp;
            idle_time = 0.0;
        }

        if(commit.timestamp > currtime) break;

        processCommit(commit, t);

        currtime = commit.timestamp;
        subseconds = 0.0;

        commitqueue.pop_front();
    }

    //reset loop counters
    gGourceUserInnerLoops = 0;
    gGourceDirNodeInnerLoops = 0;
    gGourceFileInnerLoops = 0;

    interactUsers();
    interactDirs();

    updateUsers(t, dt);
    updateDirs(dt);

    updateCamera(dt);

    updateTime();
}

void Gource::mousetrace(Frustum& frustum, float dt) {
    GLuint	buffer[512];
	GLint	viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    (void) glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPickMatrix((GLdouble) mousepos.x, (GLdouble) (viewport[3]-mousepos.y), 1.0f, 1.0f, viewport);
    gluPerspective(90.0f, (GLfloat)display.width/(GLfloat)display.height, 0.1f, camera.getZFar());
    camera.look();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        it->second->drawSimple(dt);
    }

    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    root->drawSimple(frustum, dt);

    glMatrixMode(GL_MODELVIEW);

    mouse_hits = glRenderMode(GL_RENDER);

    RFile* fileSelection = 0;
    RUser* userSelection = 0;

    if (mouse_hits > 0) {
        int choice   = buffer[3];
        GLuint depth = buffer[1];

        for (int loop = 1; loop < mouse_hits; loop++) {
            if (buffer[loop*4+1] < depth) {
                choice = buffer[loop*4+3];
                depth  = buffer[loop*4+1];
            }
        }

        if(choice != 0) {
            selectionDepth = depth;

            std::map<int, RFile*>::iterator filetest;
            std::map<int, RUser*>::iterator usertest;

            if((filetest = tagfilemap.find(choice)) != tagfilemap.end()) {
                fileSelection = filetest->second;
            }
            else if((usertest = tagusermap.find(choice)) != tagusermap.end()) {
                userSelection = usertest->second;
            }
		}
    }

    glDisable(GL_DEPTH_TEST);

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
        if(hoverUser!=0) selectUser(hoverUser);
        else if(hoverFile!=0) selectFile(hoverFile);
        else {
            selectBackground();
        }
    }
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

    display.setClearColour(background_colour);
    display.clear();
}

void Gource::drawTree(Frustum& frustum, float dt) {
    draw_tree_time = SDL_GetTicks();

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

    if(!gGourceHideTree) {
        glBindTexture(GL_TEXTURE_2D, beamtex->textureid);

        root->drawEdgeShadows(dt);
        root->drawEdges(dt);
    }

    //switch back
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //draw shadows

    if(!gGourceHideUsers) {
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            it->second->drawShadow(dt);
        }
    }

    if(!gGourceHideFiles) {
        root->drawShadows(frustum, dt);
    }

    drawActions(dt);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(!trace_debug) {
        if(!gGourceHideFiles) {
            root->drawFiles(frustum,dt);
        }
    } else {
        root->drawSimple(frustum,dt);
    }

    draw_tree_time = SDL_GetTicks() - draw_tree_time;
}

void Gource::drawActions(float dt) {
    if(gGourceHideUsers) return;

    glBindTexture(GL_TEXTURE_2D, beamtex->textureid);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //draw actions
    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        it->second->drawActions(dt);
    }
}

void Gource::drawBloom(Frustum &frustum, float dt) {
    if(gGourceDisableBloom) return;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //draw 'gourceian blur' around dirnodes
    glBindTexture(GL_TEXTURE_2D, bloomtex->textureid);
    glBlendFunc (GL_ONE, GL_ONE);
    root->drawBloom(frustum, dt);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    mousetrace(frustum,dt);

    trace_time = SDL_GetTicks() - trace_time;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    camera.focus();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //draw tree
    drawTree(frustum, dt);

    glColor4f(1.0, 1.0, 0.0, 1.0);
    for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
        trace_debug ? it->second->drawSimple(dt) : it->second->draw(dt);
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    //draw bloom
    drawBloom(frustum, dt);

    root->drawNames(font,frustum);

    if(!(gGourceHideUsernames || gGourceHideUsers)) {
        for(std::map<std::string,RUser*>::iterator it = users.begin(); it!=users.end(); it++) {
            it->second->drawName();
        }
    }

    //draw selected item names again so they are over the top
    if(selectedUser !=0) selectedUser->drawName();

    if(selectedFile !=0) {
        vec2f dirpos = selectedFile->getDir()->getPos();

        glPushMatrix();
            glTranslatef(dirpos.x, dirpos.y, 0.0);
            selectedFile->drawName();
        glPopMatrix();
    }

    if(debug) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        track_users ? user_bounds.draw() : dir_bounds.draw();
    }

    if(gGourceQuadTreeDebug) {
        glDisable(GL_TEXTURE_2D);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

        glLineWidth(1.0);
        dirNodeTree->outline();

        glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

        userTree->outline();
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    display.mode2D();

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    vec3f campos = camera.getPos();

    if(!gGourceHideDate) {
        fontmedium.draw(display.width/2 - date_x_offset, 20, displaydate);
    }

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

    if(debug) {
        font.print(0,20, "FPS: %.2f", fps);
        font.print(0,40,"Time Scale: %.2f", time_scale);
        font.print(0,60,"Users: %d", users.size());
        font.print(0,80,"Files: %d", files.size());
        font.print(0,100,"Dirs: %d",  gGourceDirMap.size());

        font.print(0,120,"Log Position: %.2f", commitlog->getPercent());
        font.print(0,140,"Camera: (%.2f, %.2f, %.2f)", campos.x, campos.y, campos.z);
        font.print(0,160,"Gravity: %.2f", gGourceForceGravity);
        font.print(0,180,"Update Tree: %u ms", update_dir_tree_time);
        font.print(0,200,"Draw Tree: %u ms", draw_tree_time);
        font.print(0,220,"Mouse Trace: %u ms", trace_time);
        font.print(0,240,"Logic Time: %u ms", logic_time);
        font.print(0,260,"Draw Time: %u ms", SDL_GetTicks() - draw_time);
        font.print(0,280,"File Inner Loops: %d", gGourceFileInnerLoops);
        font.print(0,300,"User Inner Loops: %d", gGourceUserInnerLoops);
        font.print(0,320,"Dir Inner Loops: %d (QTree items = %d, nodes = %d)", gGourceDirNodeInnerLoops,
            dirNodeTree->item_count, dirNodeTree->node_count);

        if(selectedUser != 0) {

        }

        if(selectedFile != 0) {
            font.print(0,360,"%s: %d files (%d visible)", selectedFile->getDir()->getPath().c_str(),
                    selectedFile->getDir()->fileCount(), selectedFile->getDir()->visibleFileCount());
        }

    }

    glDisable(GL_TEXTURE_2D);

    if(canSeek()) slider.draw(dt);

    mousemoved=false;
    mouseclicked=false;
}

