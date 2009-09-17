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

// parse git log entries

std::string gGourceGitLogCommand = "git log "
    "--pretty=format:'%aN%n%ct' --reverse --raw";

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

    if(command_rc != 0) {
        return 0;
    }

    // check for new-enough Git version
    std::ifstream in(logfile_buff);
    char firstBytes[4];
    in.read(firstBytes, 3);
    in.close();
    firstBytes[3] = '\0';
    if(!strcmp(firstBytes, "%aN")) {
        char *pos = strstr(cmd_buff, "%aN");
        pos[2] = 'n';
        command_rc = system(cmd_buff);
    }

    //change back to original directoy
    chdir(cwd_buff);

    if(command_rc != 0) {
        return 0;
    }

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
#endif
}

// parse modified git format log entries

bool GitCommitLog::parseCommit(RCommit& commit) {

    std::string line = lastline;

    //read author name
    if(!line.size()) {
        if(!logf->getNextLine(line)) return false;
    }

    commit.username = line;

    if(!logf->getNextLine(line)) return false;

    //committer time - used instead of author time (most likely cronological)
    // NOTE: ignoring timezone ... 
    commit.timestamp = atol(line.c_str());

    //debugLog("timestamp = %ld\n", commit.timestamp);

    //read files
    while(logf->getNextLine(line) && line.size()) {
        //debugLog("file??? %s\n", line.c_str());

        size_t tab = line.find('\t');
        if(tab == std::string::npos)
            continue;
        std::string status = line.substr(tab - 1, 1);
        line = line.substr(tab + 1);
        commit.addFile(line, status);
    }

    lastline = "";

//    commit.debug();

    return true;
}
