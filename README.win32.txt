Special notes for running Gource on Windows:
============================================

If gource is run without supplying a log file, you may see a console window pop up. This is Gource calling git-log
itself. This means if you launch gource in fullscreen mode (-f) it may lose focus. To avoid this happening use a
pre-generated log (see --git-log-command in README.txt).

Cheers

Andrew Caudwell
