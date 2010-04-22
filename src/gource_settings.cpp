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

GourceSettings gGourceSettings;

//display help message
void GourceSettings::help(bool extended_help) {

#ifdef _WIN32
    SDLAppCreateWindowsConsole();

    //resize window to fit help message
    SDLAppResizeWindowsConsole(750);
#endif

    printf("Gource v%s\n", GOURCE_VERSION);

    printf("Usage: gource [OPTIONS] [PATH]\n");
    printf("\nOptions:\n");
    printf("  -h, --help                       Help\n\n");
    printf("  -WIDTHxHEIGHT, --viewport        Set viewport size\n");
    printf("  -f, --fullscreen                 Fullscreen\n");
    printf("      --multi-sampling             Enable multi-sampling\n\n");

    printf("  -p, --start-position POSITION    Begin at some position (0.0-1.0 or 'random')\n");
    printf("      --stop-position  POSITION    Stop at some position\n");
    printf("  -t, --stop-at-time SECONDS       Stop after a specified number of seconds\n");
    printf("      --stop-on-idle               Stop on break in activity\n");
    printf("      --stop-at-end                Stop at end of the log\n");
    printf("      --dont-stop                  Keep running after the end of the log\n");
    printf("      --loop                       Loop at the end of the log\n\n");

    printf("  -a, --auto-skip-seconds SECONDS  Auto skip to next entry if nothing happens\n");
    printf("                                   for a number of seconds (default: 3)\n");
    printf("      --disable-auto-skip          Disable auto skip\n");
    printf("  -s, --seconds-per-day SECONDS    Speed in seconds per day (default: 10)\n");
    printf("      --realtime                   Realtime playback speed\n");
    printf("  -e, --elasticity FLOAT           Elasticity of nodes\n\n");

    printf("  --user-image-dir DIRECTORY       Dir containing images to use as avatars\n");
    printf("  --default-user-image IMAGE       Default user image file\n");
    printf("  --colour-images                  Colourize user images\n\n");

    printf("  -i, --file-idle-time SECONDS     Time files remain idle (default: 60)\n");
    printf("      --file-filter REGEX          Ignore files matching this regexe\n\n");

    printf("  --max-files NUMBER       Max number of active files (default: 1000)\n");
    printf("  --max-file-lag SECONDS   Max time files of a commit can take to appear\n\n");

    printf("  --log-command VCS        Show the log command used by gource (git,cvs,hg,bzr)\n");
    printf("  --log-format  VCS        Specify format of the log (git,cvs,hg,bzr,custom)\n");
    printf("  --git-branch             Get the git log of a particular branch\n\n");

    printf("  --follow-user USER       Camera will automatically follow this user\n");
    printf("  --highlight-user USER    Highlight the names of a particular user\n");
    printf("  --highlight-all-users    Highlight the names of all users\n\n");

    printf("  --load-config CONF_FILE  Load a config file\n");
    printf("  --save-config CONF_FILE  Save a config file with the current options\n\n");

    printf("  -o, --output-ppm-stream FILE    Output PPM stream to a file ('-' for STDOUT)\n");
    printf("  -r, --output-framerate  FPS     Framerate of output (25,30,60)\n\n");

if(extended_help) {
    printf("Extended Options:\n\n");

    printf("  -b, --background-colour  FFFFFF    Background colour in hex\n");
    printf("      --background-image   IMAGE     Set a background image\n\n");

    printf("  --bloom-multiplier       Adjust the amount of bloom (default: 1.0)\n");
    printf("  --bloom-intensity        Adjust the intensity of the bloom (default: 0.75)\n\n");

    printf("  --crop AXIS              Crop view on an axis (vertical,horizontal)\n");
    printf("  --camera-mode MODE       Camera mode (overview,track)\n\n");

    printf("  --disable-auto-rotate    Disable automatic camera rotation\n\n");

    printf("  --date-format FORMAT     Specify display date string (strftime format)\n\n");

    printf("  --font-size SIZE         Font size\n");
    printf("  --font-colour FFFFFF     Font colour in hex\n\n");

    printf("  --hide DISPLAY_ELEMENT   bloom,date,dirnames,files,filenames,mouse,progress,\n");
    printf("                           tree,users,usernames\n\n");

    printf("  --logo IMAGE             Logo to display in the foreground\n");
    printf("  --logo-offset XxY        Offset position of the logo\n\n");

    printf("  --title TITLE            Set a title\n\n");

    printf("  --transparent            Make the background transparent\n\n");

    printf("  --user-friction SECONDS  Time users come to a complete hault (default: 0.67)\n");
    printf("  --user-scale SCALE       Change scale of users (default: 1.0)\n");
    printf("  --max-user-speed UNITS   Speed users can travel per second (default: 500)\n\n");

    printf("  --path PATH\n\n");
}

    printf("PATH may be a Git, Bazaar or Mercurial dir, a log file or '-' to read STDIN.\n");
    printf("If ommited, gource will attempt to generate a log from the current directory.\n\n");

    if(!extended_help) {
        printf("To see the full command line options use '-H'\n\n");
    }

#ifdef _WIN32
    printf("Press Enter\n");
    getchar();
#endif

    exit(0);
}

