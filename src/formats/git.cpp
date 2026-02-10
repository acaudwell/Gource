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
#include "../gource_settings.h"
#include "../core/logger.h"
#include "../core/sdlapp.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <limits>
#include <sstream>

#ifndef _MSC_VER
#include <unistd.h>
#endif

// parse git log entries

//git-log command notes:
// - no single quotes on WIN32 as system call treats them differently
// - 'user:' prefix allows us to quickly tell if the log is the wrong format
//   and try a different format (eg cvs-exp)

int git_version_major = 0;
int git_version_minor = 0;
int git_version_patch = 0;

Regex git_version_regex("([0-9]+)(?:\\.([0-9]+))?(?:\\.([0-9]+))?");

namespace {

bool parseRawGitLine(const std::string& line,
                     std::string& status,
                     std::string& filename,
                     std::string& dst_blob,
                     bool& has_blob_hash) {
    if(line.empty() || line[0] != ':') return false;

    size_t tab = line.find('\t');
    if(tab == std::string::npos || tab == line.size()-1) return false;

    std::string header = line.substr(1, tab-1);
    filename = line.substr(tab + 1);
    if(filename.empty()) return false;

    std::istringstream header_stream(header);
    std::vector<std::string> fields;
    std::string field;
    while(header_stream >> field) fields.push_back(field);

    if(fields.empty()) return false;

    if(fields.size() >= 5) {
        status = fields[4];
    } else {
        status = fields.back();
    }

    if(status.empty()) return false;
    status = status.substr(0,1);

    if(fields.size() >= 4) {
        dst_blob = fields[3];
        has_blob_hash = true;
    } else {
        dst_blob.clear();
        has_blob_hash = false;
    }

    return true;
}

}

void GitCommitLog::readGitVersion() {
    if(git_version_major != 0) return;

    std::string temp_file;
    if(!createTempFile(temp_file)) {
        return;
    }

    char cmd_buff[2048];
    int result = snprintf(cmd_buff, sizeof(cmd_buff), "git --version > %s", temp_file.c_str());

    if(result < 0 || result >= sizeof(cmd_buff)) {
        remove(temp_file.c_str());
        return;
    }

    int command_rc = systemCommand(cmd_buff);

    if(command_rc != 0) {
        remove(temp_file.c_str());
        return;
    }

    std::ifstream in(temp_file.c_str());

    if(!in.is_open()) {
        remove(temp_file.c_str());
        return;
    }

    std::string version_str;
    std::getline(in, version_str);
    in.close();

    remove(temp_file.c_str());

    std::vector<std::string> entries;
    if(!git_version_regex.match(version_str, &entries)) return;

    git_version_major = atoi(entries[0].c_str());

    if(entries.size() > 1) {
        git_version_minor = atoi(entries[1].c_str());
    }

    if(entries.size() > 2) {
        git_version_patch = atoi(entries[2].c_str());
    }
}

std::string GitCommitLog::logCommand() {

    std::string log_command = "git log "
    "--reverse --raw --encoding=UTF-8 "
    "--no-renames --abbrev=40";

    readGitVersion();

    // Add --no-show-signature either
    // if git version couldn't be determined or if version
    // is at least 2.10
    if(   git_version_major == 0
       || git_version_major > 2
       || (git_version_major == 2 && git_version_minor >= 10))
    {
        log_command += " --no-show-signature";
    }

    if(gGourceSettings.author_time) {
        log_command += " --pretty=format:user:%aN%n%at%n%H";
    } else {
        log_command += " --pretty=format:user:%aN%n%ct%n%H";
    }

    if(!gGourceSettings.start_date.empty()) {
        log_command += " --since ";
        log_command += gGourceSettings.start_date;
    }

    if(!gGourceSettings.stop_date.empty()) {
        log_command += " --until ";
        log_command += gGourceSettings.stop_date;
    }

    if(!gGourceSettings.git_branch.empty()) {
        log_command += " ";
        log_command += gGourceSettings.git_branch;
    }

    return log_command;
}

GitCommitLog::GitCommitLog(const std::string& logfile)
    : RCommitLog(logfile, 'u'),
      m_repository_path(is_dir ? getLogfilePath() : std::string("")),
      m_blob_sizes(),
      m_blob_index_ready(false),
      m_warned_missing_blob_size(false) {

    log_command = logCommand();

    //can generate log from directory
    if(!logf && is_dir) {
        logf = generateLog(logfile);

        if(logf) {
            success  = true;
            seekable = true;

            if(gGourceSettings.scale_by_file_size) {
                m_blob_index_ready = buildBlobSizeIndex(logfile);
                if(!m_blob_index_ready) {
                    warnLog("failed to build git blob-size index for %s; file-size scaling may be unavailable", logfile.c_str());
                }
            }
        }
    }
}

