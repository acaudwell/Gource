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


#include "../core/seeklog.h"
#include "../core/display.h"
#include "../core/regex.h"
#include "../core/stringhash.h"

#include <time.h>
#include <string>
#include <list>

#include "sys/stat.h"

class RCommitFile {
public:
    std::string filename;
    std::string action;
    vec3 colour;
    long lines;

    RCommitFile(const std::string& filename, const std::string& action, vec3 colour);
};

class RCommit {
    vec3 fileColour(const std::string& filename);
public:
    time_t timestamp;
    std::string username;

    std::list<RCommitFile> files;

    void postprocess();
    bool isValid();

    void addFile(const std::string& filename, const std::string& action);
    void addFile(const std::string& filename, const std::string& action, const vec3& colour);

    void addLines(const std::string& filename, long delta);

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
    virtual ~RCommitLog();

    static std::string filter_utf8(const std::string& str);

    void seekTo(float percent);

    bool checkFormat();

    std::string getLogCommand();

    int systemCommand(const std::string& command);
    void requireExecutable(const std::string& exename);

    void bufferCommit(RCommit& commit);

    bool getCommitAt(float percent, RCommit& commit);
    bool findNextCommit(RCommit& commit, int attempts);
    bool nextCommit(RCommit& commit, bool validate = true);
    bool hasBufferedCommit();
    bool isFinished();
    bool isSeekable();
    float getPercent();
};

#endif
