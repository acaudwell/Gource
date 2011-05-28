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

#include "hg.h"

Regex hg_regex("^([0-9]+) -?[0-9]+\\|([^|]+)\\|([ADM]?)\\|(.+)$");

// parse Mercurial log entries (using the gource.style template)

std::string gGourceMercurialCommand() {

    std::string gource_style_path = gSDLAppResourceDir + std::string("gource.style");

    return std::string("hg log -r 0:tip --style \"") + gource_style_path + std::string("\"");
}

MercurialLog::MercurialLog(const std::string& logfile) : RCommitLog(logfile) {

    log_command = gGourceMercurialCommand();

    //can generate log from directory
    if(!logf && is_dir) {
        logf = generateLog(logfile);

        if(logf) {
            success  = true;
            seekable = true;
        }
    }
}

BaseLog* MercurialLog::generateLog(const std::string& dir) {

    //does directory have a .git ?
    std::string hgdir = dir + std::string("/.hg");
    struct stat dirinfo;
    int stat_rc = stat(hgdir.c_str(), &dirinfo);
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR)) {
        return 0;
    }

    std::string command = getLogCommand();

    createTempLog();

    if(temp_file.size()==0) return 0;

    char cmd_buff[2048];
    sprintf(cmd_buff, "%s -R \"%s\" > %s", command.c_str(), dir.c_str(), temp_file.c_str());

    int command_rc = system(cmd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}


bool MercurialLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;

    if(!logf->getNextLine(line)) return false;

    //custom line
    if(!hg_regex.match(line, &entries)) return false;

    commit.timestamp = atol(entries[0].c_str());

    commit.username = entries[1];

    std::string action = "A";

    if(entries[2].size()>0) {
        action = entries[2];
    }

    bool has_colour = false;
    vec3f colour;

//    debugLog("file = %s, timestamp=%d, username=%s, action=%s\n",  entries[3].c_str(),
//        commit.timestamp, commit.username.c_str(), action.c_str());

    if(has_colour) {
        commit.addFile(entries[3], action, colour);
    } else {
        commit.addFile(entries[3], action);
    }

    //commit.debug();

    return true;
}
