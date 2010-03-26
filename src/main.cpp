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
        } else {
            conf.setEntry("gource", "path", ".");
        }

        //apply the config / see if its valid
        gGourceSettings.importDisplaySettings(conf);
        gGourceSettings.importGourceSettings(conf);

        //save config
        if(gGourceSettings.save_config.size() > 0) {
            conf.save(gGourceSettings.save_config);
            exit(0);
        }

    } catch(ConfFileException& exception) {

        SDLAppQuit(exception.what());
    }

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
    GourceDemo* gource_demo = 0;

    try {

        if(gGourceSettings.demo) {

            gource_demo = new GourceDemo(&conf, exporter);
            gource_demo->run();

        } else {

            gource = new Gource(exporter);
            gource->run();

        }

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

    if(gource!=0)      delete gource;
    if(gource_demo!=0) delete gource_demo;
    if(exporter != 0)  delete exporter;

    //free resources
    display.quit();

    return 0;
}
