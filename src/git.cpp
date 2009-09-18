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
    "--pretty=format:user:%aN%n%ct --reverse --raw";

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
    //get working directory
    char cwd_buff[1024];

    if(getcwd(cwd_buff, 1024) != cwd_buff) {
        return 0;
    }

    std::string command = getLogCommand();

    //create temp file
    char cmd_buff[2048];
    char logfile_buff[1024];

#ifdef _WIN32
    DWORD tmplen = GetTempPath(0, "");

    if(tmplen == 0) return 0;

    std::vector<TCHAR> temp(tmplen+1);

    tmplen = GetTempPath(static_cast<DWORD>(temp.size()), &temp[0]);

    if(tmplen == 0 || tmplen >= temp.size()) return 0;

    std::string temp_file_path(temp.begin(),
                               temp.begin() + static_cast<std::size_t>(tmplen));

    temp_file_path += "gource.tmp";

    sprintf(logfile_buff, "%s", temp_file_path.c_str());
#else
    uid_t myuid = getuid();
    sprintf(logfile_buff, "/tmp/gource-%d.tmp", myuid);
#endif

    sprintf(cmd_buff, "%s > %s", command.c_str(), logfile_buff);

    debugLog("command: %s\n", cmd_buff);

    temp_file = std::string(logfile_buff);

    if(chdir(dir.c_str()) != 0) {
        return 0;
    }

    int command_rc = system(cmd_buff);

    if(command_rc != 0) {
        return 0;
    }

    // check for new-enough Git version
    // if %aN does not appear to be supported try %an
    std::ifstream in(logfile_buff);
    char firstBytes[9];
    in.read(firstBytes, 8);
    in.close();
    firstBytes[8] = '\0';
    if(!strcmp(firstBytes, "user:%aN")) {
        char *pos = strstr(cmd_buff, "%aN");
        pos[2] = 'n';
        command_rc = system(cmd_buff);
    }

    //change back to original directoy
    chdir(cwd_buff);

    if(command_rc != 0) {
        return 0;
    }

    debugLog("temp_file = %s\n", temp_file.c_str());

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}

//get next line, preserving the last line, incase we abort parsing
bool GitCommitLog::getNextLine(std::string& line) {
    if(!logf->getNextLine(line)) return false;

    lastline = line;

    return true;
}

// parse modified git format log entries

bool GitCommitLog::parseCommit(RCommit& commit) {

    std::string line = lastline;

    //read author name
    if(!line.size()) {
        if(!getNextLine(line)) return false;
    }
    //ensure username prefixed with user: otherwise the log is not in
    //the expected format and we can try a different format
    if(line.size() < 6 || line.find("user:") != 0) {
        lastline = ""; //prevent loop

        return false;
    }

    //username follows user prefix
    commit.username = line.substr(5);

    if(!getNextLine(line)) return false;

    //committer time - used instead of author time (most likely cronological)
    // NOTE: ignoring timezone ...
    commit.timestamp = atol(line.c_str());

    //this isnt a commit we are parsing, abort
    if(commit.timestamp == 0) return false;

    //read files
    while(getNextLine(line) && line.size()) {
        size_t tab = line.find('\t');

        if(tab == std::string::npos) continue;

        //incorrect log format
        if(tab == 0) return false;

        std::string status = line.substr(tab - 1, 1);
        line = line.substr(tab + 1);
        commit.addFile(line, status);
    }

    //next call should read a new line from the file
    lastline = "";

    //commit.debug();

    return true;
}