GourceSettings::GourceSettings() {
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
    arg_aliases["background"] = "background-colour";
    arg_aliases["disable-bloom"]    = "hide-bloom";
    arg_aliases["disable-progress"] = "hide-progress";

    //command line only options
    conf_sections["help"]            = "command-line";
    conf_sections["extended-help"]   = "command-line";
    conf_sections["log-command"]     = "command-line";
    conf_sections["git-log-command"] = "command-line";
    conf_sections["cvs-exp-command"] = "command-line";
    conf_sections["hg-log-command"]  = "command-line";
    conf_sections["bzr-log-command"] = "command-line";
    conf_sections["load-config"]     = "command-line";
    conf_sections["save-config"]     = "command-line";

    //boolean args
    arg_types["help"]            = "bool";
    arg_types["extended-help"]   = "bool";
    arg_types["cvs-exp-command"] = "bool";
    arg_types["hg-log-command"]  = "bool";
    arg_types["bzr-log-command"] = "bool";
    arg_types["stop-on-idle"]    = "bool";
    arg_types["stop-at-end"]     = "bool";
    arg_types["dont-stop"]       = "bool";
    arg_types["loop"]            = "bool";
    arg_types["realtime"]        = "bool";
    arg_types["colour-images"]   = "bool";
    arg_types["hide-date"]       = "bool";
    arg_types["hide-files"]      = "bool";
    arg_types["hide-users"]      = "bool";
    arg_types["hide-tree"]       = "bool";
    arg_types["hide-usernames"]  = "bool";
    arg_types["hide-filenames"]  = "bool";
    arg_types["hide-dirnames"]   = "bool";
    arg_types["hide-progress"]   = "bool";
    arg_types["hide-bloom"]      = "bool";
    arg_types["hide-mouse"]      = "bool";

    arg_types["disable-auto-rotate"] = "bool";
    arg_types["disable-auto-skip"]   = "bool";

    arg_types["git-log-command"]= "bool";
    arg_types["cvs-exp-command"]= "bool";
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

    arg_types["max-files"] = "int";
    arg_types["font-size"] = "int";

    arg_types["file-filter"]    = "multi-value";
    arg_types["follow-user"]    = "multi-value";
    arg_types["highlight-user"] = "multi-value";

    arg_types["background-image"]   = "string";
    arg_types["logo"]               = "string";
    arg_types["logo-offset"]        = "string";
    arg_types["log-command"]        = "string";
    arg_types["load-config"]        = "string";
    arg_types["save-config"]        = "string";
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
    arg_types["stop-position"]      = "string";
    arg_types["crop"]               = "string";
    arg_types["hide"]               = "string";
    arg_types["max-file-lag"]       = "string";
    arg_types["user-scale"]         = "string";
    arg_types["camera-mode"]        = "string";
    arg_types["title"]              = "string";
    arg_types["font-colour"]        = "string";
}

void GourceSettings::setGourceDefaults() {

    path = ".";

    hide_date      = false;
    hide_users     = false;
    hide_tree      = false;
    hide_files     = false;
    hide_usernames = false;
    hide_filenames = false;
    hide_dirnames  = false;
    hide_progress = false;
    hide_bloom    = false;
    hide_mouse    = false;

    start_position = 0.0f;
    stop_position  = 0.0f;
    stop_at_time   = -1.0f;
    stop_on_idle   = false;
    stop_at_end    = false;
    dont_stop      = false;

    disable_auto_rotate = false;

    auto_skip_seconds = 3.0f;
    days_per_second   = 0.1f; // TODO: check this is right
    file_idle_time    = 60.0f;

    loop = false;

    logo = "";
    logo_offset = vec2f(20.0f,20.0f);

    colour_user_images = false;
    default_user_image = "";
    user_image_dir     = "";
    user_image_map.clear();

    camera_mode     = "overview";

    crop_vertical   = false;
    crop_horizontal = false;

    bloom_multiplier = 1.0f;
    bloom_intensity  = 0.75f;

    background_colour = vec3f(0.1f, 0.1f, 0.1f);
    background_image  = "";

    title             = "";

    font_size = 16;
    font_colour = vec3f(1.0f, 1.0f, 1.0f);

    elasticity = 0.0f;

    git_branch = "";

    log_format  = "";
    date_format = "%A, %d %B, %Y %X";

    max_files      = 1000;
    max_user_speed = 500.0f;
    max_file_lag   = 5.0f;

    user_idle_time = 3.0f;
    user_friction  = 1.0f;
    user_scale     = 1.0f;

    follow_users.clear();
    highlight_users.clear();
    highlight_all_users = false;

    //delete file filters
    for(std::vector<Regex*>::iterator it = file_filters.begin(); it != file_filters.end(); it++) {
        delete (*it);
    }
    file_filters.clear();
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
        SDLAppInfo(gGourceGitLogCommand);
    }

    if(name == "cvs-exp-command" || log_command == "cvs") {
        SDLAppInfo(gGourceCvsExpLogCommand);
    }

    if(name == "hg-log-command" || log_command == "hg") {
        std::string command = gGourceMercurialCommand();
        SDLAppInfo(command);
    }

    if(name == "bzr-log-command" || log_command == "bzr") {
        std::string command = gGourceBzrLogCommand();
        SDLAppInfo(command);
    }

    std::string invalid_error = std::string("invalid ") + name + std::string(" value");
    throw ConfFileException(invalid_error, "", 0);
}


