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

#ifndef RCOMMIT_LOG_H
#define RCOMMIT_LOG_H


#include "core/seeklog.h"
#include "core/display.h"
#include "core/regex.h"
#include "core/stringhash.h"
#include "core/utf8/utf8.h"

#include <time.h>
#include <string>
#include <list>

#include "sys/stat.h"

class RCommitFile {
public:
    std::string filename;
    std::string rename_to;
    std::string action;
    vec3f colour;

    RCommitFile(const std::string& filename, const std::string& action,  const vec3f& colour);
    RCommitFile(const std::string& rename_from, const std::string& rename_to, const std::string& action, const vec3f& colour);
};

class RCommit {
    vec3f fileColour(const std::string& filename);
public:
    time_t timestamp;
    std::string username;

    std::list<RCommitFile> files;

    bool isValid();

    void addFile(const std::string& filename, const std::string& action);
    void addFile(const std::string& filename, const std::string& action,  const vec3f& colour);

    void addFile(const std::string& rename_from, const std::string& renamed_to, const std::string& action);
    void addFile(const std::string& rename_from, const std::string& renamed_to, const std::string& action, const vec3f& colour);

    RCommit();
    void debug();
    virtual bool parse(BaseLog* logf) { return false; };
};

class RCommitLog {
protected:
    BaseLog* logf;

    std::string temp_file;
    std::string log_command;

    std::string lastline;

    bool is_dir;
    bool success;
    bool seekable;

    RCommit lastCommit;
    bool buffered;

    bool checkFirstChar(int firstChar, std::istream& stream);

    void createTempLog();

    bool getNextLine(std::string& line);

    virtual bool parseCommit(RCommit& commit) { return false; };
public:
    RCommitLog(const std::string& logfile, int firstChar = -1);
    ~RCommitLog();

    void seekTo(float percent);

    bool checkFormat();

    std::string getLogCommand();

    bool getCommitAt(float percent, RCommit& commit);
    bool findNextCommit(RCommit& commit, int attempts);
    bool nextCommit(RCommit& commit);
    bool isFinished();
    bool isSeekable();
    float getPercent();
};

#endif
