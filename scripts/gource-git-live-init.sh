#!/bin/bash
# This script initializes the Gource in "gource-git-live" mode.
# In result, Gource will wait for you to use "gource-git-refresh.sh" script,
# which will update the log and the current graph view.
# @author: Konrad Talik (github.com/ktalik)

# usage:
#    gource-git-live-init.sh

mkfifo .gourcefifo

# Transfer .gourcefifo to the STDIN.of Gource.
`gource --realtime --log-format git - < .gourcefifo`

rm .gourcefifo

