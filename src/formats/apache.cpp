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

#include "apache.h"
#include <time.h>

const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug" , "Sep", "Oct", "Nov", "Dec" };
Regex apache_entry_start("^(?:[^ ]+ )?([^ ]+) +[^ ]+ +([^ ]+) +\\[(.*?)\\] +(.*)$");
Regex apache_entry_date("(\\d+)/([A-Za-z]+)/(\\d+):(\\d+):(\\d+):(\\d+) ([+-])(\\d+)");
Regex apache_entry_request("\"([^ ]+) +([^ ]+) +([^ ]+)\" +([^ ]+) +([^\\s+]+)(.*)");
Regex apache_entry_agent(" +\"([^\"]+)\" +\"([^\"]+)\" +\"([^\"]+)\"");
Regex apache_hostname_parts("([^.]+)(?:\\.([^.]+))?(?:\\.([^.]+))?(?:\\.([^.]+))?(?:\\.([^.]+))?(?:\\.([^.]+))?(?:\\.([^.]+))?(?:\\.([^.]+))?$");

ApacheCombinedLog::ApacheCombinedLog(const std::string& logfile) : RCommitLog(logfile) {
}

//parse apache access.log entry into components
bool ApacheCombinedLog::parseCommit(RCommit& commit) {

    std::string line;
    std::vector<std::string> matches;

    if(!logf->getNextLine(line)) return false;

    apache_entry_start.match(line, &matches);

    if(matches.size()!=4) {
        return 0;
    }

    //get details
    commit.username = matches[0];

    std::string request_str = matches[3];
    std::string datestr     = matches[2];

    apache_entry_date.match(datestr, &matches);

    if(matches.size()!=8) {
        return 0;
    }

    //parse timestamp
    int day    = atoi(matches[0].c_str());
    int year   = atoi(matches[2].c_str());
    int hour   = atoi(matches[3].c_str());
    int minute = atoi(matches[4].c_str());
    int second = atoi(matches[5].c_str());

    int month=0;
    for(int i=0;i<12;i++) {
        if(matches[1] == months[i]) {
            month=i;
            break;
        }
    }

    struct tm time_str;
    time_str.tm_year = year - 1900;
    time_str.tm_mon  = month;
    time_str.tm_mday = day;
    time_str.tm_hour = hour;
    time_str.tm_min = minute;
    time_str.tm_sec = second;
    time_str.tm_isdst = -1;

    commit.timestamp = mktime(&time_str);

    matches.clear();
    apache_entry_request.match(request_str, &matches);

    if(matches.size() < 5) {
        return false;
    }

    std::string rtype = matches[0];
    std::string file  = matches[1];
    std::string proto = matches[2];

    int code      = atoi(matches[3].c_str());
    int bytes     = atol(matches[4].c_str());

    //remove args from url
    size_t argpos = file.rfind("?");
    if(argpos != std::string::npos) {
        file = file.substr(0,argpos);
    }

    if(file.size()==0) file = "/";

   //name index pages
    if(file[file.size()-1] == '/') {
        file += "index.html";
    }

    std::string action = "A";
    commit.addFile(file, action);

    std::string refer;
    std::string agent;

    if(matches.size() > 5) {
        std::string agentstr = matches[5];
        matches.clear();
        apache_entry_agent.match(agentstr, &matches);

        if(matches.size()>1) {
            refer     = matches[0];
            agent     = matches[1];
        }
    }

    return true;
}
