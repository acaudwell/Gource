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

#include "conffile.h"

//section of config file
Regex ConfFile_section("^\\s*\\[([^\\]]+)\\]\\s*$");

// parse key value pair, seperated by an equals sign, removing white space on key and front of the value
Regex ConfFile_key_value("^\\s*([^=\\s]+)\\s*=\\s*([^\\s].*)?$");

// vec2f, vec3f, or vec4f with liberal allowance for whitespace
Regex ConfFile_vec2_value("^\\s*vec2\\(\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*\\)\\s*$");
Regex ConfFile_vec3_value("^\\s*vec3\\(\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*\\)\\s*$");
Regex ConfFile_vec4_value("^\\s*vec4\\(\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*,\\s*(-?[0-9.]+)\\s*\\)\\s*$");


//ConfEntry

ConfEntry::ConfEntry(const std::string& name) {
    this->name = name;
}

ConfEntry::ConfEntry(const std::string& name, const std::string& value, int lineno) {
    this->name  = name;
    this->value = value;
    this->lineno = lineno;
}

ConfEntry::ConfEntry(const std::string& name, bool value) {
    this->name  = name;
    setBool(value);
}

ConfEntry::ConfEntry(const std::string& name, int value) {
    this->name  = name;
    setInt(value);
}

ConfEntry::ConfEntry(const std::string& name, float value) {
    this->name  = name;
    setFloat(value);
}

ConfEntry::ConfEntry(const std::string& name, vec2f value) {
    this->name  = name;
    setVec2(value);
}

ConfEntry::ConfEntry(const std::string& name, vec3f value) {
    this->name  = name;
    setVec3(value);
}

ConfEntry::ConfEntry(const std::string& name, vec4f value) {
    this->name  = name;
    setVec4(value);
}

void ConfEntry::setName(const std::string& name) {
    this->name = name;
}

void ConfEntry::setString(const std::string& value) {
    this->value = value;
}

void ConfEntry::setFloat(float value) {
    char floattostr[256];
    sprintf(floattostr, "%.5f", value);

    this->value = std::string(floattostr);
}

void ConfEntry::setInt(int value) {
    char inttostr[256];
    sprintf(inttostr, "%d", value);

    this->value = std::string(inttostr);
}

void ConfEntry::setBool(bool value) {
    this->value = std::string(value ? "yes" : "no");
}

void ConfEntry::setVec2(vec2f value) {
    char vectostr[256];
    sprintf(vectostr, "vec2(%.5f, %.5f)", value.x, value.y);

    this->value = std::string(vectostr);
}

void ConfEntry::setVec3(vec3f value) {
    char vectostr[256];
    sprintf(vectostr, "vec3(%.5f, %.5f, %.5f)", value.x, value.y, value.z);

    this->value = std::string(vectostr);
}

void ConfEntry::setVec4(vec4f value) {
    char vectostr[256];
    sprintf(vectostr, "vec4(%.5f, %.5f, %.5f, %.5f)", value.x, value.y, value.z, value.w);

    this->value = std::string(vectostr);
}

std::string ConfEntry::getName() {
    return name;
}

std::string ConfEntry::getString() {
    return value;
}

int ConfEntry::getLineNumber() {
    return lineno;
}

int ConfEntry::getInt() {
    return atoi(value.c_str());
}

float ConfEntry::getFloat() {
    return atof(value.c_str());
}

bool ConfEntry::hasValue() {
    return getString().size() > 0;
}

bool ConfEntry::getBool() {

    if(value == "1" || value == "true" || value == "True" || value == "TRUE" || value == "yes" || value == "Yes" || value == "YES")
        return true;

    return false;
}

vec2f ConfEntry::getVec2() {

    std::vector<std::string> matches;

    if(ConfFile_vec2_value.match(value, &matches)) {
        return vec2f(atof(matches[0].c_str()), atof(matches[1].c_str()));
    }

    debugLog("'%s' did not match vec2 regex\n", value.c_str());

    return vec2f(0.0, 0.0);
}

