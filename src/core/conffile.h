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

#ifndef CONF_FILE_H
#define CONF_FILE_H

#include <fstream>
#include <string>
#include <list>
#include <map>
#include <stdexcept>

#include "regex.h"

class ConfFileException : public std::exception {
protected:
    std::string errmsg;
public:
    ConfFileException(const std::string& errmsg, const std::string& conffile, int lineno = 0) : errmsg(errmsg), conffile(conffile), lineno(lineno) {}

    std::string conffile;
    int lineno;

    virtual ~ConfFileException() throw () {};

    virtual const char* what() const throw() { return errmsg.c_str(); }
};

class ConfEntry {
    std::string name;
    std::string value;
    int lineno;

public:
    ConfEntry();
    ConfEntry(const std::string& name);
    ConfEntry(const std::string& name, const std::string& value, int lineno = 0);
    ConfEntry(const std::string& name, int value);
    ConfEntry(const std::string& name, float value);
    ConfEntry(const std::string& name, bool value);
    ConfEntry(const std::string& name, vec2f value);
    ConfEntry(const std::string& name, vec3f value);
    ConfEntry(const std::string& name, vec4f value);

    void setName(const std::string& name);

    void setString(const std::string& value);
    void setFloat(float value);
    void setInt(int value);
    void setBool(bool value);
    void setVec2(vec2f value);
    void setVec3(vec3f value);
    void setVec4(vec4f value);

    bool hasValue();

    int getLineNumber();
    std::string getName();

    std::string getString();
    int         getInt();
    float       getFloat();
    bool        getBool();
    vec2f       getVec2();
    vec3f       getVec3();
    vec4f       getVec4();
};

typedef std::list<ConfEntry*> ConfEntryList;

class ConfSection {
    std::map<std::string, ConfEntryList*> entrymap;
    std::string name;
public:
    ConfSection();
    ConfSection(const std::string& name);
    ~ConfSection();

    void clear();

    ConfEntry* getEntry(const std::string& key);
    ConfEntryList* getEntries(const std::string& key);

    std::string getName();

    bool        hasValue(const std::string& key);

    std::string getString(const std::string& key);
    int         getInt(const std::string& key);
    float       getFloat(const std::string& key);
    bool        getBool(const std::string& key);
    vec3f       getVec3(const std::string& key);
    vec4f       getVec4(const std::string& key);

    void print(std::ostream& out);

    void setEntry(ConfEntry* entry);
    void addEntry(ConfEntry* entry);

    void setEntry(const std::string& name, const std::string& value, int lineno=0);
    void addEntry(const std::string& name, const std::string& value, int lineno=0);
};

typedef std::list<ConfSection*> ConfSectionList;

class ConfFile {

    std::string conffile;

    std::map<std::string, ConfSectionList*> sectionmap;
public:
    ConfFile();
    ~ConfFile();
    void clear();

    void setFilename(const std::string& filename);
    std::string getFilename();

    void load(const std::string& conffile);
    void load();

    void save(const std::string& conffile);
    void save();

    bool hasSection(const std::string& section);
    ConfSection* getSection(const std::string& section);
    ConfSectionList* getSections(const std::string& section);

    ConfEntry*   getEntry(const std::string& section, const std::string& key);
    ConfEntryList* getEntries(const std::string& section, const std::string& key);

    void addSection(ConfSection* section);
    void setSection(ConfSection* section);

    int countSection(const std::string& section);

    void setEntry(const std::string& section, const std::string& key, const std::string& value);

    bool        hasEntry(const std::string& section,  const std::string& key);
    bool        hasValue(const std::string& section,  const std::string& key);
    std::string getString(const std::string& section, const std::string& key);
    int         getInt(const std::string& section,    const std::string& key);
    float       getFloat(const std::string& section,  const std::string& key);
    bool        getBool(const std::string& section,   const std::string& key);
    vec3f       getVec3(const std::string& section,   const std::string& key);
    vec4f       getVec4(const std::string& section,   const std::string& key);

    void unknownOptionException(ConfEntry* entry);
    void missingValueException(ConfEntry* entry);
    void invalidValueException(ConfEntry* entry);
    void entryException(ConfEntry* entry, std::string reason);
};

#endif
