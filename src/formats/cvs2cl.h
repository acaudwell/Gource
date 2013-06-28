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

#ifndef CVS2CL_H
#define CVS2CL_H

#include "commitlog.h"

class CVS2CLCommitLog : public RCommitLog {
protected:
    bool parseCommit(RCommit& commit);

    std::string logentry;
public:
    CVS2CLCommitLog(const std::string& logfile);

    static std::string logCommand();    
};

#endif
