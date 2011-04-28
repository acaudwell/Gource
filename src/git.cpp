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

//git-log command notes:
// - no single quotes on WIN32 as system call treats them differently
// - 'user:' prefix allows us to quickly tell if the log is the wrong format
//   and try a different format (eg cvs-exp)

std::string gGourceGitLogCommand = "git log "
    "--pretty=format:user:%aN%n%ct "
    "--reverse --raw --encoding=UTF-8 "
    "-M";

GitCommitLog::GitCommitLog(const std::string& logfile) : RCommitLog(logfile, 'u') {

    log_command = gGourceGitLogCommand;

    if(gGourceSettings.git_branch.size()>0) {
        log_command += " ";
        log_command += gGourceSettings.git_branch;
    }

    //can generate log from directory
    if(!logf && is_dir) {
        logf = generateLog(logfile);

        if(logf) {
            success  = true;
            seekable = true;
        }
    }
}

BaseLog* GitCommitLog::generateLog(const std::string& dir) {
    //get working directory
    char cwd_buff[1024];

    if(getcwd(cwd_buff, 1024) != cwd_buff) {
        return 0;
    }

    //does directory have a .git ?
    std::string gitdir = dir + std::string("/.git");
    struct stat dirinfo;
    int stat_rc = stat(gitdir.c_str(), &dirinfo);
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR)) {
        return 0;
    }

    std::string command = getLogCommand();

    //create temp file
    createTempLog();

    if(temp_file.size()==0) return 0;

    if(chdir(dir.c_str()) != 0) {
        return 0;
    }

    char cmd_buff[2048];
    sprintf(cmd_buff, "%s > %s", command.c_str(), temp_file.c_str());

    int command_rc = system(cmd_buff);

    if(command_rc != 0) {
        chdir(cwd_buff);
        return 0;
    }

    // check for new-enough Git version
    // if %aN does not appear to be supported try %an
    std::ifstream in(temp_file.c_str());
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

    BaseLog* seeklog = new SeekLog(temp_file);

    return seeklog;
}

// parse modified git format log entries

bool GitCommitLog::parseCommit(RCommit& commit) {

    std::string line;

    commit.username = "";

    while(logf->getNextLine(line) && line.size()) {

        if(line.find("user:") == 0) {

            //username follows user prefix
            commit.username = line.substr(5);

            if(!logf->getNextLine(line)) return false;

            commit.timestamp = atol(line.c_str());

            //this isnt a commit we are parsing, abort
            if(commit.timestamp == 0) return false;

            continue;
        }

        //should see username before files
        if(commit.username.size() == 0) return false;

        size_t tab = line.find('\t');

        //incorrect log format
        if(tab == std::string::npos || tab == 0 || tab == line.size()-1) continue;

        size_t space_before_tab = line.rfind(' ', tab);

        if(space_before_tab == std::string::npos) continue;
        
        std::string status = line.substr(space_before_tab+1, 1);                
        
        std::string file   = line.substr(tab + 1);

        if(file.empty()) continue;

        std::string rename_to;       
       
        if(status=="R") {
            size_t file_tab = file.find('\t');

            if(file_tab != std::string::npos) {
                rename_to = file.substr(file_tab+1);
                file      = file.substr(0, file_tab);
            }
            
            fprintf(stderr, "git rename: %s to %s\n", file.c_str(), rename_to.c_str());
        }

        //check for and remove double quotes in rename_to
        if(!rename_to.empty() && rename_to.find('"') == 0 && rename_to.rfind('"') == rename_to.size()-1) {
            if(rename_to.size()<=2) continue;

            rename_to = rename_to.substr(1,rename_to.size()-2);
        }

        //check for and remove double quotes
        if(file.find('"') == 0 && file.rfind('"') == file.size()-1) {
            if(file.size()<=2) continue;

            file = file.substr(1,file.size()-2);
        }

        if(status=="R") {
            if(rename_to.empty()) continue;
            commit.addFile(file, rename_to, status);
        } else {
            commit.addFile(file, status);
        }
    }

    //check we at least got a username
    if(commit.username.size()==0) return false;

    return true;
}
