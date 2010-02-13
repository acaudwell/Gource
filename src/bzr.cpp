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

Regex bzr_commit_regex("^ *([0-9]+) (.*)\t([0-9][0-9][0-9][0-9])-([0-9]+)-([0-9]+).*$");
Regex bzr_file_regex(" *([AMDR])  (.*)$");

// parse Bazaar log entries (using the gource.style template)

std::string gGourceBzrLogCommand() {
    return std::string("bzr log -r 1..-1 --short -n0 --forward");
}

BazaarLog::BazaarLog(std::string logfile) : RCommitLog(logfile) {

    log_command = gGourceBzrLogCommand();

    //can generate log from directory
    if(!logf && is_dir) {
        logf = generateLog(logfile);

        if(logf) {
            success  = true;
            seekable = true;
        }
    }
}

BaseLog* BazaarLog::generateLog(std::string dir) {

    //does directory have a .bzr ?
    std::string bzrdir = dir + std::string("/.bzr");
    struct stat dirinfo;
    int stat_rc = stat(bzrdir.c_str(), &dirinfo);
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR)) {
        return 0;
    }

    std::string command = getLogCommand();

    createTempLog();

    char cmd_buff[2048];
    sprintf(cmd_buff, "%s %s > %s", command.c_str(), dir.c_str(), temp_file.c_str());

    int command_rc = system(cmd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}

static const char *_separator = "------------------------------------------------------------";

bool BazaarLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;
    int year, month, day;

    if(!logf->getNextLine(line)) return false;

    debugLog("read %s\n", line.c_str());
    if (!bzr_commit_regex.match(line, &entries)) {
        debugLog("regex failed\n");
        return false;
    }
    commit.username = entries[1];
    year = atol(entries[2].c_str());
    month = atol(entries[3].c_str());
    day = atol(entries[3].c_str());
    // Bad approximation, but should work for now
    commit.timestamp = (((year - 1970) * 365) + (month * 30) + day) * 24*3600;
    while(logf->getNextLine(line) && line.size()) {
        if (!bzr_file_regex.match(line, &entries)) continue;
        commit.addFile(entries[1], entries[0]);
    }
    return true;
}
