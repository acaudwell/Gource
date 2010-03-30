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

#ifndef SDLAPP_SETTINGS_H
#define SDLAPP_SETTINGS_H

#include "regex.h"

#include "conffile.h"

class SDLAppSettings {
protected:
    std::string default_section_name;

    std::map<std::string, std::string> arg_types;
    std::map<std::string, std::string> arg_aliases;
    std::map<std::string, std::string> conf_sections;

    virtual void commandLineOption(const std::string& name, const std::string& value) {}

    bool parseRectangle(const std::string& value, int* x=0, int* y=0);
public:
    int display_width;
    int display_height;
    bool multisample;
    bool fullscreen;
    bool transparent;

    std::string output_ppm_filename;
    int output_framerate;

    SDLAppSettings();

    void parseArgs(int argc, char *argv[], ConfFile& conffile, std::vector<std::string>* files = 0);
    void parseArgs(const std::vector<std::string>& args, ConfFile& conffile, std::vector<std::string>* files = 0);

    void exportDisplaySettings(ConfFile& conf);
    void importDisplaySettings(ConfFile& conf);

    void setDisplayDefaults();
};

#endif
