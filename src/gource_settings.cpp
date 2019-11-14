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

#include "gource_settings.h"
#include "core/sdlapp.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "core/utf8/utf8.h"
#include <time.h>

#include "formats/hg.h"
#include "formats/git.h"
#include "formats/bzr.h"
#include "formats/cvs-exp.h"
#include "formats/cvs2cl.h"
#include "formats/svn.h"

#ifndef GOURCE_FONT_FILE
#define GOURCE_FONT_FILE "FreeSans.ttf"
#endif

GourceSettings gGourceSettings;

//display help message
void GourceSettings::help(bool extended_help) {

#ifdef _WIN32
    //resize window to fit help message
    SDLApp::resizeConsole(1000);
    SDLApp::showConsole(true);
#endif

    printf("Gource v%s\n", GOURCE_VERSION);

    printf("Usage: gource [options] [path]\n");
    printf("\nOptions:\n");
    printf("  -h, --help                       Help\n\n");
    printf("  -WIDTHxHEIGHT, --viewport        Set viewport size\n");
    printf("  -f, --fullscreen                 Fullscreen\n");
    printf("      --screen SCREEN              Screen number\n");
    printf("      --multi-sampling             Enable multi-sampling\n");
    printf("      --no-vsync                   Disable vsync\n\n");

    printf("  --start-date 'YYYY-MM-DD hh:mm:ss +tz'  Start at a date and optional time\n");
    printf("  --stop-date  'YYYY-MM-DD hh:mm:ss +tz'  Stop at a date and optional time\n\n");
    printf("  -p, --start-position POSITION    Start at some position (0.0-1.0 or 'random')\n");
    printf("      --stop-position  POSITION    Stop at some position\n");
    printf("  -t, --stop-at-time SECONDS       Stop after a specified number of seconds\n");
    printf("      --stop-at-end                Stop at end of the log\n");
    printf("      --dont-stop                  Keep running after the end of the log\n");
    printf("      --loop                       Loop at the end of the log\n\n");

    printf("  -a, --auto-skip-seconds SECONDS  Auto skip to next entry if nothing happens\n");
    printf("                                   for a number of seconds (default: 3)\n");
    printf("      --disable-auto-skip          Disable auto skip\n");
    printf("  -s, --seconds-per-day SECONDS    Speed in seconds per day (default: 10)\n");
    printf("      --realtime                   Realtime playback speed\n");
    printf("      --no-time-travel             Use the time of the last commit if the\n");
    printf("                                   time of a commit is in the past\n");
    printf("  -c, --time-scale SCALE           Change simulation time scale (default: 1.0)\n");
    printf("  -e, --elasticity FLOAT           Elasticity of nodes (default: 0.0)\n\n");

    printf("  --key                            Show file extension key\n\n");

    printf("  --user-image-dir DIRECTORY       Dir containing images to use as avatars\n");
    printf("  --default-user-image IMAGE       Default user image file\n");
    printf("  --colour-images                  Colourize user images\n\n");

    printf("  -i, --file-idle-time SECONDS     Time files remain idle (default: 0)\n\n");

    printf("  --max-files NUMBER      Max number of files or 0 for no limit\n");
    printf("  --max-file-lag SECONDS  Max time files of a commit can take to appear\n\n");

    printf("  --log-command VCS       Show the VCS log command (git,svn,hg,bzr,cvs2cl)\n");
    printf("  --log-format  VCS       Specify the log format (git,svn,hg,bzr,cvs2cl,custom)\n\n");

    printf("  --load-config CONF_FILE  Load a config file\n");
    printf("  --save-config CONF_FILE  Save a config file with the current options\n\n");

    printf("  -o, --output-ppm-stream FILE    Output PPM stream to a file ('-' for STDOUT)\n");
    printf("  -r, --output-framerate  FPS     Framerate of output (25,30,60)\n\n");

if(extended_help) {
    printf("Extended Options:\n\n");

    printf("  --window-position XxY    Initial window position\n");
    printf("  --frameless              Frameless window\n\n");

    printf("  --output-custom-log FILE  Output a custom format log file ('-' for STDOUT).\n\n");

    printf("  -b, --background-colour  FFFFFF    Background colour in hex\n");
    printf("      --background-image   IMAGE     Set a background image\n\n");

    printf("  --bloom-multiplier       Adjust the amount of bloom (default: 1.0)\n");
    printf("  --bloom-intensity        Adjust the intensity of the bloom (default: 0.75)\n\n");

    printf("  --camera-mode MODE       Camera mode (overview,track)\n");
    printf("  --crop AXIS              Crop view on an axis (vertical,horizontal)\n");
    printf("  --padding FLOAT          Camera view padding (default: 1.1)\n\n");

    printf("  --disable-auto-rotate    Disable automatic camera rotation\n\n");

    printf("  --disable-input          Disable keyboard and mouse input\n\n");

    printf("  --date-format FORMAT     Specify display date string (strftime format)\n\n");

    printf("  --font-file FILE         Specify the font\n");
    printf("  --font-scale SCALE       Scale the size of all fonts\n");
    printf("  --font-size SIZE         Font size used by date and title\n");
    printf("  --file-font-size SIZE    Font size for filenames\n");
    printf("  --dir-font-size SIZE     Font size for directory names\n");
    printf("  --user-font-size SIZE    Font size for user names\n");
    printf("  --font-colour FFFFFF     Font colour used by date and title in hex\n\n");

    printf("  --file-extensions          Show filename extensions only\n");
    printf("  --file-extension-fallback  Use filename as extension if the extension\n");
    printf("                             is missing or empty\n\n");

    printf("  --git-branch             Get the git log of a particular branch\n\n");

    printf("  --hide DISPLAY_ELEMENT   bloom,date,dirnames,files,filenames,mouse,progress,\n");
    printf("                           root,tree,users,usernames\n\n");

    printf("  --logo IMAGE             Logo to display in the foreground\n");
    printf("  --logo-offset XxY        Offset position of the logo\n\n");

    printf("  --loop-delay-seconds SECONDS Seconds to delay before looping (default: 3)\n\n");

    printf("  --title TITLE            Set a title\n\n");

    printf("  --transparent            Make the background transparent\n\n");

    printf("  --user-filter REGEX      Ignore usernames matching this regex\n");
    printf("  --user-show-filter REGEX Show only usernames matching this regex\n\n");
    printf("  --file-filter REGEX      Ignore file paths matching this regex\n");
    printf("  --file-show-filter REGEX Show only file paths matching this regex\n\n");

    printf("  --user-friction SECONDS  Change the rate users slow down (default: 0.67)\n");
    printf("  --user-scale SCALE       Change scale of users (default: 1.0)\n");
    printf("  --max-user-speed UNITS   Speed users can travel per second (default: 500)\n\n");

    printf("  --follow-user USER       Camera will automatically follow this user\n");
    printf("  --highlight-dirs         Highlight the names of all directories\n");
    printf("  --highlight-user USER    Highlight the names of a particular user\n");
    printf("  --highlight-users        Highlight the names of all users\n\n");

    printf("  --highlight-colour       Font colour for highlighted users in hex.\n");
    printf("  --selection-colour       Font colour for selected users and files.\n");
    printf("  --filename-colour        Font colour for filenames.\n");
    printf("  --dir-colour             Font colour for directories.\n\n");

    printf("  --dir-name-depth DEPTH    Draw names of directories down to a specific depth.\n");
    printf("  --dir-name-position FLOAT Position along edge of the directory name\n");
    printf("                            (between 0.0 and 1.0, default is 0.5).\n\n");

    printf("  --filename-time SECONDS  Duration to keep filenames on screen (default: 4.0)\n\n");

    printf("  --caption-file FILE         Caption file\n");
    printf("  --caption-size SIZE         Caption font size\n");
    printf("  --caption-colour FFFFFF     Caption colour in hex\n");
    printf("  --caption-duration SECONDS  Caption duration (default: 10.0)\n");
    printf("  --caption-offset X          Caption horizontal offset\n\n");

    printf("  --hash-seed SEED         Change the seed of hash function.\n\n");

    printf("  --path PATH\n\n");
}

    printf("PATH may be a supported version control directory, a log file, a gource config\n");
    printf("file, or '-' to read STDIN. If omitted, gource will attempt to generate a log\n");
    printf("from the current directory.\n\n");

    if(!extended_help) {
        printf("To see the full command line options use '-H'\n\n");
    }

#ifdef _WIN32
    if(!SDLApp::existing_console) {
        printf("Press Enter\n");
        getchar();
    }
#endif

    exit(0);
}

