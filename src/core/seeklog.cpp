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

#include "seeklog.h"

//StreamLog

StreamLog::StreamLog() {
    this->stream = &std::cin;

    fcntl_fail = false;

#ifndef _WIN32
    int ret = fcntl(STDIN_FILENO, F_GETFL, 0);

    if (fcntl (STDIN_FILENO, F_SETFL, ret | O_NONBLOCK) < 0) {
        debugLog("fcntl(stdin) failed\n");
        fcntl_fail = true;
    }
#endif
}

StreamLog::StreamLog(std::istream* stream) {
    this->stream = stream;
}

bool StreamLog::getNextLine(std::string& line) {

    if(isFinished()) return false;

    char buff[1024];

    stream->getline(buff, 1024);
    line = std::string(buff);

    if(isFinished()) {
        //clear the failbit if only failed because the line was too long
        if(!stream->bad() && stream->gcount() >= (1024-1)) {
            stream->clear();
        }

        return false;
    }

    return true;
}

bool StreamLog::isFinished() {

    if(fcntl_fail || stream->fail() || stream->eof()) {
        return true;
    }

    return false;
}

// SeekLog

SeekLog::SeekLog(std::string logfile) {
    this->logfile = logfile;

    this->buffstream = 0;

    if(!readFully()) {
        printf("failed to read %s\r\n", logfile.c_str());
        exit(1);
    }
}

bool SeekLog::readFully() {

    if(buffstream!=0) delete buffstream;

    std::ifstream file(logfile.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    file_size = file.tellg();

    debugLog("file_size = %d\n", file_size);

    if(!file.is_open()) return false;

    //buffer entire file into memory
    char* filebuffer = new char[file_size];

    file.seekg (0, std::ios::beg);

    if(!file.read (filebuffer, file_size)) {
        file.close();
        return false;
    }

    file.close();

    buffstream = new std::istringstream(std::string(filebuffer));

    return true;
}

SeekLog::~SeekLog() {
    if(buffstream!=0) delete buffstream;
}

float SeekLog::getPercent() {
    return current_percent;
}

void SeekLog::setPointer(long pointer) {
    buffstream->seekg(pointer);
}

long SeekLog::getPointer() {
    return buffstream->tellg();
}

void SeekLog::seekTo(float percent) {

    if(isFinished()) buffstream->clear();

    long mem_pointer = (long) (percent * file_size);

    setPointer(mem_pointer);

    //throw away end of line
    if(mem_pointer!=0) {
        std::string eol;
        getNextLine(eol);
    }
}

bool SeekLog::getNextLine(std::string& line) {

    //try and fix the stream
    if(isFinished()) buffstream->clear();

    char buff[1024];

    buffstream->getline(buff, 1024);
    line = std::string(buff);

    if(buffstream->fail()) {
        //clear the failbit if only failed because the line was too long
        if(!buffstream->bad() && buffstream->gcount() >= (1024-1)) {
            buffstream->clear();
        }

        return false;
    }

    current_percent = (float) buffstream->tellg() / file_size;
    //debugLog("current_percent = %.2f\n", current_percent);

    return true;
}

bool SeekLog::isFinished() {
    bool finished = false;

    if(buffstream->fail() || buffstream->eof()) {
        debugLog("buffstream is finished\n");
        finished=true;
    }

    return finished;
}