vec3f ConfEntry::getVec3() {

    std::vector<std::string> matches;

    if(ConfFile_vec3_value.match(value, &matches)) {
        return vec3f(atof(matches[0].c_str()), atof(matches[1].c_str()), atof(matches[2].c_str()));
    }

    debugLog("'%s' did not match vec3 regex\n", value.c_str());

    return vec3f(0.0, 0.0, 0.0);
}


vec4f ConfEntry::getVec4() {

    std::vector<std::string> matches;

    if(ConfFile_vec4_value.match(value, &matches)) {
        return vec4f(atof(matches[0].c_str()), atof(matches[1].c_str()), atof(matches[2].c_str()), atof(matches[3].c_str()) );
    }

    debugLog("'%s' did not match vec4 regex\n", value.c_str());

    return vec4f(0.0, 0.0, 0.0, 0.0);
}

//ConfSection

ConfSection::ConfSection() {
}

ConfSection::ConfSection(const std::string& name) {
    this->name = name;
}

ConfSection::~ConfSection() {
    clear();
}

std::string ConfSection::getName() {
    return name;
}

ConfEntryList* ConfSection::getEntries(const std::string& key) {
    std::map<std::string, ConfEntryList*>::iterator entry_finder = entrymap.find(key);

    if(entry_finder == entrymap.end()) return 0;

    return entry_finder->second;
}

ConfEntry* ConfSection::getEntry(const std::string& key) {
    ConfEntryList* entryList = getEntries(key);

    if(entryList==0 || entryList->size()==0) return 0;

    return entryList->front();
}

void ConfSection::addEntry(ConfEntry* entry) {

    ConfEntryList* entrylist = entrymap[entry->getName()];

    if(entrylist==0) {
        entrymap[entry->getName()] = entrylist = new ConfEntryList;
    }

    entrylist->push_back(entry);
}

void ConfSection::addEntry(const std::string& name, const std::string& value, int lineno) {

    ConfEntry* entry = new ConfEntry(name, value, lineno);
    addEntry(entry);
}

//replace first entry with that name
void ConfSection::setEntry(ConfEntry* entry) {

    ConfEntryList* entrylist = entrymap[entry->getName()];

    if(entrylist==0) {
        entrymap[entry->getName()] = entrylist = new ConfEntryList;
    }

    //remove any entries with this name
    while(entrylist->size()>0) {
        ConfEntry* front = entrylist->front();
        entrylist->pop_front();
        delete front;
    }

    //add new entry
    entrylist->push_front(entry);
}

void ConfSection::setEntry(const std::string& name, const std::string& value, int lineno) {

    ConfEntry* entry = new ConfEntry(name, value, lineno);
    setEntry(entry);
}

void ConfSection::clear() {

    //delete entries
    for(std::map<std::string, ConfEntryList*>::iterator it = entrymap.begin();
        it!= entrymap.end(); it++) {

        ConfEntryList* entrylist = it->second;

        for(std::list<ConfEntry*>::iterator eit = entrylist->begin();
            eit != entrylist->end(); eit++) {

            ConfEntry* e = *eit;
            delete e;
        }

        delete entrylist;
    }

    entrymap.clear();
}

bool ConfSection::hasValue(const std::string& key) {
    std::string value = getString(key);

    if(value.size()>0) return true;

    return false;
}

std::string ConfSection::getString(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry==0) return std::string("");

    return entry->getString();
}

int ConfSection::getInt(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry) return entry->getInt();

    return 0;
}

float ConfSection::getFloat(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry) return entry->getFloat();

    return 0.0f;
}

bool ConfSection::getBool(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry) return entry->getBool();

    return false;
}

vec3f ConfSection::getVec3(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry) return entry->getVec3();

    return vec3f(0.0, 0.0, 0.0);
}

vec4f ConfSection::getVec4(const std::string& key) {
    ConfEntry* entry = getEntry(key);

    if(entry) return entry->getVec4();

    return vec4f(0.0, 0.0, 0.0, 0.0);
}

void ConfSection::print(std::ostream& out) {

    out << "[" << getName() << "]" << std::endl;

    for(std::map<std::string, ConfEntryList*>::iterator it = entrymap.begin();
        it!= entrymap.end(); it++) {

        ConfEntryList* entrylist = it->second;

        for(std::list<ConfEntry*>::iterator eit = entrylist->begin();
            eit != entrylist->end(); eit++) {
            ConfEntry* e = *eit;
            out << e->getName() << "=" << e->getString() << std::endl;
        }
   }
   out << std::endl;
}

