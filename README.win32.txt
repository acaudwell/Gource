Special notes for running Gource on Windows:
============================================

* Gource will write messages to stout.txt (or stderr.txt) instead of the console as it would on Linux.

* You can generate a log for your project using the following steps:

  1. run gource.exe --git-log-command
  2. get the command written to stdout.txt
  3. generate a log of your project using this command (eg my-project.log)
  4. run gource.exe my-project.log (or drag and drop my-project.log on top of gource.exe)

Cheers

Andrew Caudwell
