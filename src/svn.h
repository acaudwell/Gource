/*
    Copyright (C) 2010 Andrew Caudwell (acaudwell@gmail.com)

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

#ifndef SVNLOG_H
#define SVNLOG_H

#include "gource_settings.h"

#include "commitlog.h"

#include <sstream>
#include <set>

#ifdef HAVE_LIBTINYXML
#include <tinyxml.h>
#else
#include "tinyxml/tinyxml.h"
#endif

#include <unistd.h>

extern std::string gGourceSVNLogCommand;

class SVNCommitLog : public RCommitLog {
protected:
    bool parseCommit(RCommit& commit);
    BaseLog* generateLog(const std::string& dir);

    std::string logentry;
public:
    SVNCommitLog(const std::string& logfile);
};

#endif