GourceSettings::GourceSettings() {
    repo_count = 0;
    file_graphic = 0;
    log_level = LOG_LEVEL_OFF;
    shutdown = false;

    setGourceDefaults();

    default_section_name = "gource";

    //translate args
    arg_aliases["p"] = "start-position";
    arg_aliases["a"] = "auto-skip-seconds";
    arg_aliases["s"] = "seconds-per-day";
    arg_aliases["t"] = "stop-at-time";
    arg_aliases["i"] = "file-idle-time";
    arg_aliases["e"] = "elasticity";
    arg_aliases["h"] = "help";
    arg_aliases["?"] = "help";
    arg_aliases["H"] = "extended-help";
    arg_aliases["b"] = "background-colour";
    arg_aliases["c"] = "time-scale";
    arg_aliases["background"]          = "background-colour";
    arg_aliases["disable-bloom"]       = "hide-bloom";
    arg_aliases["disable-progress"]    = "hide-progress";
    arg_aliases["highlight-all-users"] = "highlight-users";

    //command line only options
    conf_sections["help"]            = "command-line";
    conf_sections["extended-help"]   = "command-line";
    conf_sections["log-command"]     = "command-line";
    conf_sections["git-log-command"] = "command-line";
    conf_sections["cvs-exp-command"] = "command-line";
    conf_sections["cvs2cl-command"]  = "command-line";
    conf_sections["hg-log-command"]  = "command-line";
    conf_sections["bzr-log-command"] = "command-line";
    conf_sections["svn-log-command"] = "command-line";
    conf_sections["load-config"]     = "command-line";
    conf_sections["save-config"]     = "command-line";
    conf_sections["output-custom-log"] = "command-line";
    conf_sections["log-level"]         = "command-line";

    //boolean args
    arg_types["help"]                    = "bool";
    arg_types["extended-help"]           = "bool";
    arg_types["stop-on-idle"]            = "bool";
    arg_types["stop-at-end"]             = "bool";
    arg_types["dont-stop"]               = "bool";
    arg_types["loop"]                    = "bool";
    arg_types["realtime"]                = "bool";
    arg_types["no-time-travel"]          = "bool";
    arg_types["colour-images"]           = "bool";
    arg_types["hide-date"]               = "bool";
    arg_types["hide-files"]              = "bool";
    arg_types["hide-users"]              = "bool";
    arg_types["hide-tree"]               = "bool";
    arg_types["hide-usernames"]          = "bool";
    arg_types["hide-filenames"]          = "bool";
    arg_types["hide-dirnames"]           = "bool";
    arg_types["hide-progress"]           = "bool";
    arg_types["hide-bloom"]              = "bool";
    arg_types["hide-mouse"]              = "bool";
    arg_types["hide-root"]               = "bool";
    arg_types["highlight-users"]         = "bool";
    arg_types["highlight-dirs"]          = "bool";
    arg_types["file-extensions"]         = "bool";
    arg_types["file-extension-fallback"] = "bool";
    arg_types["key"]                     = "bool";
    arg_types["ffp"]                     = "bool";

    arg_types["disable-auto-rotate"] = "bool";
    arg_types["disable-auto-skip"]   = "bool";
    arg_types["disable-input"]       = "bool";

    arg_types["git-log-command"]= "bool";
    arg_types["cvs-exp-command"]= "bool";
    arg_types["cvs2cl-command"] = "bool";
    arg_types["svn-log-command"]= "bool";
    arg_types["hg-log-command"] = "bool";
    arg_types["bzr-log-command"]= "bool";

    arg_types["bloom-intensity"]   = "float";
    arg_types["bloom-multiplier"]  = "float";
    arg_types["elasticity"]        = "float";
    arg_types["seconds-per-day"]   = "float";
    arg_types["auto-skip-seconds"] = "float";
    arg_types["stop-at-time"]      = "float";
    arg_types["max-user-speed"]    = "float";
    arg_types["user-friction"]     = "float";
    arg_types["padding"]           = "float";
    arg_types["time-scale"]        = "float";
    arg_types["dir-name-position"] = "float";
    arg_types["loop-delay-seconds"] = "float";

    arg_types["max-files"] = "int";
    arg_types["font-size"] = "int";
    arg_types["font-scale"] = "float";
    arg_types["file-font-size"] = "int";
    arg_types["dir-font-size"] = "int";
    arg_types["user-font-size"] = "int";
    arg_types["hash-seed"] = "int";

    arg_types["user-filter"]      = "multi-value";
    arg_types["user-show-filter"] = "multi-value";
    arg_types["file-filter"]      = "multi-value";
    arg_types["file-show-filter"] = "multi-value";
    arg_types["follow-user"]      = "multi-value";
    arg_types["highlight-user"]   = "multi-value";

    arg_types["log-level"]          = "string";
    arg_types["background-image"]   = "string";
    arg_types["logo"]               = "string";
    arg_types["logo-offset"]        = "string";
    arg_types["log-command"]        = "string";
    arg_types["load-config"]        = "string";
    arg_types["save-config"]        = "string";
    arg_types["output-custom-log"]  = "string";
    arg_types["path"]               = "string";
    arg_types["log-command"]        = "string";
    arg_types["background-colour"]  = "string";
    arg_types["file-idle-time"]     = "string";
    arg_types["user-image-dir"]     = "string";
    arg_types["default-user-image"] = "string";
    arg_types["date-format"]        = "string";
    arg_types["log-format"]         = "string";
    arg_types["git-branch"]         = "string";
    arg_types["start-position"]     = "string";
    arg_types["start-date"]         = "string";
    arg_types["stop-date"]          = "string";
    arg_types["stop-position"]      = "string";
    arg_types["crop"]               = "string";
    arg_types["hide"]               = "string";
    arg_types["max-file-lag"]       = "string";
    arg_types["user-scale"]         = "string";
    arg_types["camera-mode"]        = "string";
    arg_types["title"]              = "string";
    arg_types["font-file"]          = "string";
    arg_types["font-colour"]        = "string";
    arg_types["highlight-colour"]   = "string";
    arg_types["selection-colour"]   = "string";
    arg_types["dir-colour"]         = "string";

    arg_types["caption-file"]       = "string";
    arg_types["caption-size"]       = "int";
    arg_types["caption-duration"]   = "float";
    arg_types["caption-colour"]     = "string";
    arg_types["caption-offset"]     = "int";

    arg_types["filename-colour"]    = "string";
    arg_types["filename-time"]      = "float";

    arg_types["dir-name-depth"]     = "int";
}

