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

#include "cvs2cl.h"
#include "../gource_settings.h"

#ifdef HAVE_LIBTINYXML
#include <tinyxml.h>
#else
#include "../tinyxml/tinyxml.h"
#endif

Regex cvs2cl_xml_tag("^<\\??xml");
Regex cvs2cl_logentry_start("^<entry");
Regex cvs2cl_logentry_end("^</entry>");
Regex cvs2cl_logentry_timestamp("(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):(\\d{2})Z");

std::string CVS2CLCommitLog::logCommand() {
    std::string log_command = "cvs2cl --chrono --stdout --xml -g-q";
    return log_command;
}

CVS2CLCommitLog::CVS2CLCommitLog(const std::string& logfile) : RCommitLog(logfile, '<') {
}

bool CVS2CLCommitLog::parseCommit(RCommit& commit) {

    //fprintf(stderr,"parsing cvs2cl log\n");

    std::string line;

    if(!getNextLine(line)) return false;

    //start of log entry
    if(!cvs2cl_logentry_start.match(line)) {

        //is this the start of the document
        if(!cvs2cl_xml_tag.match(line)) return false;

        //fprintf(stderr,"found xml tag\n");

        //if so find the first logentry tag

        bool found_logentry = false;
        
        while(getNextLine(line)) {
            if(cvs2cl_logentry_start.match(line)) {
                found_logentry = true;
                break;
            }
        }

        if(!found_logentry) return false;   
    }

    //fprintf(stderr,"found logentry\n");

    logentry.clear();

    logentry.append(line);
    logentry.append("\n");

    //fprintf(stderr,"found opening tag\n");

    bool endfound = false;
    
    while(getNextLine(line)) {
        logentry.append(line);
        logentry.append("\n");
        if(cvs2cl_logentry_end.match(line)) {
            //fprintf(stderr,"found closing tag\n");
            endfound=true;
            break;
        }
    }

    //incomplete commit
    if(!endfound) return false;

    //fprintf(stderr,"read logentry\n");

    TiXmlDocument doc;

    if(!doc.Parse(logentry.c_str())) return false;

    //fprintf(stderr,"try to parse logentry: %s\n", logentry.c_str());
    
    TiXmlElement* leE = doc.FirstChildElement( "entry" );
    
    std::vector<std::string> entries;
    
    if(!leE) return false;

    //parse date
    TiXmlElement* dateE = leE->FirstChildElement( "isoDate" );

    if(!dateE) return false;

    std::string timestamp_str(dateE->GetText());

    if(!cvs2cl_logentry_timestamp.match(timestamp_str, &entries))
        return false;
                    
    struct tm time_str;

    time_str.tm_year  = atoi(entries[0].c_str()) - 1900;
    time_str.tm_mon   = atoi(entries[1].c_str()) - 1;
    time_str.tm_mday  = atoi(entries[2].c_str());
    time_str.tm_hour  = atoi(entries[3].c_str());
    time_str.tm_min   = atoi(entries[4].c_str());
    time_str.tm_sec   = atoi(entries[5].c_str());
    time_str.tm_isdst = -1;

    commit.timestamp = mktime(&time_str);            
   
    //parse author
    TiXmlElement* authorE = leE->FirstChildElement("author");
    
    if(authorE != 0) {
    
        std::string author(authorE->GetText());

        if(author.empty()) author = "Unknown";
    
        commit.username = author;
    }

    //parse changes
    
    for(TiXmlElement* fileE = leE->FirstChildElement("file"); fileE != 0; fileE = fileE->NextSiblingElement()) {
        
        TiXmlElement* state = fileE->FirstChildElement("cvsstate");
        TiXmlElement* name  = fileE->FirstChildElement("name");

        //check for state
        if(name == 0 || state == 0) continue;

        std::string status = strcmp(state->GetText(), "dead") == 0 ? "D" : "M";
        std::string file(name->GetText());

        if(file.empty()) continue;
        
        commit.addFile(file, status);
    }
    
    //fprintf(stderr,"parsed logentry\n");

    //read files
    
    return true;
}
