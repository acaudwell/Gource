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

// look for a .gource in this order:
// * if a directory specified and there is one in that directory.
// * if the user has a .gource in their home directory.


std::string find_gource_rc(const std::string& file_path) {
   
    std::vector<std::string> rc_search_dirs;

    if(!file_path.empty()) rc_search_dirs.push_back(file_path);
    else rc_search_dirs.push_back(".");
    
    char* home = getenv("HOME");
    if(home!=0) rc_search_dirs.push_back(std::string(home));
    
    // TODO: check some other env var on windows ?

    std::string gource_rc_path;
    
    for(const std::string& dir : rc_search_dirs) {
        debugLog("checking %s", dir.c_str());
        boost::filesystem::path path(dir);
        
        if(boost::filesystem::is_directory(path)) {
            path /= ".gource";

            if(boost::filesystem::exists(path)) {
                gource_rc_path = path.string();
                break;
            }
        }
    }
    
    return gource_rc_path;
}

int main(int argc, char *argv[]) {

    SDLAppInit("Gource", "gource");

    ConfFile conf;
    std::vector<std::string> files;

    //convert args to a conf file
    //read the conf file
    //apply the conf file to settings

    try {
        gGourceSettings.parseArgs(argc, argv, conf, &files);

        if(gGourceSettings.load_config.empty() && !files.empty()) {

            //see if file looks like a config file
            for(std::vector<std::string>::iterator fit = files.begin(); fit != files.end(); fit++) {

                std::string file = *fit;

                int file_length = file.size();

                if(   (file.rfind(".conf")  == (file_length-5))
                   || (file.rfind(".cfg")   == (file_length-4))
                   || (file.rfind(".gourc") == (file_length-5))
                   || (file.rfind(".ini")   == (file_length-4)) ) {

                    bool is_conf=true;

                    try {
                        ConfFile conftest;
                        conftest.load(file);
                    } catch(ConfFileException& exception) {
                        is_conf = false;
                    }

                    if(is_conf) {
                        gGourceSettings.load_config = file;
                        files.erase(fit);
                        break;
                    }
                }
            }
        }       
        
        //set log level
        Logger::getDefault()->setLevel(gGourceSettings.log_level);

        std::string gource_path;
        
        if(!files.empty()) {
            gource_path = files[files.size()-1];
        }
        
        // see if there is a .gourc file to use
        if(gGourceSettings.load_config.empty()) {
            gGourceSettings.load_config = find_gource_rc(gource_path);
        }
        
        //load config
        if(!gGourceSettings.load_config.empty()) {
            conf.clear();
            conf.load(gGourceSettings.load_config);

            //apply args to loaded conf file
            gGourceSettings.parseArgs(argc, argv, conf);
        }

        //set path
        if(!gource_path.empty()) {
            ConfSectionList* sectionlist = conf.getSections("gource");

            if(sectionlist!=0) {
                for(ConfSectionList::iterator sit = sectionlist->begin(); sit != sectionlist->end(); sit++) {
                    (*sit)->setEntry("path", gource_path);
                }
            } else {
                conf.setEntry("gource", "path", gource_path);
            }
        }

        //apply the config / see if its valid
        gGourceSettings.importDisplaySettings(conf);
        gGourceSettings.importGourceSettings(conf);

        //save config
        if(!gGourceSettings.save_config.empty()) {
            conf.save(gGourceSettings.save_config);
            exit(0);
        }

        //write custom log file
        if(gGourceSettings.output_custom_filename.size() > 0 && gGourceSettings.path.size() > 0) {

            Gource::writeCustomLog(gGourceSettings.path, gGourceSettings.output_custom_filename);
            exit(0);
        }

    } catch(ConfFileException& exception) {

        SDLAppQuit(exception.what());
    }

    // this causes corruption on some video drivers
    if(gGourceSettings.multisample) {
        display.multiSample(4);
    }

    //background needs alpha channel
    if(gGourceSettings.transparent) {
        display.enableAlpha(true);
    }

    //enable vsync
    display.enableVsync(gGourceSettings.vsync);

    //allow resizing window if we are not recording
    if(gGourceSettings.resizable && gGourceSettings.output_ppm_filename.empty()) {
        display.enableResize(true);
    }

    try {

        display.init("Gource", gGourceSettings.display_width, gGourceSettings.display_height, gGourceSettings.fullscreen);

    } catch(SDLInitException& exception) {

        char errormsg[1024];
        snprintf(errormsg, 1024, "SDL initialization failed - %s", exception.what());

        SDLAppQuit(errormsg);
    }

#ifdef _WIN32
    SDLApp::attachConsole();
#endif

    //init frame exporter
    FrameExporter* exporter = 0;

    if(gGourceSettings.output_ppm_filename.size() > 0) {

        try {

            exporter = new PPMExporter(gGourceSettings.output_ppm_filename);

        } catch(PPMExporterException& exception) {

            char errormsg[1024];
            snprintf(errormsg, 1024, "could not write to '%s'", exception.what());

            SDLAppQuit(errormsg);
        }
    }

    if(display.multiSamplingEnabled()) {
        glEnable(GL_MULTISAMPLE_ARB);
    }

    GourceShell* gourcesh = 0;

    try {
        gourcesh = gGourceShell = new GourceShell(&conf, exporter);
        gourcesh->run();

    } catch(ResourceException& exception) {

        char errormsg[1024];
        snprintf(errormsg, 1024, "failed to load resource '%s'", exception.what());

        SDLAppQuit(errormsg);

    } catch(SDLAppException& exception) {

        if(exception.showHelp()) {
            gGourceSettings.help();
        } else {
            SDLAppQuit(exception.what());
        }
    }

    gGourceShell = 0;

    if(gourcesh != 0) delete gourcesh;
    if(exporter != 0) delete exporter;

    //free resources
    display.quit();

    return 0;
}
