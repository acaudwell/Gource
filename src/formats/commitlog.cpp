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
#include "../gource_settings.h"
#include "../core/sdlapp.h"

#include "../core/utf8/utf8.h"

std::string RCommitLog::filter_utf8(const std::string& str) {

    std::string filtered;

    try {
        utf8::replace_invalid(str.begin(), str.end(), back_inserter(filtered), '?');
    }
    catch(...) {
        filtered = "???";
    }

    return filtered;
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

int RCommitLog::systemCommand(const std::string& command) {
    int rc = system(command.c_str());
    return rc;
}

// TODO: implement check for 'nix OSs
void RCommitLog::requireExecutable(const std::string& exename) {

#ifdef _WIN32
    TCHAR exePath[MAX_PATH];
    DWORD result = SearchPath(0, exename.c_str(), ".exe", MAX_PATH, exePath, 0);

    if(result) return;

    throw SDLAppException("unable to find %s.exe", exename.c_str());
#endif
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
    if(nextCommit(lastCommit, false)) {

        if(seekable) {
            //if the log is seekable, go back to the start
            ((SeekLog*)logf)->seekTo(0.0);
            lastline.clear();
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
        lastline.clear();
        return true;
    }

    return logf->getNextLine(line);
}


void RCommitLog::seekTo(float percent) {
    if(!seekable) return;

    lastline.clear();

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

void RCommitLog::bufferCommit(RCommit& commit) {
    lastCommit = commit;
    buffered = true;
}

bool RCommitLog::nextCommit(RCommit& commit, bool validate) {

    if(buffered) {
        commit = lastCommit;
        buffered = false;
        return true;
    }

    // ensure commit is re-initialized
    commit = RCommit();

    bool success = parseCommit(commit);

    if(!success) return false;

    commit.postprocess();

    if(validate) return commit.isValid();

    return true;
}

bool RCommitLog::isFinished() {
    if(seekable && logf->isFinished()) return true;

    return false;
}

bool RCommitLog::hasBufferedCommit() {
    return buffered;
}

//create temp file
bool RCommitLog::createTempLog() {
    return createTempFile(temp_file);
}

bool RCommitLog::createTempFile(std::string& temp_file) {

    std::string tempdir;

#ifdef _WIN32
    DWORD tmplen = GetTempPath(0, 0);

    if(tmplen == 0) return false;

    std::vector<TCHAR> temp(tmplen+1);

    tmplen = GetTempPath(static_cast<DWORD>(temp.size()), &temp[0]);

    if(tmplen == 0 || tmplen >= temp.size()) return false;

    tempdir = std::string(temp.begin(), temp.begin() + static_cast<std::size_t>(tmplen));
    tempdir += "\\";
#else
    tempdir = "/tmp/";
#endif

    char tmplate[1024];
    snprintf(tmplate, 1024, "%sgource-XXXXXX", tempdir.c_str());

#ifdef _WIN32
    if(mktemp(tmplate) < 0) return false;
#else
    if(mkstemp(tmplate) < 0) return false;
#endif

    temp_file = std::string(tmplate);

    return true;
}

// RCommitFile

RCommitFile::RCommitFile(const std::string& filename, const std::string& action, vec3 colour) {

    this->filename = RCommitLog::filter_utf8(filename);

    //prepend a root slash
    if(this->filename[0] != '/') {
        this->filename.insert(0, 1, '/');
    }

    this->action   = action;
    this->colour   = colour;
}

RCommit::RCommit() {
    timestamp = 0;
}

vec3 RCommit::fileColour(const std::string& filename) {

    size_t slash = filename.rfind('/');
    size_t dot   = filename.rfind('.');

    if(dot != std::string::npos && dot+1<filename.size() && (slash == std::string::npos || slash < dot)) {
        std::string file_ext = filename.substr(dot+1);

        return colourHash(file_ext);
    } else {
        return vec3(1.0, 1.0, 1.0);
    }
}

void RCommit::addFile(const std::string& filename, const std::string& action) {
    addFile(filename, action, fileColour(filename));
}

void RCommit::addFile(const std::string& filename, const  std::string& action, const vec3& colour) {
    //check filename against filters
    if(!gGourceSettings.file_filters.empty()) {

        for(std::vector<Regex*>::iterator ri = gGourceSettings.file_filters.begin(); ri != gGourceSettings.file_filters.end(); ri++) {
            Regex* r = *ri;

            if(r->match(filename)) {
                return;
            }
        }
    }

    // Only allow files that have been whitelisted
    if(!gGourceSettings.file_show_filters.empty()) {

        for(std::vector<Regex*>::iterator ri = gGourceSettings.file_show_filters.begin(); ri != gGourceSettings.file_show_filters.end(); ri++) {
            Regex* r = *ri;

            if(!r->match(filename)) {
                return;
            }
        }
    }

    files.push_back(RCommitFile(filename, action, colour));
}

void RCommit::postprocess() {
    username = RCommitLog::filter_utf8(username);
}

bool RCommit::isValid() {

    //check user against filters, if found, discard commit
    if(!gGourceSettings.user_filters.empty()) {
        for(std::vector<Regex*>::iterator ri = gGourceSettings.user_filters.begin(); ri != gGourceSettings.user_filters.end(); ri++) {
            Regex* r = *ri;

            if(r->match(username)) {
                return false;
            }
        }
    }

    // Only allow users that have been whitelisted
    if(!gGourceSettings.user_show_filters.empty()) {

        for(std::vector<Regex*>::iterator ri = gGourceSettings.user_show_filters.begin(); ri != gGourceSettings.user_show_filters.end(); ri++) {
            Regex* r = *ri;

            if(!r->match(username)) {
                return false;
            }
        }
    }


    return !files.empty();
}

void RCommit::debug() {
    debugLog("files:\n");

    for(std::list<RCommitFile>::iterator it = files.begin(); it != files.end(); it++) {
        RCommitFile f = *it;
        debugLog("%s %s\n", f.action.c_str(), f.filename.c_str());
    }
}
