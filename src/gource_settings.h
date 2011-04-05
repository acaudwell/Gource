/*
    Copyright (C) 2010 Andrew Caudwell (acaudwell@gmail.com)

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

#ifndef GOURCE_SETTINGS_H
#define GOURCE_SETTINGS_H

#define GOURCE_VERSION "0.33"

#include <dirent.h>

#include "hg.h"
#include "git.h"
#include "bzr.h"
#include "cvs-exp.h"
#include "cvs2cl.h"
#include "svn.h"

#include "core/settings.h"
#include "core/regex.h"

class GourceSettings : public SDLAppSettings {
protected:
    void commandLineOption(const std::string& name, const std::string& value);
public:
    int repo_count;

    bool hide_date;
    bool hide_users;
    bool hide_tree;
    bool hide_files;
    bool hide_usernames;
    bool hide_filenames;
    bool hide_dirnames;
    bool hide_progress;
    bool hide_bloom;
    bool hide_mouse;
    bool hide_root;

    bool disable_auto_rotate;

    bool show_key;
    
    std::string load_config;
    std::string save_config;
    std::string path;

    std::string logo;
    vec2f logo_offset;

    float start_position;
    float stop_position;
    float stop_at_time;

    bool stop_on_idle;
    bool stop_at_end;
    bool dont_stop;

    float auto_skip_seconds;
    float days_per_second;
    float file_idle_time;

    bool loop;

    bool colour_user_images;
    std::string default_user_image;
    std::string user_image_dir;
    std::map<std::string, std::string> user_image_map;

    std::string camera_mode;
    float padding;

    bool crop_vertical;
    bool crop_horizontal;

    float bloom_multiplier;
    float bloom_intensity;

    vec3f background_colour;
    std::string background_image;

    std::string title;

    int font_size;
    vec3f font_colour;

    float elasticity;

    std::string git_branch;

    std::string log_format;
    std::string date_format;

    int max_files;
    float max_user_speed;
    float max_file_lag;

    float user_idle_time;
    float user_friction;
    float user_scale;
    float time_scale;

    bool highlight_dirs;
    bool highlight_all_users;
    vec3f highlight_colour;
   
    std::vector<std::string> highlight_users;
    std::vector<std::string> follow_users;
    std::vector<Regex*> file_filters;
    std::vector<Regex*> user_filters;
    bool file_extensions;
    
    std::string output_custom_filename;

    TextureResource* file_graphic;

    GourceSettings();

    void setGourceDefaults();

    void importGourceSettings(ConfFile& conf, ConfSection* gource_settings = 0);

    void help(bool extended_help=false);
};

extern GourceSettings gGourceSettings;

#endif
