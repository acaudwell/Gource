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

#include "custom.h"
#include "../gource_settings.h"
#include <boost/algorithm/string.hpp>

CustomLog::CustomLog(const std::string& logfile) : RCommitLog(logfile) {
}

vec3 CustomLog::parseColour(const std::string& cstr) {

    vec3 colour;
    int r,g,b;

    if(sscanf(cstr.c_str(), "%02x%02x%02x", &r, &g, &b) == 3) {
        colour = vec3( r, g, b );
        colour /= 255.0f;
    }

    return colour;
}

// parse modified cvs format log entries

bool CustomLog::parseCommit(RCommit& commit) {

    while(parseCommitEntry(commit));

    return !commit.files.empty();
}

bool CustomLog::parseCommitEntry(RCommit& commit) {

    std::string line;
    if(!getNextLine(line)) return false;

    std::vector<std::string> parts;
    boost::split(parts, line, boost::is_any_of("|"));

    if (parts.size() < 4) return false;

    time_t timestamp;
    if(parts[0].size() > 1 && parts[0].find("-", 1) != std::string::npos) {
        if(!SDLAppSettings::parseDateTime(parts[0], timestamp))
            return false;
    } else {
        timestamp = (time_t) atoll(parts[0].c_str());
        if(!timestamp && parts[0] != "0")
            return false;
    }

    std::string username = (parts[1].size()>0) ? parts[1] : "Unknown";
    std::string action   = (parts[2].size()>0) ? parts[2] : "A";

    //if this file is for the same person and timestamp
    //we add to the commit, else we save the lastline
    //and return false
    if(commit.files.empty()) {
        commit.timestamp = timestamp;
        commit.username  = username;
    } else {
        if(commit.timestamp != timestamp || commit.username  != username) {
            lastline = line;
            return false;
        }
    }

    bool has_colour = false;
    vec3 colour;
    
    if (parts.size() >= 5 && parts[4].size() > 0) {
        has_colour = true;
        colour = parseColour(parts[4]);
    }

    unsigned int file_size = 0;
    if ((action == "A" || action == "M") && parts.size() >= 6 && parts[5].size() > 0) {
        file_size = std::stoul(parts[5]);
    }

    if(has_colour) {
        commit.addFile(parts[3], action, colour, file_size);
    } else {
        commit.addFile(parts[3], action, file_size);
    }

    return true;
}
