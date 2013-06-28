/*
    Copyright (C) 2010 John Arbash Meinel <john@arbash-meinel.com>

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

#include "bzr.h"
#include "../gource_settings.h"

#include <boost/format.hpp>

Regex bzr_commit_regex("^ *([\\d.]+) (.+)\t(\\d{4})-(\\d+)-(\\d+)(?: \\{[^}]+})?(?: \\[merge\\])?$");
Regex bzr_file_regex("^ *([AMDR])  (.*[^/])$");

// parse Bazaar log entries (using the gource.style template)

std::string BazaarLog::logCommand() {
    std::string log_command = std::string("bzr log --verbose -r 1..-1 --short -n0 --forward");

    if(!gGourceSettings.start_date.empty()) {
        log_command.replace(log_command.find("1..-1"), 5, str(boost::format("date:%s..-1") % gGourceSettings.start_date));
    }

    return log_command;
}

BazaarLog::BazaarLog(const std::string& logfile) : RCommitLog(logfile) {

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

BaseLog* BazaarLog::generateLog(const std::string& dir) {

    //does directory have a .bzr ?
    std::string bzrdir = dir + std::string("/.bzr");
    struct stat dirinfo;
    int stat_rc = stat(bzrdir.c_str(), &dirinfo);
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR)) {
        return 0;
    }

    std::string command = getLogCommand();

    // do we have this client installed
    requireExecutable("bzr");

    createTempLog();

    if(temp_file.size()==0) return 0;

    char cmd_buff[2048];
    sprintf(cmd_buff, "%s %s > %s", command.c_str(), dir.c_str(), temp_file.c_str());

    int command_rc = systemCommand(cmd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}

bool BazaarLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;
    int year, month, day;

    if(!logf->getNextLine(line)) return false;

    if (!bzr_commit_regex.match(line, &entries)) {
        //debugLog("regex failed\n");
        return false;
    }

    commit.username = entries[1];

    year  = atoi(entries[2].c_str());
    month = atoi(entries[3].c_str());
    day   = atoi(entries[4].c_str());

    struct tm time_str;

    time_str.tm_year  = year - 1900;
    time_str.tm_mon   = month - 1;
    time_str.tm_mday  = day;
    time_str.tm_hour  = 0;
    time_str.tm_min   = 0;
    time_str.tm_sec   = 0;
    time_str.tm_isdst = -1;

    commit.timestamp = mktime(&time_str);

    while(logf->getNextLine(line) && line.size()) {
        if (!bzr_file_regex.match(line, &entries)) continue;
        commit.addFile(entries[1], entries[0]);
    }

    return true;
}