void GourceSettings::importGourceSettings(ConfFile& conffile, ConfSection* gource_settings) {

    setGourceDefaults();

    if(gource_settings == 0) gource_settings = conffile.getSection(default_section_name);

    if(gource_settings == 0) {
        ConfSection* gource_section = new ConfSection("gource");
        conffile.addSection(gource_section);
        gource_settings = gource_section;
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
               && hide_field != "mouse") {
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

    if(gource_settings->getBool("loop")) {
        loop = true;
    }

    if((entry = gource_settings->getEntry("git-branch")) != 0) {

        if(!entry->hasValue()) conffile.missingValueException(entry);

        git_branch = entry->getString();
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

        if(   log_format != "git"
           && log_format != "cvs"
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

        //get jpg and png images in dir
        DIR *dp;
        struct dirent *dirp;

        user_image_map.clear();

        if((dp = opendir(gGourceSettings.user_image_dir.c_str())) != 0) {

            while ((dirp = readdir(dp)) != 0) {
                std::string dirfile = std::string(dirp->d_name);

                size_t extpos = 0;

                if(   (extpos=dirfile.rfind(".jpg"))  == std::string::npos
                && (extpos=dirfile.rfind(".jpeg")) == std::string::npos
                && (extpos=dirfile.rfind(".png"))  == std::string::npos) continue;


                std::string image_path = gGourceSettings.user_image_dir + dirfile;
                std::string name       = dirfile.substr(0,extpos);

                debugLog("%s => %s\n", name.c_str(), image_path.c_str());

                user_image_map[name] = image_path;
            }

            closedir(dp);
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

    if((entry = gource_settings->getEntry("font-size")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font size (FFFFFF)");

        font_size = entry->getInt();

        if(font_size<1 || font_size>100) {
            conffile.invalidValueException(entry);
        }
    }

    if((entry = gource_settings->getEntry("font-colour")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify font colour (FFFFFF)");

        int r,g,b;

        std::string colstring = entry->getString();

        if(entry->isVec3()) {
            font_colour = entry->getVec3();
        } else if(colstring.size()==6 && sscanf(colstring.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
            font_colour = vec3f(r,g,b);
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
            background_colour = vec3f(r,g,b);
            background_colour /= 255.0f;
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

        if(parseRectangle(logo_offset_str, &posx, &posy)) {
            logo_offset = vec2f(posx, posy);
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

        if(file_idle_time<0.0f || file_idle_time == 0.0f && file_idle_str[0] != '0' ) {
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

    if(gource_settings->getBool("realtime")) {
        days_per_second = 1.0 / 86400.0;
    }

    if(gource_settings->getBool("dont-stop")) {
        dont_stop = true;
    }

    if(gource_settings->getBool("stop-at-end")) {
        stop_at_end = true;
    }

    if(gource_settings->getBool("stop-on-idle")) {
        stop_on_idle = true;
    }

    if((entry = gource_settings->getEntry("max-files")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify max-files (number)");

        max_files = entry->getInt();

        if(max_files<1) {
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

    if(gource_settings->getBool("highlight-all-users")) {
        highlight_all_users = true;
    }

    if((entry = gource_settings->getEntry("camera-mode")) != 0) {

        if(!entry->hasValue()) conffile.entryException(entry, "specify camera-mode (overview,track)");

        camera_mode = entry->getString();

        if(camera_mode != "overview" && camera_mode != "track") {
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

    if((entry = gource_settings->getEntry("file-filter")) != 0) {

        ConfEntryList* filters = gource_settings->getEntries("file-filter");

        for(ConfEntryList::iterator it = filters->begin(); it != filters->end(); it++) {

            entry = *it;

            if(!entry->hasValue()) conffile.entryException(entry, "specify file-filter (regex)");

            std::string filter_string = entry->getString();

            Regex* r = new Regex(filter_string, 1);

            if(!r->isValid()) {
                delete r;
                conffile.entryException(entry, "invalid filt-filter regular expression");
            }

            file_filters.push_back(r);
        }
    }


    //validate path
    if(gource_settings->hasValue("path")) {
        path = gource_settings->getString("path");
    }

    if(path == "-") {

        if(log_format.size() == 0) {
            throw ConfFileException("log-format required when reading from STDIN", "", 0);
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
}