void GourceSettings::setGourceDefaults() {

    path = ".";
    default_path = true;

    ffp = false;

    hide_date      = false;
    hide_users     = false;
    hide_tree      = false;
    hide_files     = false;
    hide_usernames = false;
    hide_filenames = false;
    hide_dirnames  = false;
    hide_progress  = false;
    hide_bloom     = false;
    hide_mouse     = false;
    hide_root      = false;

    start_timestamp = 0;
    start_date = "";

    stop_timestamp = 0;
    stop_date = "";

    start_position = 0.0f;
    stop_position  = 0.0f;
    stop_at_time   = -1.0f;
    stop_on_idle   = false;
    stop_at_end    = false;
    dont_stop      = false;
    no_time_travel = false;

    show_key = false;

    disable_auto_rotate = false;

    disable_input = false;

    auto_skip_seconds = 3.0f;
    days_per_second   = 0.1f; // TODO: check this is right
    file_idle_time    = 0.0f;
    time_scale        = 1.0f;

    loop = false;
    loop_delay_seconds = 3.0f;

    logo = "";
    logo_offset = vec2(20.0f,20.0f);

    colour_user_images = false;
    default_user_image = "";
    user_image_dir     = "";
    user_image_map.clear();

    camera_zoom_min     = 50.0f;
    camera_zoom_default = 100.0f;
    camera_zoom_max     = 10000.0f;

    camera_mode     = "overview";
    padding         = 1.1f;

    crop_vertical   = false;
    crop_horizontal = false;

    bloom_multiplier = 1.0f;
    bloom_intensity  = 0.75f;

    background_colour = vec3(0.1f, 0.1f, 0.1f);
    background_image  = "";

    title             = "";

    font_file = GOURCE_FONT_FILE;
    font_size = 16;
    filename_font_size = 14;
    dirname_font_size = 14;
    user_font_size = 14;
    dir_colour       = vec3(1.0f);
    font_colour      = vec3(1.0f);
    highlight_colour = vec3(1.0f);
    selection_colour = vec3(1.0, 1.0, 0.3f);

    dir_name_depth = 0;
    dir_name_position = 0.5f;

    elasticity = 0.0f;

    git_branch = "";

    log_format  = "";
    date_format = "%A, %d %B, %Y %X";

    max_files      = 0;
    max_user_speed = 500.0f;
    max_file_lag   = 5.0f;

    user_idle_time = 3.0f;
    user_friction  = 1.0f;
    user_scale     = 1.0f;

    follow_users.clear();
    highlight_users.clear();
    highlight_all_users = false;
    highlight_dirs = false;

    caption_file     = "";
    caption_duration = 10.0f;
    caption_size     = 16;
    caption_offset   = 0;
    caption_colour   = vec3(1.0f, 1.0f, 1.0f);

    filename_colour  = vec3(1.0f, 1.0f, 1.0f);
    filename_time = 4.0f;

    gStringHashSeed = 31;

    //delete file filters
    for(std::vector<Regex*>::iterator it = file_filters.begin(); it != file_filters.end(); it++) {
        delete (*it);
    }
    file_filters.clear();

    //delete file whitelists
    for(std::vector<Regex*>::iterator it = file_show_filters.begin(); it != file_show_filters.end(); it++) {
        delete (*it);
    }    
    file_show_filters.clear();

    file_extensions = false;
    file_extension_fallback = false;

    //delete user filters
    for(std::vector<Regex*>::iterator it = user_filters.begin(); it != user_filters.end(); it++) {
        delete (*it);
    }
    user_filters.clear();

    //delete user whitelist
    for(std::vector<Regex*>::iterator it = user_show_filters.begin(); it != user_show_filters.end(); it++) {
        delete (*it);
    }
    user_show_filters.clear();
}