//ConfFile

ConfFile::ConfFile() {

}

ConfFile::~ConfFile() {
    clear();
}

void ConfFile::clear() {

    //delete sections
    for(std::map<std::string, ConfSectionList*>::iterator it = sectionmap.begin();
        it!= sectionmap.end(); it++) {

        ConfSectionList* sectionlist = it->second;

        for(std::list<ConfSection*>::iterator sit = sectionlist->begin();
            sit != sectionlist->end(); sit++) {

            ConfSection* s = *sit;
            delete s;
        }

        delete sectionlist;
    }

    sectionmap.clear();
}

void ConfFile::setFilename(const std::string& filename) {
    this->conffile = filename;
}

std::string ConfFile::getFilename() {
    return conffile;
}

void ConfFile::save(const std::string& conffile) {
    this->conffile = conffile;
    save();
}

void ConfFile::save() {
    if(conffile.size()==0) {
        throw ConfFileException("filename not set", conffile.c_str(), 0);
    }

    //save conf file
    std::ofstream out;
    out.open(conffile.c_str());

    if(!out.is_open()) {
        std::string write_error = std::string("failed to write config to ") + conffile;
        throw ConfFileException(write_error, conffile.c_str(), 0);
    }


    for(std::map<std::string, ConfSectionList*>::iterator it = sectionmap.begin();
        it!= sectionmap.end(); it++) {

        ConfSectionList* sectionlist = it->second;

        for(ConfSectionList::iterator sit = sectionlist->begin();
            sit != sectionlist->end(); sit++) {

            ConfSection* s = *sit;

            s->print(out);
        }
    }

    out.close();
}

void ConfFile::load(const std::string& conffile) {
    this->conffile = conffile;
    load();
}

void ConfFile::load() {
    debugLog("ConfFile::load(%s)\n", conffile.c_str());

    clear();

    char buff[1024];

    int lineno = 0;
    ConfSection* sec = 0;

    std::ifstream in(conffile.c_str());

    if(!in.is_open()) {
        sprintf(buff, "failed to open config file %s", conffile.c_str());
        std::string conf_error = std::string(buff);

        throw ConfFileException(conf_error, conffile, 0);
    }

    std::string whitespaces (" \t\f\v\n\r");
    std::string line;

    while(std::getline(in, line)) {

        lineno++;

        std::vector<std::string> matches;

        // blank line or commented out lines
        if(line.size() == 0 || (line.size() > 0 && line[0] == '#')) {

            continue;

        // sections
        } else if(ConfFile_section.match(line, &matches)) {

            if(sec != 0) addSection(sec);

            sec = new ConfSection(matches[0]);

        // key value pairs
        } else if(ConfFile_key_value.match(line, &matches)) {

            std::string key   = matches[0];
            std::string value = (matches.size()>1) ? matches[1] : "";

            //trim whitespace
            if(value.size()>0) {
                size_t string_end = value.find_last_not_of(whitespaces);

                if(string_end == std::string::npos) value = "";
                else if(string_end != value.size()-1) value = value.substr(0,string_end+1);
            }

            if(sec==0) sec = new ConfSection("");

            sec->addEntry(key, value, lineno);

            debugLog("%s: [%s] %s => %s\n", conffile.c_str(), sec->getName().c_str(), key.c_str(), value.c_str());

        } else {
            sprintf(buff, "%s, line %d: could not parse line", conffile.c_str(), lineno);
            std::string conf_error = std::string(buff);
            throw ConfFileException(conf_error, conffile, lineno);
        }
    }

    if(sec != 0) addSection(sec);

    in.close();
}

bool ConfFile::hasEntry(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry != 0) return true;

    return false;
}

bool ConfFile::hasValue(const std::string& section, const std::string& key) {
    std::string value = getString(section, key);

    if(value.size()>0) return true;

    return false;
}

int ConfFile::countSection(const std::string& section) {
    ConfSectionList* sectionlist = getSections(section);

    if(sectionlist==0) return 0;

    int count = 0;

    for(ConfSectionList::iterator sit = sectionlist->begin(); sit != sectionlist->end(); sit++) {
        count++;
    }

    return count;
}

