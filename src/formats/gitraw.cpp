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

#include "gitraw.h"

Regex git_raw_commit("^commit ([0-9a-z]+)");
Regex git_raw_tree("^tree ([0-9a-z]+)");
Regex git_raw_parent("^parent ([0-9a-z]+)");
Regex git_raw_author("^author (.+) <([^@>]+)@?([^>]*)> (\\d+) ([-+]\\d+)");
Regex git_raw_committer("^committer (.+) <([^@>]+)@?([^>]*)> (\\d+) ([-+]\\d+)");
Regex git_raw_file("^:[0-9]+ [0-9]+ [0-9a-z]+\\.* ([0-9a-z]+)\\.* ([A-Z])[ \\t]+(.+)");

// parse git log entries

// NOTE: this format is deprecated and exists
// to allow existing log files produced in this format to work

std::string gGourceGitRawLogCommand = "git log --reverse --raw --pretty=raw";

GitRawCommitLog::GitRawCommitLog(const std::string& logfile) : RCommitLog(logfile, 'c') {

    log_command = gGourceGitRawLogCommand;
}

bool GitRawCommitLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> entries;

    //read commit ref/ branch
    if(!logf->getNextLine(line)) return false;

    //commit
    if(!git_raw_commit.match(line, &entries)) return false;

    if(!logf->getNextLine(line)) return false;

    //tree
    if(!git_raw_tree.match(line, &entries)) return false;

    if(!logf->getNextLine(line)) return false;

    //0 or more parents
    while(git_raw_parent.match(line, &entries)) {
        if(!logf->getNextLine(line)) return false;
    }

    //author - used for display name
    if(!git_raw_author.match(line, &entries)) return false;

    commit.username = entries[0];

    if(!logf->getNextLine(line)) return false;

    //committer - used for time (most likely cronological)
    if(!git_raw_committer.match(line, &entries)) return false;

    commit.timestamp = atol(entries[3].c_str());

    //blank line before message
    if(!logf->getNextLine(line)) return false;

    //read commit message
    while(logf->getNextLine(line) && line.size()) {
    }

    //read files
    while(logf->getNextLine(line) && line.size()) {
        //debugLog("file??? %s\n", line.c_str());

        if(git_raw_file.match(line, &entries)) {
                commit.addFile(entries[2], entries[1]);
        }
    }

//    commit.debug();

    return true;
}
