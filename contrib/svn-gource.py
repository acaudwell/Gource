#!/usr/bin/python
## Copyright (c) 2009 Cameron Hart (cam@bitshifter.net.nz)
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
## IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
## OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
## NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
## THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
import sys
import time
import getopt
import re
from xml.etree import ElementTree

opt_filter_dirs = False

_USAGE = """
svn-gource.py [--help] [--filter-dirs] <file>

The input file must be the output of the command svn log --verbose --xml.
"""

# regular expression for matching any file with an extension
extn_prog = re.compile(".*/?[^/]+\.[^\.]+$")

def reverse(data):
    """Returns the log entries in reverse."""
    for index in range(len(data)-1, -1, -1):
        yield data[index]

def processXmltree(xmltree):
    global opt_filter_dirs
    for logentry in reverse(xmltree.getiterator("logentry")):
        datetext = logentry.find("date").text

        # svn xml logs always use UTC
        timestamp = (time.mktime(time.strptime(datetext[:-8], "%Y-%m-%dT%H:%M:%S")))
        # a bit of a hack to get it into local time again...
        #timestamp = timestamp - time.timezone

        #author might not exist
        try:
            author = logentry.find("author").text
        except:
            author = ""

        # output all affected files
        for pathentry in logentry.getiterator("path"):

            # apply directory filtering strategy
            if opt_filter_dirs and not re.match(extn_prog, pathentry.text):
                continue;

            # join output
            print( "|".join( ( "%d" % int(timestamp), "%s" % author.encode("utf-8"), "%s" % pathentry.get("action"), "%s" % pathentry.text.encode("utf-8"), "" ) ) )

def printUsage(message):
    sys.stderr.write(_USAGE)
    if message:
        sys.exit('\nFATAL ERROR: ' + message)
    else:
        sys.exit(1)

def processArguments():
    global opt_filter_dirs

    try:
        opts, filenames = getopt.getopt(sys.argv[1:], '', ['help', 'filter-dirs'])
    except getopt.GetoptError:
        printUsage('Invalid arguments.')

    for (opt, val) in opts:
        if opt == '--help':
            printUsage(None)
        elif opt == '--filter-dirs':
            opt_filter_dirs = True

    if not filenames:
        printUsage('No input file specified.')

    return filenames[0]


if __name__ == "__main__":
    filename = processArguments()

    xmltree = ElementTree.parse(filename)

    processXmltree(xmltree)