void GourceSettings::commandLineOption(const std::string& name, const std::string& value) {

    if(name == "help") {
        help();
    }

    if(name == "extended-help") {
        help(true);
    }

    if(name == "load-config" && value.size() > 0) {
        load_config = value;
        return;
    }

    if(name == "save-config" && value.size() > 0) {
        save_config = value;
        return;
    }

    std::string log_command;

    if(name == "log-command") {
        log_command = value;
    }

    if(name == "git-log-command" || log_command == "git") {
        SDLAppInfo(GitCommitLog::logCommand());
    }

    if(name == "cvs-exp-command" || log_command == "cvs-exp") {
        SDLAppInfo(CVSEXPCommitLog::logCommand());
    }

    if(log_command == "cvs") {
        throw ConfFileException("please use either 'cvs2cl' or 'cvs-exp'", "", 0);
    }

    if(name == "cvs2cl-command" || log_command == "cvs2cl") {
        SDLAppInfo(CVS2CLCommitLog::logCommand());
    }

    if(name == "svn-log-command" || log_command == "svn") {
        SDLAppInfo(SVNCommitLog::logCommand());
    }

    if(name == "hg-log-command" || log_command == "hg") {
        SDLAppInfo(MercurialLog::logCommand());
    }

    if(name == "bzr-log-command" || log_command == "bzr") {
        SDLAppInfo(BazaarLog::logCommand());
    }

    if(name == "output-custom-log" && value.size() > 0) {
        output_custom_filename = value;
        return;
    }

    if(name == "log-level") {
        if(value == "warn") {
            log_level = LOG_LEVEL_WARN;
        } else if(value == "debug") {
            log_level = LOG_LEVEL_DEBUG;
        } else if(value == "info") {
            log_level = LOG_LEVEL_INFO;
        } else if(value == "error") {
            log_level = LOG_LEVEL_ERROR;
        } else if(value == "pedantic") {
            log_level = LOG_LEVEL_PEDANTIC;
        }
        return;
    }

    std::string invalid_error = std::string("invalid ") + name + std::string(" value");
    throw ConfFileException(invalid_error, "", 0);
}


