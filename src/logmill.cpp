/*
    Copyright (C) 2012 Andrew Caudwell (acaudwell@gmail.com)

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

#include "logmill.h"
#include "gource_settings.h"

#include "formats/git.h"
#include "formats/gitraw.h"
#include "formats/custom.h"
#include "formats/hg.h"
#include "formats/bzr.h"
#include "formats/svn.h"
#include "formats/apache.h"

extern "C" {

    static int logmill_thread(void *lmill) {

        RLogMill *logmill = static_cast<RLogMill*> (lmill);
        logmill->run();

        return 0;
    }

};

RLogMill::RLogMill(const std::string& logfile)
    : logfile(logfile) {

    logmill_thread_state = LOGMILL_STATE_STARTUP;
    clog = 0;

#if SDL_VERSION_ATLEAST(1,3,0)
    thread = SDL_CreateThread( logmill_thread, "logmill", this );
#else
    thread = SDL_CreateThread( logmill_thread, this );
#endif
}

RLogMill::~RLogMill() {

    abort();

    if(thread) SDL_KillThread(thread);
    if(clog) delete clog;
}

void RLogMill::run() {
    logmill_thread_state = LOGMILL_STATE_FETCHING;

    try {

        clog = fetchLog();

    } catch(SeekLogException& exception) {
        error = "unable to read log file";
    } catch(SDLAppException& exception) {
        error = exception.what();
    }

    if(!clog && error.empty()) {
        if(SDLAppDirExists(logfile)) {
            error = "directory not supported";
        } else {
            error = "unsupported log format (you may need to regenerate your log file)";
        }
    }

    logmill_thread_state = clog ? LOGMILL_STATE_SUCCESS : LOGMILL_STATE_FAILURE;
}

void RLogMill::abort() {
    // TODO: make abort nicer by notifying the log process
    //       we want to shutdown

    while(logmill_thread_state <= LOGMILL_STATE_FETCHING) {
        SDL_Delay(100);
    }
}

bool RLogMill::isFinished() {
    return logmill_thread_state > LOGMILL_STATE_FETCHING;
}

int RLogMill::getStatus() {
    return logmill_thread_state;
}

std::string RLogMill::getError() {
    return error;
}


RCommitLog* RLogMill::getLog() {

    while(logmill_thread_state <= LOGMILL_STATE_FETCHING) {
        SDL_Delay(100);
    }

    return clog;
}

bool RLogMill::findRepository(boost::filesystem::path& dir, std::string& log_format) {

    dir = absolute(dir);

    //fprintf(stderr, "find repository from initial path: %s\n", dir.string().c_str());

    while(is_directory(dir)) {

             if(is_directory(dir / ".git")) log_format = "git";
        else if(is_directory(dir / ".hg"))  log_format = "hg";
        else if(is_directory(dir / ".bzr")) log_format = "bzr";
        else if(is_directory(dir / ".svn")) log_format = "svn";

        if(!log_format.empty()) {
            //fprintf(stderr, "found '%s' repository at: %s\n", log_format.c_str(), dir.string().c_str());
            return true;
        }

        if(!dir.has_parent_path()) return false;

        dir = dir.parent_path();
    }

    return false;
}


RCommitLog* RLogMill::fetchLog() {

    RCommitLog* clog = 0;

    std::string log_format = gGourceSettings.log_format;

    //if the log format is not specified and 'logfile' is a directory, recursively look for a version control repository.
    //this method allows for something strange like someone who having an svn repository inside a git repository
    //(in which case it would pick the svn directory as it would encounter that first)

    if(log_format.empty() && logfile != "-") {

        try {
            boost::filesystem::path repo_path(logfile);

            if(is_directory(repo_path)) {
                if(findRepository(repo_path, log_format)) {
                    logfile = repo_path.string();
                }
            }
        } catch(boost::filesystem3::filesystem_error& error) {
        }
    }

    //we've been told what format to use
    if(log_format.size() > 0) {
        debugLog("log-format = %s", log_format.c_str());

        if(log_format == "git") {
            clog = new GitCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;

            clog = new GitRawCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "hg") {
            clog = new MercurialLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "bzr") {
            clog = new BazaarLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "cvs") {
            clog = new CVSEXPCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "custom") {
            clog = new CustomLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "apache") {
            clog = new ApacheCombinedLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "svn") {
            clog = new SVNCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        if(log_format == "cvs2cl") {
            clog = new CVS2CLCommitLog(logfile);
            if(clog->checkFormat()) return clog;
            delete clog;
        }

        return 0;
    }

    // try different formats until one works

    //git
    debugLog("trying git...");
    clog = new GitCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //mercurial
    debugLog("trying mercurial...");
    clog = new MercurialLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //bzr
    debugLog("trying bzr...");
    clog = new BazaarLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //git raw
    debugLog("trying git raw...");
    clog = new GitRawCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //cvs exp
    debugLog("trying cvs-exp...");
    clog = new CVSEXPCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //svn
    debugLog("trying svn...");
    clog = new SVNCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //cvs2cl
    debugLog("trying cvs2cl...");
    clog = new CVS2CLCommitLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //custom
    debugLog("trying custom...");
    clog = new CustomLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    //apache
    debugLog("trying apache combined...");
    clog = new ApacheCombinedLog(logfile);
    if(clog->checkFormat()) return clog;

    delete clog;

    return 0;
}
