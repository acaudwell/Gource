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

int main(int argc, char *argv[]) {

    SDLAppInit("Gource", "gource");

    ConfFile conf;
    std::vector<std::string> files;

    //convert args to a conf file
    //read the conf file
    //apply the conf file to settings

    try {
        gGourceSettings.parseArgs(argc, argv, conf, &files);

        //load config
        if(gGourceSettings.load_config.size() > 0) {
            conf.clear();
            conf.load(gGourceSettings.load_config);

            //apply args to loaded conf file
            gGourceSettings.parseArgs(argc, argv, conf);
        }

        //set path
        if(files.size()>0) {
            std::string path = files[files.size()-1];

            ConfSectionList* sectionlist = conf.getSections("gource");

            if(sectionlist!=0) {
                for(ConfSectionList::iterator sit = sectionlist->begin(); sit != sectionlist->end(); sit++) {
                    (*sit)->setEntry("path", path);
                }
            } else {
                conf.setEntry("gource", "path", path);
            }
        }

        //save config
        if(gGourceSettings.save_config.size() > 0) {
            conf.save(gGourceSettings.save_config);
            exit(0);
        }

        gGourceSettings.setDisplaySettings(conf);
        gGourceSettings.setGourceSettings(conf);

    } catch(ConfFileException& exception) {

        SDLAppQuit(exception.what());
    }

    std::string path = conf.getString("gource", "path");
    if(path.size()==0) path = ".";

    //validate path
    if(path == "-") {

        if(gGourceSettings.log_format.size() == 0) {
            SDLAppQuit("log-format required when reading from STDIN");
        }

        while(std::cin.peek() == EOF && !std::cin.fail()) SDL_Delay(100);
        std::cin.clear();
    }

    //remove trailing slash and check if path is a directory
    if(path.size() &&
    (path[path.size()-1] == '\\' || path[path.size()-1] == '/')) {
        path = path.substr(0,path.size()-1);
    }

#ifdef _WIN32
    //on windows, pre-open console window if we think this is a directory the
    //user is trying to open, as system() commands will create a console window
    //if there isn't one anyway.

    bool isdir = false;

    if(path.size()>0) {
        struct stat fileinfo;
        int rc = stat(path.c_str(), &fileinfo);

        if(rc==0 && fileinfo.st_mode & S_IFDIR) isdir = true;
    }

    if(path.size()==0 || isdir) {
        SDLAppCreateWindowsConsole();
    }
#endif

    // this causes corruption on some video drivers
    if(gGourceSettings.multisample) {
        display.multiSample(4);
    }

    //enable vsync
    display.enableVsync(true);

    try {

        display.init("Gource", gGourceSettings.display_width, gGourceSettings.display_height, gGourceSettings.fullscreen);

    } catch(SDLInitException& exception) {

        char errormsg[1024];
        snprintf(errormsg, 1024, "SDL initialization failed - %s", exception.what());

        SDLAppQuit(errormsg);
    }

    //init frame exporter
    FrameExporter* exporter = 0;

    if(gGourceSettings.output_ppm_filename.size() > 0) {
        //recording a video kind of implies you want this
        if(!gGourceSettings.dont_stop) gGourceSettings.stop_at_end = true;

        try {

            exporter = new PPMExporter(gGourceSettings.output_ppm_filename);

        } catch(PPMExporterException& exception) {

            char errormsg[1024];
            snprintf(errormsg, 1024, "could not write to '%s'", exception.what());

            SDLAppQuit(errormsg);
        }
    }

    if(gGourceSettings.multisample) glEnable(GL_MULTISAMPLE_ARB);

    Gource* gource = 0;

    try {
        gource = new Gource(path);

        if(exporter!=0) gource->setFrameExporter(exporter, gGourceSettings.output_framerate);

//         for(std::vector<std::string>::iterator it = settings.follow_users.begin(); it != settings.follow_users.end(); it++) {
//             gource->addFollowUser(*it);
//         }
// 
//         for(std::vector<std::string>::iterator it = settings.highlight_users.begin(); it != settings.highlight_users.end(); it++) {
//             gource->addHighlightUser(*it);
//         }
// 
//         for(std::vector<Regex*>::iterator it = settings.filters.begin(); it != settings.filters.end(); it++) {
//             gource->addFilter(*it);
//         }

        gource->run();

    } catch(ResourceException& exception) {

        char errormsg[1024];
        snprintf(errormsg, 1024, "failed to load resource '%s'", exception.what());

        SDLAppQuit(errormsg);

    } catch(SDLAppException& exception) {

        if(exception.showHelp()) {
            gource_help();
        } else {
            SDLAppQuit(exception.what());
        }

    }

    if(gource!=0) delete gource;

    if(exporter != 0) delete exporter;

    //free resources
    display.quit();

    return 0;
}
