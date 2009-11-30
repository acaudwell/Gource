#!/usr/bin/env python

# python script to parse the mercurial repository log into the custom format
# expected by Gource (http://code.google.com/p/gource/)
# Gource is a software version control visualization program, pretty cool :)
#
# usage:
# ./mercurial-gource.py [OUTPUT_FILE]
#
# where:
#       OUTPUT_FILE is the output file for the custom log
#       if OUTPUT_FILE is not provided, output to stdout
#
# example usage with Gource (assuming that Gource is installed):
#   ./mercurial-gource.py project.log && gource project.log
# OR
#   ./mercurial-gource.py | gource --log-format custom -
# but gource doesn't loop at the end when reading from stdin
#
#
# This script should work for any mercurial repository (version number?),
#
# Something that may be worth doing is adding a "--limit NUMBER" to hg log,
# where NUMBER is an argument to this script?
#


def custom_logformat(date, author, type, file):
    return '%(date)s|%(author)s|%(type)s|%(file)s' %{
                'date': date,
                'author': author,
                'type': type,
                'file': file,
            }


if __name__ == '__main__':
    import subprocess
    # get the log, in a easy to parse format
    args = ['hg', 'log', '--template', '{date}|{author|person}|m {files}|a {file_adds}|d {file_dels}\n']
    process = subprocess.Popen(args, stdout=subprocess.PIPE)
    log_output, log_err = process.communicate()
    entries = log_output.split('\n')

    # regex used to parse the hg log
    import re
    custom_regex = r'([0-9.+-]+)\|([^|]+)\|m ([^|]+)?\|a ([^|]+)?\|d ([^|]+)?'
    regex = re.compile(custom_regex)

    # get the output (file or stdout)
    import sys
    outfile = None
    if len(sys.argv) == 2:
        outfile = open(sys.argv[1], 'w')
    else:
        outfile = sys.stdout

    # important fields of log
    date = ''
    author = ''
    # mercurial doesn't seem to have a 'file_modifies' template keyword, but
    # there is a 'files' template keyword, which includes:
    # modified, added and removed
    # so: modfied = files - added - removed
    modified = []
    added = []
    deleted = []

    # hg log lists newest to oldest, gource wants oldest to newest
    # so just reverse the entries
    entries.reverse()
    for entry in entries:
        if entry == '':
            continue

        match = regex.match(entry)
        if not match:
            # sys.stderr.write('** Non-matching: %s\n' %entry)
            continue

        date, author, modified, added, deleted = match.groups()
        # dunno if this is the best way of parsing the timestamp, which should be
        # in the format: {UTC timestamp}{timezone offset}
        # e.g. 1234567890.0-7200
        date = str(int(eval(date)))

        modified = modified.split() if modified else []
        added = added.split() if added else []
        deleted = deleted.split() if deleted else []

        for f in added:
            outfile.write(custom_logformat(date, author, 'A', f))
            outfile.write('\n')
            if f in modified:
                modified.remove(f)

        for f in deleted:
            outfile.write(custom_logformat(date, author, 'D', f))
            outfile.write('\n')
            if f in modified:
                modified.remove(f)

        for f in modified:
            outfile.write(custom_logformat(date, author, 'M', f))
            outfile.write('\n')

    outfile.close()

