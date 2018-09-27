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
#include "../core/sdlapp.h"
#include "../gource_settings.h"

#include <boost/format.hpp>

Regex hg_regex("^([0-9]+) -?[0-9]+\\|([^|]+)\\|([ADM]?)\\|(.+)$");

std::string MercurialLog::logCommand() {

    // parse Mercurial log entries (using the gource.style template)
    std::string gource_style_path = gSDLAppResourceDir + std::string("gource.style");

    std::string range =
        // date range
        (!gGourceSettings.start_date.empty() && !gGourceSettings.stop_date.empty()) ?
          str(boost::format("--date '%s to %s'") % gGourceSettings.start_date % gGourceSettings.stop_date)

        // start date only
        : (!gGourceSettings.start_date.empty()) ?
          str(boost::format("--date '>%s'") % gGourceSettings.start_date)

        // stop date only
        : (!gGourceSettings.stop_date.empty()) ?
          str(boost::format("--date '<%s'") % gGourceSettings.stop_date)

        // default
        : "";

    std::string log_command = str(boost::format("hg log %s -r 0:tip --style '%s'") % range % gource_style_path);

#ifdef _WIN32
    std::replace(log_command.begin(), log_command.end(), '\'', '"');
#endif

    return log_command;
}

MercurialLog::MercurialLog(const std::string& logfile) : RCommitLog(logfile) {

    log_command = logCommand();

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

    //does directory have a .hg ?
    std::string hgdir = dir + std::string("/.hg");
    struct stat dirinfo;
    int stat_rc = stat(hgdir.c_str(), &dirinfo);
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR)) {
        return 0;
    }

    // do we have this client installed
    requireExecutable("hg");

    std::string command = getLogCommand();

    createTempLog();

    if(temp_file.size()==0) return 0;

    char cmd_buff[2048];
    snprintf(cmd_buff, 2048, "%s -R \"%s\" > %s", command.c_str(), dir.c_str(), temp_file.c_str());

    int command_rc = systemCommand(cmd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}


bool MercurialLog::parseCommit(RCommit& commit) {

    while(parseCommitEntry(commit));

    return !commit.files.empty();
}

bool MercurialLog::parseCommitEntry(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;

    if(!getNextLine(line)) return false;

    //custom line
    if(!hg_regex.match(line, &entries)) return false;

    time_t timestamp     = atol(entries[0].c_str());
    std::string username = entries[1];

    //if this file is for the same person and timestamp
    //we add to the commit, else we save the lastline
    //and return false
    if(commit.files.empty()) {
        commit.timestamp = timestamp;
        commit.username  = username;
    } else {
        if(commit.timestamp != timestamp || commit.username  != username) {
            lastline = line;
            return false;
        }
    }

    std::string action = "A";

    if(!entries[2].empty()) {
        action = entries[2];
    }

    commit.addFile(entries[3], action);

    //commit.debug();

    return true;
}