#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif
void GourceSettings::importGourceSettings(ConfFile& conffile, ConfSection* gource_settings) {

    setGourceDefaults();

    if(gource_settings == 0) gource_settings = conffile.getSection(default_section_name);

    if(gource_settings == 0) {
        gource_settings = conffile.addSection("gource");
    }

    ConfEntry* entry = 0;

    //hide flags

    std::vector<std::string> hide_fields;

    if((entry = gource_settings->getEntry("hide")) != 0) {

        if(!entry->hasValue()) conffile.missingValueException(entry);

        std::string hide_string = entry->getString();

        size_t sep;
        while((sep = hide_string.find(",")) != std::string::npos) {

            if(sep == 0 && hide_string.size()==1) break;

            if(sep == 0) {
                hide_string = hide_string.substr(sep+1, hide_string.size()-1);
                continue;
            }

            std::string hide_field  = hide_string.substr(0, sep);
            hide_fields.push_back(hide_field);
            hide_string = hide_string.substr(sep+1, hide_string.size()-1);
        }

        if(hide_string.size() > 0 && hide_string != ",") hide_fields.push_back(hide_string);

        //validate field list

        for(std::vector<std::string>::iterator it = hide_fields.begin(); it != hide_fields.end(); it++) {
            std::string hide_field = (*it);

            if(   hide_field != "date"
               && hide_field != "users"
               && hide_field != "tree"
               && hide_field != "files"
               && hide_field != "usernames"
               && hide_field != "filenames"
               && hide_field != "dirnames"
               && hide_field != "bloom"
               && hide_field != "progress"
               && hide_field != "mouse"
               && hide_field != "root") {
                std::string unknown_hide_option = std::string("unknown option hide ") + hide_field;
                conffile.entryException(entry, unknown_hide_option);
            }
        }
    }

    //check hide booleans
    for(std::map<std::string,std::string>::iterator it = arg_types.begin(); it != arg_types.end(); it++) {
        if(it->first.find("hide-") == 0 && it->second == "bool") {

            if(gource_settings->getBool(it->first)) {
                std::string hide_field = it->first.substr(5, it->first.size()-5);
                hide_fields.push_back(hide_field);
            }
        }
    }

    if(hide_fields.size()>0) {

        for(std::vector<std::string>::iterator it = hide_fields.begin(); it != hide_fields.end(); it++) {
            std::string hidestr = (*it);

                if(hidestr == "date")       hide_date      = true;
            else if(hidestr == "users")     hide_users     = true;
            else if(hidestr == "tree")      hide_tree      = true;
            else if(hidestr == "files")     hide_files     = true;
            else if(hidestr == "usernames") hide_usernames = true;
            else if(hidestr == "filenames") hide_filenames = true;
            else if(hidestr == "dirnames")  hide_dirnames  = true;
            else if(hidestr == "bloom")     hide_bloom     = true;
            else if(hidestr == "progress")  hide_progress  = true;
            else if(hidestr == "root")      hide_root      = true;
            else if(hidestr == "mouse")     {
                hide_mouse     = true;
                hide_progress  = true;
            }
        }
    }

    if((entry = gource_settings->getEntry("date-format")) != 0) {

        if(!entry->hasValue()) conffile.missingValueException(entry);

        date_format = entry->getString();
    }

    if(gource_settings->getBool("disable-auto-rotate")) {
        disable_auto_rotate=true;
    }

    if(gource_settings->getBool("disable-auto-skip")) {
        auto_skip_seconds = -1.0;
    }

    if(gource_settings->getBool("disable-input")) {
        disable_input=true;
    }

    if(gource_settings->getBool("loop")) {
        loop = true;
    }

    if((entry = gource_settings->getEntry("loop-delay-seconds")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify loop-delay-seconds (float)");

        loop_delay_seconds = entry->getFloat();

        if(loop_delay_seconds<=0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("git-branch")) != 0) {

        if(!entry->hasValue()) conffile.missingValueException(entry);

        Regex branch_regex("^(?!-)[/\\w.,;_=+{}\\[\\]-]+$");

        std::string branch = entry->getString();

        if(branch_regex.match(branch)) {
            git_branch = branch;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if(gource_settings->getBool("colour-images")) {
        colour_user_images = true;
    }

    if((entry = gource_settings->getEntry("crop")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify crop (vertical,horizontal)");

        std::string crop = entry->getString();

        if(crop == "vertical") {
            crop_vertical = true;
        } else if (crop == "horizontal") {
            crop_horizontal = true;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("log-format")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify log-format (format)");

        log_format = entry->getString();

        if(log_format == "cvs") {
            conffile.entryException(entry, "please use either 'cvs2cl' or 'cvs-exp'");
        }

        if(   log_format != "git"
           && log_format != "cvs-exp"
           && log_format != "cvs2cl"
           && log_format != "svn"
           && log_format != "custom"
           && log_format != "hg"
           && log_format != "bzr"
           && log_format != "apache") {

            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("default-user-image")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify default-user-image (image path)");

        default_user_image = entry->getString();
    }

    if((entry = gource_settings->getEntry("user-image-dir")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify user-image-dir (directory)");

        user_image_dir = entry->getString();

        //append slash
        if(user_image_dir[user_image_dir.size()-1] != '/') {
            user_image_dir += std::string("/");
        }

        user_image_map.clear();

        boost::filesystem::path image_dir_path(user_image_dir);

        if(!is_directory(image_dir_path)) {
             conffile.entryException(entry, "specified user-image-dir is not a directory");
        }

        std::vector<boost::filesystem::path> image_dir_files;

        try {
            copy(boost::filesystem::directory_iterator(image_dir_path), boost::filesystem::directory_iterator(), back_inserter(image_dir_files));
        } catch(const boost::filesystem::filesystem_error& exception) {
             conffile.entryException(entry, "error reading specified user-image-dir");
        }

        for(boost::filesystem::path& p : image_dir_files) {

            std::string dirfile;

#ifdef _WIN32
            std::wstring dirfile_16 = p.filename().wstring();
            utf8::utf16to8(dirfile_16.begin(), dirfile_16.end(), back_inserter(dirfile));
#else
            dirfile = p.filename().string();
#endif
            std::string file_ext = extension(p);
            boost::algorithm::to_lower(file_ext);

            if(file_ext != ".jpg" && file_ext != ".jpeg" && file_ext != ".png") continue;

            std::string image_path = gGourceSettings.user_image_dir + dirfile;
            std::string name       = dirfile.substr(0,dirfile.size() - file_ext.size());

#ifdef __APPLE__
                CFMutableStringRef help = CFStringCreateMutable(kCFAllocatorDefault, 0);
                CFStringAppendCString(help, name.c_str(), kCFStringEncodingUTF8);
                CFStringNormalize(help, kCFStringNormalizationFormC);
                char data[4096];
                CFStringGetCString(help,
                                   data,
                                   sizeof(data),
                                   kCFStringEncodingUTF8);
                name = data;
#endif

            debugLog("%s => %s", name.c_str(), image_path.c_str());

            user_image_map[name] = image_path;
        }
    }

    if((entry = gource_settings->getEntry("caption-file")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify caption file (filename)");

        caption_file = entry->getString();

        if(!boost::filesystem::exists(caption_file)) {
            conffile.entryException(entry, "caption file not found");
        }
    }

    if((entry = gource_settings->getEntry("caption-duration")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify caption duration (seconds)");

        caption_duration = entry->getFloat();

        if(caption_duration <= 0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("caption-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify caption size");

        caption_size = entry->getInt();

        if(caption_size<1 || caption_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("caption-offset")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify caption offset");

        caption_offset = entry->getInt();
    }

    if((entry = gource_settings->getEntry("caption-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify caption colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            caption_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            caption_colour = vec3(r,g,b);
            caption_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("filename-colour")) != 0) {
        if(!entry->hasValue()) conffile.entryException(entry, "specify filename colour (FFFFFF)");

	int r,g,b;

	std::string colstring = entry->getString();

	if(entry->isVec3()) {
	    filename_colour = entry->getVec3();
	} else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            filename_colour = vec3(r,g,b);
            filename_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("filename-time")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify duration to keep files on screen (float)");

        filename_time = entry->getFloat();

        if(filename_time<2.0f) {
            conffile.entryException(entry, "filename-time must be >= 2.0");
        }
    }

    if((entry = gource_settings->getEntry("bloom-intensity")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify bloom-intensity (float)");

        bloom_intensity = entry->getFloat();

        if(bloom_intensity<=0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("bloom-multiplier")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify bloom-multiplier (float)");

        bloom_multiplier = entry->getFloat();

        if(bloom_multiplier<=0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("elasticity")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify elasticity (float)");

        elasticity = entry->getFloat();

        if(elasticity<=0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("font-file")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font file");

        font_file = entry->getString();

        boost::filesystem::path font_file_path(font_file);

        if(!boost::filesystem::exists(font_file_path)) {
            conffile.invalidValueException(entry);
        }

        font_file = boost::filesystem::canonical(font_file_path).string();

        if(font_file.empty()) {
           conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("font-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font size");

        font_size = entry->getInt();

        if(font_size<1 || font_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("file-font-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font size");

        filename_font_size = entry->getInt();

        if(filename_font_size<1 || filename_font_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("dir-font-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font size");

        dirname_font_size = entry->getInt();

        if(dirname_font_size<1 || dirname_font_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("user-font-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font size");

        user_font_size = entry->getInt();

        if(user_font_size<1 || user_font_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("font-scale")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font scale");

        float font_scale = entry->getFloat();

        if(font_scale<0.0f || font_scale>10.0f) {
            conffile.invalidValueException(entry);
        }

        font_size         = glm::clamp((int)(font_size * font_scale), 1, 100);
        user_font_size    = glm::clamp((int)(user_font_size * font_scale), 1, 100);
        dirname_font_size = glm::clamp((int)(dirname_font_size * font_scale), 1, 100);
    }    

    if((entry = gource_settings->getEntry("hash-seed")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify hash seed (integer)");

        gStringHashSeed = entry->getInt();
    }

    if((entry = gource_settings->getEntry("font-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            font_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            font_colour = vec3(r,g,b);
            font_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("background-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify background colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            background_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            background_colour = vec3(r,g,b);
            background_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("highlight-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify highlight colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            highlight_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            highlight_colour = vec3(r,g,b);
            highlight_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("selection-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify selection colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            selection_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            selection_colour = vec3(r,g,b);
            selection_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("dir-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify dir colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            dir_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            dir_colour = vec3(r,g,b);
            dir_colour /= 255.0f;
        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("background-image")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify background image (image path)");

        background_image = entry->getString();
    }

    if((entry = gource_settings->getEntry("title")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify title");

        title = entry->getString();
    }

    if((entry = gource_settings->getEntry("logo")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify logo (image path)");

        logo = entry->getString();
    }

    if((entry = gource_settings->getEntry("logo-offset")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify logo-offset (XxY)");

        std::string logo_offset_str = entry->getString();

        int posx = 0;
        int posy = 0;

        if(parseRectangle(logo_offset_str, posx, posy)) {
            logo_offset = vec2(posx, posy);
        } else {
            conffile.invalidValueException(entry);
        }

    }

    if((entry = gource_settings->getEntry("seconds-per-day")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify seconds-per-day (seconds)");

        float seconds_per_day = entry->getFloat();

        if(seconds_per_day<=0.0f) {
            conffile.invalidValueException(entry);
        }

        // convert seconds-per-day to days-per-second
        days_per_second = 1.0 / seconds_per_day;
    }

    if((entry = gource_settings->getEntry("auto-skip-seconds")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify auto-skip-seconds (seconds)");

        auto_skip_seconds = entry->getFloat();

        if(auto_skip_seconds <= 0.0) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("file-idle-time")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify file-idle-time (seconds)");

        std::string file_idle_str = entry->getString();

        file_idle_time = (float) atoi(file_idle_str.c_str());

        if(file_idle_time<0.0f || (file_idle_time == 0.0f && file_idle_str[0] != '0') ) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("user-idle-time")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify user-idle-time (seconds)");

        user_idle_time = entry->getFloat();

        if(user_idle_time < 0.0f) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("time-scale")) != 0) {

        if(!entry->hasValue())
            conffile.entryException(entry, "specify time-scale (scale)");

        time_scale = entry->getFloat();

        if(time_scale <= 0.0f || time_scale > 4.0f) {
            conffile.entryException(entry, "time-scale outside of range 0.0 - 4.0");
        }
    }

    if((entry = gource_settings->getEntry("start-date")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify start-date (YYYY-MM-DD hh:mm:ss)");

        std::string start_date_string = entry->getString();

        if(parseDateTime(start_date_string, start_timestamp)) {

            char datestr[256];
            strftime(datestr, 256, "%Y-%m-%d", localtime ( &start_timestamp ));
            start_date = datestr;

        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("stop-date")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify stop-date (YYYY-MM-DD hh:mm:ss)");

        std::string end_date_string = entry->getString();

        if(parseDateTime(end_date_string, stop_timestamp)) {

            struct tm * timeinfo;
            timeinfo = localtime ( &stop_timestamp );

            time_t stop_timestamp_rounded = stop_timestamp;

            if(timeinfo->tm_hour > 0 || timeinfo->tm_min > 0 || timeinfo->tm_sec > 0) {
                stop_timestamp_rounded += 60*60*24;
            }

            char datestr[256];
            strftime(datestr, 256, "%Y-%m-%d", localtime ( &stop_timestamp_rounded ));
            stop_date = datestr;

        } else {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("start-position")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify start-position (float,random)");

        if(entry->getString() == "random") {
            srand(time(0));
            start_position = (rand() % 1000) / 1000.0f;
        } else {
            start_position = entry->getFloat();

            if(start_position<=0.0 || start_position>=1.0) {
                conffile.entryException(entry, "start-position outside of range 0.0 - 1.0 (non-inclusive)");
            }
        }
    }

    if((entry = gource_settings->getEntry("stop-position")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify stop-position (float)");

        stop_position = entry->getFloat();

        if(stop_position<=0.0 || stop_position>1.0) {
            conffile.entryException(entry, "stop-position outside of range 0.0 - 1.0 (inclusive)");
        }
    }

    if((entry = gource_settings->getEntry("stop-at-time")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify stop-at-time (seconds)");

        stop_at_time = entry->getFloat();

        if(stop_at_time <= 0.0) {
            conffile.invalidValueException(entry);
        }
    }

    if(gource_settings->getBool("key")) {
        show_key = true;
    }

    if(gource_settings->getBool("ffp")) {
        ffp = true;
    }

    if(gource_settings->getBool("realtime")) {
        days_per_second = 1.0 / 86400.0;
    }

    if(gource_settings->getBool("no-time-travel")) {
        no_time_travel = true;
    }

    if(gource_settings->getBool("dont-stop")) {
        dont_stop = true;
    }

    if(gource_settings->getBool("stop-at-end")) {
        stop_at_end = true;
    }

    //NOTE: this no longer does anything
    if(gource_settings->getBool("stop-on-idle")) {
        stop_on_idle = true;
    }

    if((entry = gource_settings->getEntry("max-files")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify max-files (number)");

        max_files = entry->getInt();

        if( max_files<0 || (max_files == 0 && entry->getString() != "0") ) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("max-file-lag")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify max-file-lag (seconds)");

        max_file_lag = entry->getFloat();

        if(max_file_lag==0.0) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("user-friction")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify user-friction (seconds)");

        user_friction = entry->getFloat();

        if(user_friction<=0.0) {
            conffile.invalidValueException(entry);
        }

        user_friction = 1.0 / user_friction;
    }

    if((entry = gource_settings->getEntry("user-scale")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify user-scale (scale)");

        user_scale = entry->getFloat();

        if(user_scale<=0.0 || user_scale>100.0) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("max-user-speed")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify max-user-speed (units)");

        max_user_speed = entry->getFloat();

        if(max_user_speed<=0) {
            conffile.invalidValueException(entry);
        }
    }

    if(   gource_settings->getBool("highlight-users")
       || gource_settings->getBool("highlight-all-users")) {
        highlight_all_users = true;
    }

    if(gource_settings->getBool("highlight-dirs")) {
        highlight_dirs = true;
    }

    if((entry = gource_settings->getEntry("camera-mode")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify camera-mode (overview,track)");

        camera_mode = entry->getString();

        if(camera_mode != "overview" && camera_mode != "track") {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("padding")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify padding (float)");

        padding = entry->getFloat();

        if(padding <= 0.0f || padding >= 2.0f) {
            conffile.invalidValueException(entry);
        }
    }

    // multi-value entries

    if((entry = gource_settings->getEntry("highlight-user")) != 0) {

        ConfEntryList* highlight_user_entries = gource_settings->getEntries("highlight-user");

        for(ConfEntryList::iterator it = highlight_user_entries->begin(); it != highlight_user_entries->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify highlight-user (user)");

            highlight_users.push_back(entry->getString());
        }
    }

    if((entry = gource_settings->getEntry("follow-user")) != 0) {

        ConfEntryList* follow_user_entries = gource_settings->getEntries("follow-user");

        for(ConfEntryList::iterator it = follow_user_entries->begin(); it != follow_user_entries->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify follow-user (user)");

            follow_users.push_back(entry->getString());
        }
    }

    if(gource_settings->getBool("file-extensions")) {
        file_extensions=true;
    }

    if(gource_settings->getBool("file-extension-fallback")) {
        file_extension_fallback=true;
    }

    if((entry = gource_settings->getEntry("file-filter")) != 0) {

        ConfEntryList* filters = gource_settings->getEntries("file-filter");

        for(ConfEntryList::iterator it = filters->begin(); it != filters->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify file-filter (regex)");

            std::string filter_string = entry->getString();

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                conffile.entryException(entry, "invalid file-filter regular expression");
            }

            file_filters.push_back(r);
        }
    }

    if((entry = gource_settings->getEntry("file-show-filter")) != 0) {

        ConfEntryList* filters = gource_settings->getEntries("file-show-filter");

        for(ConfEntryList::iterator it = filters->begin(); it != filters->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify file-show-filter (regex)");

            std::string filter_string = entry->getString();

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                conffile.entryException(entry, "invalid file-show-filter regular expression");
            }

            file_show_filters.push_back(r);
        }
    }

    if((entry = gource_settings->getEntry("user-filter")) != 0) {

        ConfEntryList* filters = gource_settings->getEntries("user-filter");

        for(ConfEntryList::iterator it = filters->begin(); it != filters->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify user-filter (regex)");

            std::string filter_string = entry->getString();

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                conffile.entryException(entry, "invalid user-filter regular expression");
            }

            user_filters.push_back(r);
        }
    }

    if((entry = gource_settings->getEntry("user-show-filter")) != 0) {

        ConfEntryList* filters = gource_settings->getEntries("user-show-filter");

        for(ConfEntryList::iterator it = filters->begin(); it != filters->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify user-show-filter (regex)");

            std::string filter_string = entry->getString();

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                conffile.entryException(entry, "invalid user-show-filter regular expression");
            }

            user_show_filters.push_back(r);
        }
    }

    if((entry = gource_settings->getEntry("dir-name-depth")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify dir-name-depth (depth)");

        dir_name_depth = entry->getInt();

        if(dir_name_depth <= 0) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("dir-name-position")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify dir-name-position (float)");

        dir_name_position = entry->getFloat();

        if(dir_name_position < 0.1f || dir_name_position > 1.0f) {
            conffile.entryException(entry, "dir-name-position outside of range 0.1 - 1.0 (inclusive)");
        }
    }

    //validate path
    if(gource_settings->hasValue("path")) {
        path = gource_settings->getString("path");
        default_path = false;
    }

    if(path == "-") {

        if(log_format.size() == 0) {
            throw ConfFileException("log-format required when reading from STDIN", "", 0);
        }

#ifdef _WIN32
        DWORD available_bytes;
        HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

        while(PeekNamedPipe(stdin_handle, 0, 0, 0,
            &available_bytes, 0) && available_bytes==0 && !std::cin.fail()) {
            SDL_Delay(100);
        }
#else
        while(std::cin.peek() == EOF && !std::cin.fail()) SDL_Delay(100);
#endif

        std::cin.clear();

    } else if(!path.empty() && path != ".") {

        //remove trailing slash
        if(path[path.size()-1] == '\\' || path[path.size()-1] == '/') {
            path.resize(path.size()-1);
        }

        // check path exists
        if(!boost::filesystem::exists(path)) {
            throw ConfFileException(str(boost::format("'%s' does not appear to be a valid file or directory") % path), "", 0);
        }
    }
}
