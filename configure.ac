#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT(Gource, 0.1, [acaudwell@gmail.com])
AC_CONFIG_SRCDIR([src/main.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

#check functions
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MKTIME
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit getcwd memset sqrt strchr])

#GL
AX_CHECK_GLU
CFLAGS="$CFLAGS $GLU_CFLAGS"
CPPFLAGS="$CPPFLAGS $GLU_CFLAGS"
LIBS="$LIBS $GLU_LIBS"

#SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

#Freetype2
AC_CHECK_FT2([9.0.3],[],[AC_MSG_ERROR([FreeType2 is required by FTGL. Please see README])])
CFLAGS="$CFLAGS $FT2_CFLAGS"
CPPFLAGS="$CPPFLAGS $FT2_CFLAGS"

#FTGL
PKG_CHECK_MODULES(FTGL, ftgl >= 2.1.3, [], AC_MSG_ERROR(ftgl 2.1.3 or greater is required. Please see README))
CFLAGS="$CFLAGS $FTGL_CFLAGS"
CPPFLAGS="$CPPFLAGS $FTGL_CFLAGS"
LIBS="$LIBS $FTGL_LIBS"

#PNG library
AC_CHECK_LIB(png, png_read_info, , AC_MSG_ERROR([PNG library required. Please see README]))

#JPEG library
AC_CHECK_LIB(jpeg, jpeg_read_header, , AC_MSG_ERROR([JPEG library required. Please see README]))

#SDL_image library with PNG support
AC_CHECK_LIB(SDL_image, IMG_LoadPNG_RW, , AC_MSG_ERROR([SDL_image required. Please see README]))

#PCRE
AC_CHECK_LIB([pcre], [pcre_compile],, AC_MSG_ERROR(PCRE is required. Please see README))

#Check for required headers
AC_CHECK_HEADER([SDL.h],, AC_MSG_ERROR(SDL.h is required. Please see README))
AC_CHECK_HEADER([SDL_image.h],, AC_MSG_ERROR(SDL_image.h is required. Please see README))
AC_CHECK_HEADER([ftgl.h],, AC_MSG_ERROR(ftgl.h is required. Please see README))
AC_CHECK_HEADER([pcre.h],, AC_MSG_ERROR(pcre.h is required. Please see README))

#see if ttf-font-dir option is enabled
AC_ARG_ENABLE(ttf-font-dir,[AS_HELP_STRING([--enable-ttf-font-dir=DIR],[directory containing FreeSans.ttf])],[gourcefontdir="$enableval"],[gourcefontdir=""])
AC_SUBST(gourcefontdir)

#see if FFmpeg support is enabled
AC_ARG_WITH(ffmpeg, [AS_HELP_STRING([--with-ffmpeg],[ffmpeg support for creating movies])],[ffmpeg_support="yes"],[ffmpeg_support=""])
if test ! -z "$ffmpeg_support"
then
    PKG_CHECK_MODULES(FFMPEG, [libavutil libavdevice libavcodec libavformat libswscale], [], AC_MSG_ERROR(FFmpeg development packages required. Please see README))
    AC_CHECK_LIB([libavformat],[avformat_alloc_context],[FFMPEG_CFLAGS="-DHAS_AVFORMAT_ALLOC_CONTEXT $FFMPEG_CFLAGS"], [])

    CFLAGS="$CFLAGS -DHAVE_FFMPEG $FFMPEG_CFLAGS"
    CPPFLAGS="$CPPFLAGS -DHAVE_FFMPEG $FFMPEG_CFLAGS"
    LIBS="$LIBS $FFMPEG_LIBS"
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/core/Makefile])
AC_OUTPUT
