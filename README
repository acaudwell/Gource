
                             Gource

              software version control visualization

                Copyright (C) 2009 Andrew Caudwell

                         http://gource.io

Contents
========

1. Description
2. Requirements
3. Using Gource
4. Copyright

1. Description
==============

OpenGL-based 3D visualisation tool for source control repositories.

The repository is displayed as a tree where the root of the repository is the
centre, directories are branches and files are leaves. Contributors to the
source code appear and disappear as they contribute to specific files and
directories.

2. Requirements
===============

Gource's display is rendered using OpenGL and requires a 3D accelerated video
card to run.

3. Using Gource
===============

gource [options] [path]

options:

    -h, --help
            Help ('-H' for extended help).

    -WIDTHxHEIGHT, --viewport WIDTHxHEIGHT
            Set the viewport size. If -f is also supplied, will attempt to set
            the video mode to this also. Add ! to make the window non-resizable.

    --screen SCREEN
            Set the number of the screen to display on.

    --window-position XxY
            Initial window position on your desktop which may be made up of
            multiple monitors.

            This will override the screen setting so don't specify both.

    --frameless
            Frameless window.

    -f, --fullscreen
            Fullscreen.

    -w, --windowed
            Windowed.

    --transparent
            Make the background transparent. Only really useful for screenshots.

    --start-date "YYYY-MM-DD hh:mm:ss +tz"
            Start with the first entry after the supplied date and optional time.

            If a time zone offset isn't specified the local time zone is used.

            Example accepted formats:

                "2012-06-30"
                "2012-06-30 12:00"
                "2012-06-30 12:00:00 +12"

    --stop-date "YYYY-MM-DD hh:mm:ss +tz"
            Stop after the last entry prior to the supplied date and optional time.

            Uses the same format as --start-date.

    -p, --start-position POSITION
            Begin at some position in the log (between 0.0 and 1.0 or 'random').

        --stop-position  POSITION
            Stop (exit) at some position in the log (does not work with STDIN).

    -t, --stop-at-time SECONDS
            Stop (exit) after a specified number of seconds.

        --stop-at-end
            Stop (exit) at the end of the log / stream.

        --loop
            Loop back to the start of the log when the end is reached.

        --loop-delay-seconds
            Seconds to delay before looping.

    -a, --auto-skip-seconds SECONDS
            Skip to next entry if nothing happens for a number of seconds.

    -s, --seconds-per-day SECONDS
            Speed of simulation in seconds per day.

        --realtime
            Realtime playback speed.

        --no-time-travel
            Use the time of the last commit if the time of a commit is in the past.

    -c, --time-scale SCALE
            Change simulation time scale.

    -i, --file-idle-time SECONDS
            Time in seconds files remain idle before they are removed or 0
            for no limit.

    -e, --elasticity FLOAT
            Elasticity of nodes.

    -b, --background-colour FFFFFF
            Background colour in hex.

    --background-image IMAGE
            Set a background image.

    --logo IMAGE
            Logo to display in the foreground.

    --logo-offset XxY
            Offset position of the logo.

    --title TITLE
            Set a title.

    --font-file FILE
            Specify the font. Should work with most font file formats supported by FreeType, such as TTF and OTF, among others.

    --font-scale SCALE
            Scale the size of all fonts.

    --font-size SIZE
            Font size used by the date and title.

    --file-font-size SIZE
            Font size of filenames.

    --dir-font-size SIZE
            Font size of directory names

    --user-font-size SIZE
            Font size of user names.

    --font-colour FFFFFF
            Font colour used by the date and title in hex.

    --key
            Show file extension key.

    --date-format FORMAT
            Specify display date string (strftime format).

    --log-command VCS
            Show the VCS log command used by gource (git,svn,hg,bzr,cvs2cl).

    --log-format VCS
            Specify the log format (git,svn,hg,bzr,cvs2cl,custom).

            Required when reading from STDIN.

    --git-branch
            Get the git log of a branch other than the current one.

    --follow-user USER
            Have the camera automatically follow a particular user.

    --highlight-dirs
            Highlight the names of all directories.

    --highlight-user USER
            Highlight the names of a particular user.

    --highlight-users
            Highlight the names of all users.

    --highlight-colour FFFFFF
            Font colour for highlighted users in hex.

    --selection-colour FFFFFF
            Font colour for selected users and files.

    --filename-colour FFFFFF
            Font colour for filenames.

    --dir-colour FFFFFF
            Font colour for directories.

    --dir-name-depth DEPTH
            Draw names of directories down to a specific depth in the tree.

    --dir-name-position FLOAT
            Position along edge of the directory name
            (between 0.1 and 1.0, default is 0.5).

    --filename-time SECONDS
            Duration to keep filenames on screen (>= 2.0).

    --file-extensions
            Show filename extensions only.

    --file-extension-fallback
            Use filename as extension if the extension is missing or empty.

    --file-filter REGEX
            Filter out file paths matching the specified regular expression.

    --file-show-filter REGEX
            Show only file paths matching the specified regular expression.

    --user-filter REGEX
            Filter usernames matching the specified regular expression.

    --user-show-filter REGEX
            Show only usernames matching the specified regular expression.

    --user-image-dir DIRECTORY
            Directory containing .jpg or .png images of users
            (eg "Full Name.png") to use as avatars.

    --default-user-image IMAGE
            Path of .jpg or .png to use as the default user image.

    --colour-images
            Colourize user images.

    --crop AXIS
            Crop view on an axis (vertical,horizontal).

    --padding FLOAT
            Camera view padding.

    --multi-sampling
            Enable multi-sampling.

    --no-vsync
            Disable vsync.

    --bloom-multiplier FLOAT
            Adjust the amount of bloom.

    --bloom-intensity FLOAT
            Adjust the intensity of the bloom.

    --max-files NUMBER
            Set the maximum number of files or 0 for no limit.

            Excess files will be discarded.

    --max-file-lag SECONDS
            Max time files of a commit can take to appear.

            Use -1 for no limit.

    --max-user-speed UNITS
            Max speed users can travel per second.

    --user-friction SECONDS
            Time users take to come to a halt.

    --user-scale SCALE
            Change scale of user avatars.

    --camera-mode MODE
            Camera mode (overview,track).

    --disable-auto-rotate
            Disable automatic camera rotation.

    --disable-input
            Disable keyboard and mouse input.

    --hide DISPLAY_ELEMENT
            Hide one or more display elements from the list below:

            bloom     - bloom effect
            date      - current date
            dirnames  - names of directories
            files     - file icons
            filenames - names of files
            mouse     - mouse cursor
            progress  - progress bar widget
            root      - root directory of tree
            tree      - animated tree structure
            users     - user avatars
            usernames - names of users

            Separate multiple elements with commas (eg "mouse,progress")

    --hash-seed SEED
            Change the seed of hash function.

    --caption-file FILE
            Caption file (see Caption Log Format).

    --caption-size SIZE
            Caption size.

    --caption-colour FFFFFF
            Caption colour in hex.

    --caption-duration SECONDS
            Caption duration.

    --caption-offset X
            Caption horizontal offset (0 to centre captions).

    -o, --output-ppm-stream FILE
            Output a PPM image stream to a file ('-' for STDOUT).

            This will automatically hide the progress bar initially and
            enable 'stop-at-end' unless other behaviour is specified.

    -r, --output-framerate FPS
            Framerate of output (25,30,60). Used with --output-ppm-stream.

    --output-custom-log FILE
            Output a custom format log file ('-' for STDOUT).

    --load-config CONFIG_FILE
            Load a gource conf file.

    --save-config CONFIG_FILE
            Save a gource conf file with the current options.

    --path PATH

    path    Either a supported version control directory, a pre-generated log
            file (see log commands or the custom log format), a Gource conf
            file or '-' to read STDIN.

            If path is omitted, gource will attempt to read a log from the
            current directory.

