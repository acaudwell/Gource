/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SEEK_LOG_H
#define SEEK_LOG_H

#include "display.h"
#include "logger.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <fcntl.h>

class BaseLog {
public:
    virtual bool getNextLine(std::string& line) {};
    virtual bool isFinished() {};
};

class StreamLog : public BaseLog {

    std::istream* stream;
    bool fcntl_fail;
public:
    StreamLog();
    StreamLog(std::istream* stream);
    ~StreamLog();
    bool getNextLine(std::string& line);
    bool isFinished();
};

class SeekLog : public BaseLog {

    std::string logfile;

    std::istringstream* buffstream;
    long file_size;
    float current_percent;

    bool readFully();
public:
    SeekLog(std::string logfile);
    ~SeekLog();

    void setPointer(long pointer);
    long getPointer();

    void seekTo(float percent);
    bool getNextLine(std::string& line);
    float getPercent();

    bool isFinished();
};

#endif
