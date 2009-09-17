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

#include "git.h"

Regex git_commit("^commit ([0-9a-z]+)");
Regex git_tree("^tree ([0-9a-z]+)");
Regex git_parent("^parent ([0-9a-z]+)");
Regex git_author("^author (.+) <([^@>]+)@?([^>]*)> (\\d+) ([-+]\\d+)");
Regex git_committer("^committer (.+) <([^@>]+)@?([^>]*)> (\\d+) ([-+]\\d+)");
Regex git_file("^:[0-9]+ [0-9]+ [0-9a-z]+\\.* ([0-9a-z]+)\\.* ([A-Z])[ \\t]+(.+)");

// parse git log entries

std::string gGourceGitLogCommand = "git log --reverse --raw --pretty=raw";

GitCommitLog::GitCommitLog(std::string logfile) : RCommitLog(logfile) {

    log_command = gGourceGitLogCommand;

    //can generate log from directory
    if(!logf && is_dir) {
        logf = generateLog(logfile);

        if(logf) {
            success  =  true;
            seekable = true;
        }
    }
}

BaseLog* GitCommitLog::generateLog(std::string dir) {
    // TODO: work on windows?
#ifdef _WIN32
    return 0;
#else
    //get working directory
    char cwd_buff[1024];

    if(getcwd(cwd_buff, 1024) != cwd_buff) {
        return 0;
    }

    std::string command = getLogCommand();

    //create temp file
    char cmd_buff[1024];
    char logfile_buff[128];

    uid_t myuid = getuid();

    sprintf(logfile_buff, "/tmp/gource-%d.tmp", myuid);
    sprintf(cmd_buff, "%s > %s", command.c_str(), logfile_buff);

    temp_file = std::string(logfile_buff);

    if(chdir(dir.c_str()) != 0) {
        return 0;
    }

    int command_rc = system(cmd_buff);

    //change back to original directoy
    chdir(cwd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
#endif
}

// parse modified cvs format log entries

bool GitCommitLog::parseCommit(RCommit& commit) {

    std::string line = lastline;
    std::vector<std::string> entries;

    //read commit ref/ branch
    if(!line.size()) {
        if(!logf->getNextLine(line)) return false;
    }

    //debugLog("first line: %s\n", line.c_str());

    //commit
    if(!git_commit.match(line, &entries)) return false;

    if(!logf->getNextLine(line)) return false;

    //tree
    if(!git_tree.match(line, &entries)) return false;

    if(!logf->getNextLine(line)) return false;

    //0 or more parents
    while(git_parent.match(line, &entries)) {
        if(!logf->getNextLine(line)) return false;
    }

    //author - used for display name
    if(!git_author.match(line, &entries)) return false;

    commit.username = entries[0];

    if(!logf->getNextLine(line)) return false;

    //committer - used for time (most likely cronological)
    if(!git_committer.match(line, &entries)) return false;

    // NOTE: ignoring timezone ... 
    commit.timestamp = atol(entries[3].c_str());

    //debugLog("timestamp = %ld\n", commit.timestamp);

/*
    struct tm time_str;
    time_str.tm_year  = atoi(entries[0].c_str()) - 1900;
    time_str.tm_mon   = atoi(entries[1].c_str()) - 1;
    time_str.tm_mday  = atoi(entries[2].c_str());
    time_str.tm_hour  = atoi(entries[3].c_str());
    time_str.tm_min   = atoi(entries[4].c_str());
    time_str.tm_sec   = atoi(entries[5].c_str());
    time_str.tm_isdst = -1;

    commit.timestamp = mktime(&time_str);
*/

    //blank line before message
    if(!logf->getNextLine(line)) return false;

    //std::string message;

    //read commit message
    while(logf->getNextLine(line) && line.size()) {
        /*
        if(message.size()) message += std::string("\n");
        while(line[0] == ' ') line = line.substr(1);
        message += line;
        */
    }

    //debugLog("message: %s\n", message.c_str());

    //read files
    while(logf->getNextLine(line) && line.size()) {
        //debugLog("file??? %s\n", line.c_str());

        if(git_file.match(line, &entries)) {
                commit.addFile(entries[2], entries[1]);
        } else {
            //oops this isnt a file
            lastline = line;
            break;
        }
    }

    lastline = "";

//    commit.debug();

    return true;
}