Git, Bazaar, Mercurial and SVN Examples:

View the log of the repository in the current path:

    gource

View the log of a project in the specified directory:

    gource my-project-dir

For large projects, generating a log of the project history may take a long
time. For centralized VCS like SVN, generating the log may also put load on
the central VCS server.

In these cases, you may like to save a copy of the log for later use.

You can generate a log in the VCS specific log format using
the --log-command VCS option:

    cd my-svn-project
    `gource --log-command svn` > my-svn-project.log
    gource my-svn-project.log

You can also have Gource write a copy of the log file in its own format:

    gource --output-custom-log my-project-custom.log

CVS Support:

Use 'cvs2cl' to generate the log and then pass it to Gource:

    cvs2cl --chrono --stdout --xml -g-q > my-cvs-project.log
    gource my-cvs-project.log

Custom Log Format:

If you want to use Gource with something other than the supported systems,
there is a pipe ('|') delimited custom log format:

    timestamp - A unix timestamp of when the update occured.
    username  - The name of the user who made the update.
    type      - initial for the update type - (A)dded, (M)odified or (D)eleted.
    file      - Path of the file updated.
    colour    - A colour for the file in hex (FFFFFF) format. Optional.

Caption Log Format:

Gource can display captions along the timeline by specifying a caption file
(using --caption-file) in the pipe ('|') delimited format below:

    timestamp - A unix timestamp of when to display the caption.
    caption   - The caption

Recording Videos:

See the guide on the homepage for examples of recording videos with Gource:

    https://github.com/acaudwell/Gource/wiki/Videos

More Information:

Visit the Gource homepage for guides and examples of using Gource with various
version control systems:

    http://gource.io

Interface:

The time shown in the top left of the screen is set initially from the first
log entry read and is incremented according to the simulation speed
(--seconds-per-day).

Pressing SPACE at any time will pause/resume the simulation. While paused you
may use the mouse to inspect the detail of individual files and users.

TAB cycles through selecting the current visible users.

The camera mode, either tracking activity or showing the entire code tree, can
be toggled using the Middle mouse button.

You can drag the left mouse button to manually control the camera. The right
mouse button rotates the view.

Interactive keyboard commands:

    (V)   Toggle camera mode
    (C)   Displays Gource logo
    (K)   Toggle file extension key
    (M)   Toggle mouse visibility
    (N)   Jump forward in time to next log entry
    (S)   Randomize colours
    (D)   Toggle directory name display mode
    (F)   Toggle file name display mode
    (U)   Toggle user name display mode
    (G)   Toggle display of users
    (T)   Toggle display of directory tree edges
    (R)   Toggle display of root directory edges
    (+-)  Adjust simulation speed
    (<>)  Adjust time scale
    (TAB) Cycle through visible users
    (F12) Screenshot
    (Alt+Enter) Fullscreen toggle
    (ESC) Quit

4. Copyright
============

Gource - software version control visualization
Copyright (C) 2009 Andrew Caudwell <acaudwell@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