bool ConfFile::hasSection(const std::string& section) {

    ConfSection* sec = getSection(section);

    if(sec==0) return false;

    return true;
}

void ConfFile::addSection(ConfSection* section) {

    ConfSectionList* sectionlist = getSections(section->getName());

    if(sectionlist==0) {
        sectionmap[section->getName()] = sectionlist = new ConfSectionList;
    }

    sectionlist->push_back(section);
}

void ConfFile::setSection(ConfSection* section) {

    ConfSectionList* sectionlist = getSections(section->getName());

    if(sectionlist==0) {
        sectionmap[section->getName()] = sectionlist = new ConfSectionList;
    }

    if(sectionlist->size() != 0) {
        ConfSection* front = sectionlist->front();
        sectionlist->pop_front();
        delete front;
    }

    sectionlist->push_back(section);
}

//returns the list of all sections with a particular name
ConfSectionList* ConfFile::getSections(const std::string& section) {
    std::map<std::string, ConfSectionList*>::iterator section_finder = sectionmap.find(section);

    if(section_finder == sectionmap.end()) return 0;

    return section_finder->second;
}

//returns the first section with a particular name
ConfSection* ConfFile::getSection(const std::string& section) {

    ConfSectionList* sectionlist = getSections(section);

    if(sectionlist==0 || sectionlist->size()==0) return 0;

    return sectionlist->front();
}

//returns the first section with a particular name
void ConfFile::setEntry(const std::string& section, const std::string& key, const std::string& value) {

    ConfSection* sec = getSection(section);

    if(sec==0) {
        sec = new ConfSection(section);
        addSection(sec);
    }

    sec->setEntry(key, value);
}

//returns a list of all entries in a section with a particular name
ConfEntryList* ConfFile::getEntries(const std::string& section, const std::string& key) {

    ConfSection* sec = getSection(section);

    if(sec==0) return 0;

    ConfEntryList* entryList = sec->getEntries(key);

    return entryList;
}

//get first entry in a section with a particular name
ConfEntry* ConfFile::getEntry(const std::string& section, const std::string& key) {

    ConfSection* sec = getSection(section);

    if(sec==0) return 0;

    ConfEntry* entry = sec->getEntry(key);

    return entry;
}

std::string ConfFile::getString(const std::string& section, const std::string& key) {

    ConfEntry* entry = getEntry(section, key);

    if(entry==0) return std::string("");

    return entry->getString();
}

int ConfFile::getInt(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry) return entry->getInt();

    return 0;
}

float ConfFile::getFloat(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry) return entry->getFloat();

    return 0.0f;
}

bool ConfFile::getBool(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry) return entry->getBool();

    return false;
}

vec3f ConfFile::getVec3(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry) return entry->getVec3();

    return vec3f(0.0, 0.0, 0.0);
}

vec4f ConfFile::getVec4(const std::string& section, const std::string& key) {
    ConfEntry* entry = getEntry(section, key);

    if(entry) return entry->getVec4();

    return vec4f(0.0, 0.0, 0.0, 0.0);
}

void ConfFile::unknownOptionException(ConfEntry* entry) {
    std::string reason = "unknown option ";
    reason += entry->getName();

    entryException(entry, reason);
}

void ConfFile::missingValueException(ConfEntry* entry) {

    std::string reason = std::string("no value specified for ");
    reason += entry->getName();

    entryException(entry, reason);
}

void ConfFile::invalidValueException(ConfEntry* entry) {

    std::string reason = std::string("invalid ");
    reason += entry->getName();
    reason += std::string(" value");

    entryException(entry, reason);
}

void ConfFile::entryException(ConfEntry* entry, std::string reason) {

    std::string errmsg;

    if(conffile.size()) {
        errmsg = conffile;

        if(entry->getLineNumber() != 0) {
            char linebuff[256];
            snprintf(linebuff, 256, ", line %d", entry->getLineNumber());

            errmsg += std::string(linebuff);
        }

        errmsg += std::string(": ");
    }

    errmsg += reason;

    throw ConfFileException(errmsg, conffile, entry->getLineNumber());
}

