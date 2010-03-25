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

#include "settings.h"

Regex SDLAppSettings_rect_regex("^([0-9.]+)x([0-9.]+)$");

SDLAppSettings::SDLAppSettings() {
    setDisplayDefaults();

    default_section_name = "settings";

    //conf entries in other sections
    conf_sections["viewport"]           = "display";
    conf_sections["windowed"]           = "display";
    conf_sections["fullscreen"]         = "display";
    conf_sections["multi-sampling"]     = "display";
    conf_sections["output-ppm-stream"]  = "display";
    conf_sections["output-framerate"]   = "display";

    //translate args
    arg_aliases["f"] = "fullscreen";
    arg_aliases["w"] = "windowed";

    //boolean args
    arg_types["fullscreen"]     = "bool";
    arg_types["windowed"]       = "bool";
    arg_types["multi-sampling"] = "bool";
    arg_types["viewport"]       = "string";
}

void SDLAppSettings::setDisplayDefaults() {
    display_width  = 1024;
    display_height = 768;
    fullscreen     = false;
    multisample    = false;

    output_ppm_filename = "";
    output_framerate    = 60;
}

void SDLAppSettings::exportDisplaySettings(ConfFile& conf) {

    ConfSection* section = new ConfSection("display");

    char viewportbuff[256];
    snprintf(viewportbuff, 256, "%dx%d", display_width, display_height);

    std::string viewport = std::string(viewportbuff);

    section->setEntry(new ConfEntry("viewport", viewport));

    if(fullscreen)
        section->setEntry(new ConfEntry("fullscreen", fullscreen));

    if(multisample)
        section->setEntry(new ConfEntry("multi-sampling", multisample));

    conf.setSection(section);
}

bool SDLAppSettings::parseRectangle(const std::string& value, int* x, int* y) {

    std::vector<std::string> matches;

    if(SDLAppSettings_rect_regex.match(value, &matches)) {
        if(x!=0) *x = atoi(matches[0].c_str());
        if(y!=0) *y = atoi(matches[1].c_str());
        return true;
    }

    return false;
}

void SDLAppSettings::parseArgs(int argc, char *argv[], ConfFile& conffile, std::vector<std::string>* files) {

    std::vector<std::string> arguments;

    for (int i=1; i<argc; i++) {
        arguments.push_back(argv[i]);
    }

    parseArgs(arguments, conffile, files);
}

//apply args to a conf file
void SDLAppSettings::parseArgs(const std::vector<std::string>& arguments, ConfFile& conffile, std::vector<std::string>* files) {

    std::map<std::string, std::string>::iterator findit;

    for(int i=0;i<arguments.size();i++) {
        std::string args = arguments[i];

        //remove leading hyphens
        bool is_option = false;

        while(args.size()>1 && args[0] == '-') {
            args = args.substr(1, args.size()-1);
            is_option = true;
        }

        if(args.size()==0) continue;

        if(!is_option) {
            if(files!=0) {
                files->push_back(args);
            }
            continue;
        }

        //translate args with aliases
        if((findit = arg_aliases.find(args)) != arg_aliases.end()) {
            args = findit->second;
        }

        //NUMBERxNUMBER is a magic alias for viewport
        if(args.size()>1 && args.rfind("x") != std::string::npos) {

            std::string displayarg = args;

            int width  = 0;
            int height = 0;

            if(parseRectangle(displayarg, &width, &height)) {
                if(width>0 && height>0) {

                    ConfSection* display_settings = conffile.getSection("display");

                    if(!display_settings) {
                        display_settings = new ConfSection("display");
                        conffile.addSection(display_settings);
                    }

                    display_settings->setEntry("viewport", args);
                    continue;
                }
            }
        }

        //get type

        std::string arg_type;
        if((findit = arg_types.find(args)) != arg_types.end()) {
            arg_type = findit->second;
        } else {
            std::string unknown_option = std::string("unknown option ") + args;
            throw ConfFileException(unknown_option, "", 0);
        }

        //get value (or set to true for booleans)

        std::string argvalue;
        if(arg_type == "bool") argvalue = "true";
        else if((i+1)<arguments.size()) argvalue = arguments[++i];

        //determine section
        std::string section_name = default_section_name;
        if((findit = conf_sections.find(args)) != conf_sections.end()) {
            section_name = findit->second;
        }

        //command line options dont go into the conf file
        if(section_name == "command-line") {
            commandLineOption(args, argvalue);
            continue;
        }

        //get section(s) of this type

        ConfSectionList* sections = conffile.getSections(section_name);

        if(sections == 0) {
            ConfSection* newsection = new ConfSection(section_name);
            conffile.addSection(newsection);

            sections = conffile.getSections(section_name);
        }

        //apply to section

        for(ConfSectionList::iterator it = sections->begin(); it != sections->end(); it++) {

            ConfSection* section = *it;

            if(arg_type == "multi-value") {
                section->addEntry(args, argvalue);
            } else {
                section->setEntry(args, argvalue);
            }
        }
    }
}

void SDLAppSettings::importDisplaySettings(ConfFile& conffile) {

    setDisplayDefaults();

    ConfSection* display_settings = conffile.getSection("display");

    if(display_settings == 0) return;

    ConfEntry* entry = 0;

    if((entry = display_settings->getEntry("viewport")) != 0) {

        std::string viewport = entry->getString();

        int width  = 0;
        int height = 0;

        size_t x = viewport.rfind("x");

        if(x != std::string::npos && x != 0 && x != viewport.size()-1) {
            std::string widthstr  = viewport.substr(0, x);
            std::string heightstr = viewport.substr(x+1);

            width  = atoi(widthstr.c_str());
            height = atoi(heightstr.c_str());
        }

        if(width>0 && height>0) {
            display_width  = width;
            display_height = height;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if(display_settings->getBool("multi-sampling")) {
        multisample = true;
    }

    if(display_settings->getBool("fullscreen")) {
        fullscreen = true;
    }

    if(display_settings->getBool("windowed")) {
        fullscreen = false;
    }

    if((entry = display_settings->getEntry("output-ppm-stream")) != 0) {

        if(!entry->hasValue()) {
            conffile.entryException(entry, "specify ppm output file or '-' for stdout");
        }

        output_ppm_filename = entry->getString();

#ifdef _WIN32
        if(output_ppm_filename == "-") {
            conffile.entryException(entry, "stdout PPM mode not supported on Windows");
        }
#endif
    }

    if((entry = display_settings->getEntry("output-framerate")) != 0) {

        if(!entry->hasValue()) {
             conffile.entryException(entry, "specify framerate (25,30,60)");
        }

        output_framerate = entry->getInt();

        if(   output_framerate != 25
            && output_framerate != 30
            && output_framerate != 60) {
            conffile.entryException(entry, "supported framerates are 25,30,60");
        }
    }
}
