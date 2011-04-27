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

#include "commitlog.h"

std::string munge_utf8(const std::string& str) {

    std::string munged;
    try {
        utf8::replace_invalid(str.begin(), str.end(), back_inserter(munged), '?');
    }
    catch(...) {
        munged = "???";
    }

    return munged;
}

//RCommitLog

RCommitLog::RCommitLog(const std::string& logfile, int firstChar) {

    logf     = 0;
    seekable = false;
    success  = false;
    is_dir   = false;
    buffered = false;

    if(logfile == "-") {

        //check first char
        if(checkFirstChar(firstChar, std::cin)) {
            logf     = new StreamLog();
            is_dir   = false;
            seekable = false;
            success  = true;
        }

        return;
    }

    struct stat fileinfo;
    int rc = stat(logfile.c_str(), &fileinfo);

    if(rc==0) {
        is_dir = (fileinfo.st_mode & S_IFDIR) ? true : false;

        if(!is_dir) {

            //check first char
            std::ifstream testf(logfile.c_str());

            bool firstOK = checkFirstChar(firstChar, testf);

            testf.close();

            if(firstOK) {
                logf = new SeekLog(logfile);
                seekable = true;
                success = true;
            }
        }
    }
}

RCommitLog::~RCommitLog() {
    if(logf!=0) delete logf;

    if(!temp_file.empty()) {
        remove(temp_file.c_str());
    }
}

//check firstChar of stream is as expected. if no firstChar defined just returns true.
bool RCommitLog::checkFirstChar(int firstChar, std::istream& stream) {

    //cant check this
    if(firstChar == -1) return true;

    int c = stream.peek();

    if(firstChar == c) return true;

    return false;
}

bool RCommitLog::checkFormat() {

    if(!success) return false;

    //read a commit to see if the log is in the correct format
    if(nextCommit(lastCommit)) {

        if(seekable) {
            //if the log is seekable, go back to the start
            ((SeekLog*)logf)->seekTo(0.0);
        } else {
            //otherwise set the buffered flag as we have bufferd one commit
            buffered = true;
        }

        return true;
    }

    return false;
}

std::string RCommitLog::getLogCommand() {
    return log_command;
}

bool RCommitLog::isSeekable() {
    return seekable;
}

bool RCommitLog::getCommitAt(float percent, RCommit& commit) {
    if(!seekable) return false;

    SeekLog* seeklog = ((SeekLog*)logf);

    //save settings
    long currpointer = seeklog->getPointer();
    std::string currlastline = lastline;

    seekTo(percent);
    bool success = findNextCommit(commit,500);

    //restore settings
    seeklog->setPointer(currpointer);
    lastline = currlastline;

    return success;
}

bool RCommitLog::getNextLine(std::string& line) {
    if(!lastline.empty()) {
        line = lastline;
        lastline = std::string("");
        return true;
    }

    return logf->getNextLine(line);
}


void RCommitLog::seekTo(float percent) {
    if(!seekable) return;

    lastline = "";

    ((SeekLog*)logf)->seekTo(percent);
}

float RCommitLog::getPercent() {
    if(seekable) return ((SeekLog*)logf)->getPercent();

    return 0.0;
}

bool RCommitLog::findNextCommit(RCommit& commit, int attempts) {

    for(int i=0;i<attempts;i++) {
        RCommit c;

        if(nextCommit(c)) {
            commit = c;
            return true;
        }
    }

    return false;
}

bool RCommitLog::nextCommit(RCommit& commit) {

    if(buffered) {
        commit = lastCommit;
        buffered = false;
        return true;
    }

    bool success = parseCommit(commit);

    if(!success) return false;

    return commit.isValid();
}

bool RCommitLog::isFinished() {
    if(seekable && logf->isFinished()) return true;

    return false;
}

//create temp file
void RCommitLog::createTempLog() {

    std::string tempdir;

#ifdef _WIN32
    DWORD tmplen = GetTempPath(0, "");

    if(tmplen == 0) return;

    std::vector<TCHAR> temp(tmplen+1);

    tmplen = GetTempPath(static_cast<DWORD>(temp.size()), &temp[0]);

    if(tmplen == 0 || tmplen >= temp.size()) return;

    tempdir = std::string(temp.begin(), temp.begin() + static_cast<std::size_t>(tmplen));
    tempdir += "\\";
#else
    tempdir = "/tmp/";
#endif

    char tmplate[1024];
    snprintf(tmplate, 1024, "%sgource-XXXXXX", tempdir.c_str());

#ifdef _WIN32
    if(mktemp(tmplate) < 0) return;
#else
    if(mkstemp(tmplate) < 0) return;
#endif

    temp_file = std::string(tmplate);
}

// RCommitFile

RCommitFile::RCommitFile(const std::string& filename, const std::string& action, const vec3f& colour)
    : action(action), colour(colour) {

    this->filename = munge_utf8(filename);

    //prepend a root slash
    if(this->filename[0] != '/') {
        this->filename.insert(0, 1, '/');
    }
}

RCommitFile::RCommitFile(const std::string& rename_from, const std::string& rename_to, const std::string& action, const vec3f& colour)
    : action(action), colour(colour) {

    this->filename  = munge_utf8(rename_from);
    this->rename_to = munge_utf8(rename_to);

    //prepend a root slash
    if(this->filename[0] != '/') {
        this->filename.insert(0, 1, '/');
    }

    if(this->rename_to[0] != '/') {
        this->rename_to.insert(0, 1, '/');
    }
}

//RCommit

RCommit::RCommit() {
    timestamp = 0;
}

vec3f RCommit::fileColour(const std::string& filename) {

    size_t slash = filename.rfind('/');
    size_t dot   = filename.rfind('.');

    if(dot != std::string::npos && dot+1<filename.size() && (slash == std::string::npos || slash < dot)) {
        std::string file_ext = filename.substr(dot+1);

        return colourHash(file_ext);
    } else {
        return vec3f(1.0, 1.0, 1.0);
    }
}

void RCommit::addFile(const std::string& filename, const std::string& action) {
    files.push_back(RCommitFile(filename, action, fileColour(filename)));
}

void RCommit::addFile(const std::string& filename, const  std::string& action, const vec3f& colour) {
    files.push_back(RCommitFile(filename, action, colour));
}

void RCommit::addFile(const std::string& rename_from, const std::string& renamed_to, const std::string& action) {
    files.push_back(RCommitFile(rename_from, renamed_to, action, fileColour(renamed_to)));
}

void RCommit::addFile(const std::string& rename_from, const std::string& renamed_to, const  std::string& action, const vec3f& colour) {
    files.push_back(RCommitFile(rename_from, renamed_to, action, colour));
}

bool RCommit::isValid() {

    username = munge_utf8(username);

    return true;
}

void RCommit::debug() {
    debugLog("files:\n");

    for(std::list<RCommitFile>::iterator it = files.begin(); it != files.end(); it++) {
        RCommitFile f = *it;
        debugLog("%s %s\n", f.action.c_str(), f.filename.c_str());
    }
}
