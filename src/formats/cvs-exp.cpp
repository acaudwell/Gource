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

#include "cvs-exp.h"

Regex cvsexp_commitno_regex("^([0-9]{6}):");
Regex cvsexp_branch_regex("^BRANCH \\[(.+)\\]$");
Regex cvsexp_date_regex("^\\(date: ([0-9]{4})[-/]([0-9]{2})[-/]([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})(?: [+-][0-9]{4})?;(.+)$");
Regex cvsexp_detail_regex("author: ([^;]+);  state: ([^;]+);(.+)$");
//Regex cvsexp_lines_regex("lines: \\+([0-9]+) -([0-9]+)");
Regex cvsexp_entry_regex("\\| (.+),v:([0-9.]+),?");
Regex cvsexp_end_regex("^(=+)$");

std::string CVSEXPCommitLog::logCommand() {
    std::string log_command = "cvs-exp.pl -notree";
    return log_command;
}

CVSEXPCommitLog::CVSEXPCommitLog(const std::string& logfile) : RCommitLog(logfile) {
}

// parse modified cvs format log entries

bool CVSEXPCommitLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;

    if(!logf->getNextLine(line)) return false;

    //skip empty line if there is one
    if(line.size() == 0) {
        if(!logf->getNextLine(line)) return false;
    }

    //read commit no
    if(!cvsexp_commitno_regex.match(line, &entries)) return false;

    //int commitno = atoi(entries[0].c_str());
    //debugLog("commitno matched\n");

    if(!logf->getNextLine(line)) return false;

    //should be a branch
    if(cvsexp_branch_regex.match(line, &entries)) {

        //read next blank line
        if(!logf->getNextLine(line)) return false;
        if(line.size()) return false;
        if(!logf->getNextLine(line)) return false;
   }

    //parse date
    if(!cvsexp_date_regex.match(line, &entries)) return false;

    //debugLog("date matched\n");

    struct tm time_str;

    time_str.tm_year = atoi(entries[0].c_str()) - 1900;
    time_str.tm_mon  = atoi(entries[1].c_str()) - 1;
    time_str.tm_mday = atoi(entries[2].c_str());
    time_str.tm_hour = atoi(entries[3].c_str());
    time_str.tm_min  = atoi(entries[4].c_str());
    time_str.tm_sec  = atoi(entries[5].c_str());
    time_str.tm_isdst = -1;

    commit.timestamp = mktime(&time_str);

    //parse author,state
    std::string rest = entries[6];
    if(!cvsexp_detail_regex.match(rest, &entries)) return false;

    //debugLog("author/state matched\n");

    commit.username = entries[0];

    std::string commit_state = entries[1];

    /* not used
    //if rest is not ')' parse lines
    rest = entries[2];

    // need to parse lines
    if(rest.size() > 2) {
        if(!cvsexp_lines_regex.match(rest, &entries)) return false;
    }
    */

    if(!logf->getNextLine(line)) return false;

    std::string commit_action = (commit_state == "dead") ? "D" : "M";

    while(cvsexp_entry_regex.match(line, &entries)) {

        //ignore files in Attic - previously deleted file
        if(entries[0].find("/Attic/") == std::string::npos) {
            commit.addFile(entries[0], commit_action);
        }

        if(!logf->getNextLine(line)) return false;
    }

    //read blank line
    if(!logf->getNextLine(line)) return false;

    //std::string message;

    //read commit message
    while(logf->getNextLine(line) && line.size()) {
        //if(message.size()) message += std::string("\n");
        //message += line;
    }

    //read until end of commit or eof
    while(logf->getNextLine(line)) {
        if(cvsexp_end_regex.match(line,&entries)) {
            //debugLog("read end of commit %s\n", entries[0].c_str());
            break;
        }
    }

    return true;
}
