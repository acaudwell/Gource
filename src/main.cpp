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

#include "main.h"

//hack to stop SDL redirecting stdout/stderr to txt files on windows
#ifdef _WIN32
#undef main
#endif

int main(int argc, char *argv[]) {

    int width  = 1024;
    int height = 768;
    bool fullscreen=false;
    bool multisample=false;
    vec3f background = vec3f(0.25, 0.25, 0.25);

    std::vector<std::string> follow_users;
    std::vector<std::string> highlight_users;
    std::vector<Regex*> filters;

    float start_position = 0.0;

    std::string logfile = ".";

    std::vector<std::string> arguments;

    display.displayArgs(argc, argv, &width, &height, &fullscreen, &arguments);

    for(int i=0;i<arguments.size();i++) {
        std::string args = arguments[i];

        if(args == "-h" || args == "-?" || args == "--help") {
            gource_help("");
        }

        if(args == "--loop") {
            gGourceFileLoop = true;
            continue;
        }

        if(args == "--git-branch") {

            if((i+1)>=arguments.size() || arguments[i+1].size() == 0) {
                gource_help("specify a git branch");
            }

            gGourceGitLogCommand += " ";
            gGourceGitLogCommand += arguments[++i];

            continue;
        }

        if(args == "--git-log-command") {
            printf("%s\n", gGourceGitLogCommand.c_str());
            exit(0);
        }

        if(args == "--cvs-exp-command") {
            printf("%s\n", gGourceCvsExpLogCommand.c_str());
            exit(0);
        }

        if(args == "--hide-date") {
            gGourceHideDate = true;
            continue;
        }

        if(args == "--disable-auto-skip") {
            gGourceAutoSkipSeconds = -1.0;
            continue;
        }

        if(args == "--disable-progress") {
            gGourceDisableProgress = true;
            continue;
        }

        if(args == "--hide-users") {
            gGourceHideUsers = true;
            continue;
        }

        if(args == "--hide-usernames") {
            gGourceHideUsernames = true;
            continue;
        }

        if(args == "--hide-filenames") {
            gGourceHideFilenames = true;
            continue;
        }

        if(args == "--multi-sampling") {
            multisample = true;
            continue;
        }

        if(args == "--colour-images") {
            gGourceColourUserImages = true;
            continue;
        }

        if(args == "--default-user-image") {

            if((i+1)>=arguments.size() || arguments[i+1].size() == 0) {
                gource_help("specify default-user-image (image path)");
            }

            gGourceDefaultUserImage = arguments[++i];

            continue;
        }

        if(args == "--user-image-dir") {

            if((i+1)>=arguments.size() || arguments[i+1].size() == 0) {
                gource_help("specify user-image-dir (directory)");
            }

            gGourceUserImageDir = arguments[++i];

            //append slash
            if(gGourceUserImageDir[gGourceUserImageDir.size()-1] != '/') {
                gGourceUserImageDir += std::string("/");
            }

            continue;
        }

        if(args == "-e" || args == "--elasticity") {

            if((i+1)>=arguments.size()) {
                gource_help("specify elasticity (float)");
            }

            gGourceElasticity = atof(arguments[++i].c_str());

            if(gGourceElasticity<=0.0) {
                gource_help("invalid elasticity value");
            }

            continue;
        }

        if(args == "-b" || args == "--background") {

            if((i+1)>=arguments.size()) {
                gource_help("specify background colour (#FFFFFF)");
            }

            int r,g,b;
            std::string colstring = arguments[++i];

            if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
                background = vec3f(r,g,b);
                background /= 255.0f;
            } else {
                gource_help("invalid colour string");
            }

            continue;
        }

        if(args == "-s" || args == "--seconds-per-day") {

            if((i+1)>=arguments.size()) {
                gource_help("specify seconds-per-day (seconds)");
            }

            gGourceDaysPerSecond = atof(arguments[++i].c_str());

            if(gGourceDaysPerSecond<=0.0) {
                gource_help("invalid seconds-per-day value");
            }

            gGourceDaysPerSecond = 1.0 / gGourceDaysPerSecond;

            continue;
        }

        if(args == "-a" || args == "--auto-skip-seconds") {

            if((i+1)>=arguments.size()) {
                gource_help("specify auto-skip-seconds");
            }

            gGourceAutoSkipSeconds = (float) atof(arguments[++i].c_str());

            if(gGourceAutoSkipSeconds <= 0.0) {
                gource_help("invalid auto-skip-seconds value");
            }

            continue;
        }

        if(args == "-i" || args == "--file-idle-time") {

            if((i+1)>=arguments.size()) {
                gource_help("specify file-idle-time (seconds)");
            }

            gGourceMaxFileIdle = (float) atoi(arguments[++i].c_str());

            if(gGourceMaxFileIdle<1.0) {
                gource_help("invalid file-idle-time value");
            }

            continue;
        }

        if(args == "-p" || args == "--start-position") {

            if((i+1)>=arguments.size()) {
                gource_help("specify start-position (float)");
            }

            start_position = atof(arguments[++i].c_str());

            if(start_position<=0.0 || start_position>=1.0) {
                gource_help("start-position outside of range 0.0 - 1.0 (non-inclusive)");
            }

            continue;
        }

        if(args == "--max-files") {

            if((i+1)>=arguments.size()) {
                gource_help("specify max-files (number)");
            }

            gGourceMaxFiles = atoi(arguments[++i].c_str());

            if(gGourceMaxFiles<1) {
                gource_help("invalid max-files value");
            }

            continue;
        }

        if(args == "--highlight-all-users") {

            gGourceHighlightAllUsers = true;

            continue;
        }

        if(args == "--highlight-user") {

            if((i+1)>=arguments.size()) {
                gource_help("specify highlight-user (user)");
            }

            highlight_users.push_back(arguments[++i]);

            continue;
        }

        if(args == "--follow-user") {

            if((i+1)>=arguments.size()) {
                gource_help("specify follow-user (user)");
            }

            follow_users.push_back(arguments[++i]);

            continue;
        }

        if(args == "--file-filter") {

            if((i+1)>=arguments.size()) {
                gource_help("specify file-filter (regex)");
            }

            std::string filter_string = arguments[++i];

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                gource_help("invalid filt-filter regular expression");
            }

            filters.push_back(r);

            continue;
        }

        // assume this is the log file
        if(args == "-" || args.size() > 1 && args[0] != '-') {
            logfile = args;
            continue;
        }

        // unknown argument
        std::string arg_error = std::string("unknown option ") + std::string(args);

        gource_help(arg_error);
    }

    // wait for a character on the file handle if reading stdin
    if(logfile == "-") {
        while(std::cin.peek() == EOF && !std::cin.fail()) SDL_Delay(100);
    }


    // this causes corruption on some video drivers
    if(multisample) {
        display.multiSample(4);
    }

    //enable vsync
    display.enableVsync(true);

    display.init("Gource", width, height, fullscreen);

    if(multisample) glEnable(GL_MULTISAMPLE_ARB);

    Gource* gource = new Gource(logfile);

    if(start_position>0.0) gource->setStartPosition(start_position);

    gource->setBackground(background);

    for(std::vector<std::string>::iterator it = follow_users.begin(); it != follow_users.end(); it++) {
        gource->addFollowUser(*it);
    }

    for(std::vector<std::string>::iterator it = highlight_users.begin(); it != highlight_users.end(); it++) {
        gource->addHighlightUser(*it);
    }

    for(std::vector<Regex*>::iterator it = filters.begin(); it != filters.end(); it++) {
        gource->addFilter(*it);
    }

    gource->run();

    delete gource;

    //free resources
    display.quit();

    return 0;
}
