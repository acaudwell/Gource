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

#include "regex.h"

Regex::Regex(std::string regex, bool test) {

    re = pcre_compile(
        regex.c_str(),
        0,
        &error,
        &erroffset,
        0
    );

    if(!re) {
        valid = false;

        if(!test) {
            throw RegexCompilationException(regex);
        }

    } else {
        valid = true;
    }

}

Regex::~Regex() {
    if(re != 0) pcre_free(re);
}

bool Regex::isValid() {
    return valid;
}

bool Regex::match(const std::string& str, std::vector<std::string>* results) {

    int ovector[REGEX_MAX_MATCHES];

    int rc = pcre_exec(
        re,
        0,
        str.c_str(),
        str.size(),
        0,
        0,
        ovector,
        REGEX_MAX_MATCHES
    );

    //failed match
    if(rc<1) {
        return false;
    }


    if(results!=0) {
        results->clear();
        for (int i = 1; i < rc; i++) {
            std::string match(str, ovector[2*i], ovector[2*i+1] - ovector[2*i]);
            results->push_back(match);
        }
    }

    return true;
}


