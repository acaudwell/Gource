#!/bin/bash
# This script updates Gource, when in "gource-git-live" mode.
# @author: Konrad Talik (github.com/ktalik)

# usage:
#    gource-git-refresh.sh

# NOTE: You should run it after every commit or in an alias.

# Send the last line of the log to the .gourcefifo.
git log -1 --pretty=format:user:%aN%n%ct --reverse --raw --encoding=UTF-8 --no-renames > .gourcefifo