bool GitCommitLog::buildBlobSizeIndex(const std::string& dir) {
    m_blob_sizes.clear();

    std::string temp_blob_file;
    if(!createTempFile(temp_blob_file)) return false;

    char cwd_buff[1024];
    if(getcwd(cwd_buff, 1024) != cwd_buff) {
        remove(temp_blob_file.c_str());
        return false;
    }

    if(chdir(dir.c_str()) != 0) {
        remove(temp_blob_file.c_str());
        return false;
    }

    char cmd_buff[2048];
    int written = snprintf(cmd_buff,
                           sizeof(cmd_buff),
                           "git cat-file --batch-all-objects --batch-check='%%(objectname) %%(objecttype) %%(objectsize)' > %s",
                           temp_blob_file.c_str());

    int command_rc = -1;
    if(written > 0 && written < (int)sizeof(cmd_buff)) {
        command_rc = systemCommand(cmd_buff);
    }

    int chdir_rc = chdir(cwd_buff);
    if(chdir_rc != 0) {
        remove(temp_blob_file.c_str());
        return false;
    }

    if(command_rc != 0) {
        remove(temp_blob_file.c_str());
        return false;
    }

    std::ifstream in(temp_blob_file.c_str());
    if(!in.is_open()) {
        remove(temp_blob_file.c_str());
        return false;
    }

    std::string line;
    while(std::getline(in, line)) {
        std::istringstream ss(line);
        std::string hash;
        std::string type;
        std::string size_str;

        if(!(ss >> hash >> type >> size_str)) continue;
        if(type != "blob") continue;

        unsigned long long blob_size = strtoull(size_str.c_str(), 0, 10);
        if(blob_size > std::numeric_limits<unsigned int>::max()) {
            blob_size = std::numeric_limits<unsigned int>::max();
        }

        m_blob_sizes[hash] = (unsigned int)blob_size;
    }

    in.close();
    remove(temp_blob_file.c_str());

    return true;
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
    if(stat_rc!=0 || !(dirinfo.st_mode & S_IFDIR || dirinfo.st_mode & S_IFREG)) {
        return 0;
    }

    // do we have this client installed
    requireExecutable("git");

    std::string command = getLogCommand();

    //create temp file
    createTempLog();

    if(temp_file.size()==0) return 0;

    if(chdir(dir.c_str()) != 0) {
        return 0;
    }

    char cmd_buff[2048];
    int written = snprintf(cmd_buff, 2048, "%s > %s", command.c_str(), temp_file.c_str());

    if(written < 0 || written >= 2048) {
        int restore_rc = chdir(cwd_buff);
        (void)restore_rc;
        return 0;
    }

    int command_rc = systemCommand(cmd_buff);

    //change back to original directory
    int chdir_rc = chdir(cwd_buff);
    if(chdir_rc != 0) {
        return 0;
    }

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

            if(!logf->getNextLine(line)) return false;
            commit.commit_hash = line;

            continue;
        }

        //should see username before files
        if(commit.username.empty()) return false;

        if (line[0] == ':') {
            std::string status;
            std::string filename;
            std::string dst_blob;
            bool has_blob_hash = false;

            if(!parseRawGitLine(line, status, filename, dst_blob, has_blob_hash)) {
                if(gGourceSettings.scale_by_file_size) {
                    throw SDLAppException("git raw log line is missing blob hash metadata required for --scale-by-file-size");
                }
                continue;
            }

            unsigned int file_size = 0;
            if(gGourceSettings.scale_by_file_size && status != "D") {
                if(!has_blob_hash || dst_blob.empty() || dst_blob.find_first_not_of('0') == std::string::npos) {
                    throw SDLAppException("git raw log line is missing destination blob hash required for --scale-by-file-size");
                }

                if(!m_blob_index_ready) {
                    throw SDLAppException("--scale-by-file-size requires a Git repository path so blob sizes can be indexed");
                }

                std::unordered_map<std::string, unsigned int>::const_iterator sit = m_blob_sizes.find(dst_blob);
                if(sit != m_blob_sizes.end()) {
                    file_size = sit->second;
                } else if(!m_warned_missing_blob_size) {
                    warnLog("missing blob size for object %s in %s, defaulting file size to 0",
                            dst_blob.c_str(),
                            m_repository_path.c_str());
                    m_warned_missing_blob_size = true;
                }
            }

            commit.addFile(filename, status, file_size);
        } else {
            size_t tab = line.find('\t');

            //incorrect log format
            if(tab == std::string::npos || tab == 0 || tab == line.size()-1) continue;

            std::string status = line.substr(tab - 1, 1);
            std::string file   = line.substr(tab + 1);

            if(file.empty()) continue;

            //check for and remove double quotes
            if(file.find('"') == 0 && file.rfind('"') == file.size()-1) {
                if(file.size()<=2) continue;

                file = file.substr(1,file.size()-2);
            }

            commit.addFile(file, status);
        }
    }

    //check we at least got a username
    if(commit.username.empty()) return false;

    return true;
}
