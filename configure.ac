#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT(Gource, 0.26a, [acaudwell@gmail.com])
AC_CONFIG_SRCDIR([src/main.h])

AM_INIT_AUTOMAKE([dist-bzip2 foreign subdir-objects])

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)

#Disable X11 on Macs unless required
AS_IF([test "$with_x" != yes], [
    case "$host_os" in
    darwin*)
        with_x="no"
    ;;
    esac
], [])

#GL
AX_CHECK_GLU
CXXFLAGS="$CXXFLAGS $GLU_CFLAGS"
CPPFLAGS="$CPPFLAGS $GLU_CFLAGS"
LIBS="$LIBS $GLU_LIBS"

#SDL
SDL_VERSION=1.2.10
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

#Freetype2
AC_CHECK_FT2([9.0.3],[],[AC_MSG_ERROR([FreeType2 is required by FTGL. Please see README])])
CXXFLAGS="$CXXFLAGS $FT2_CFLAGS"
CPPFLAGS="$CPPFLAGS $FT2_CFLAGS"

#FTGL
PKG_CHECK_MODULES(FTGL, ftgl >= 2.1.3, [], AC_MSG_ERROR(ftgl 2.1.3 or greater is required. Please see README))
CXXFLAGS="$CXXFLAGS $FTGL_CFLAGS"
CPPFLAGS="$CPPFLAGS $FTGL_CFLAGS"
LIBS="$LIBS $FTGL_LIBS"

#PNG library
AC_CHECK_LIB(png, png_read_info, , AC_MSG_ERROR([PNG library required. Please see README]))

#JPEG library
AC_CHECK_LIB(jpeg, jpeg_read_header, , AC_MSG_ERROR([JPEG library required. Please see README]))

#SDL_image library with PNG support
AC_CHECK_LIB(SDL_image, IMG_LoadPNG_RW, LIBS="$LIBS -lSDL_image", AC_MSG_ERROR([SDL_image required. Please see README]))

#PCRE
AC_CHECK_LIB([pcre], [pcre_compile],, AC_MSG_ERROR(PCRE is required. Please see README))

#Check for required headers
AC_CHECK_HEADER([SDL.h],, AC_MSG_ERROR(SDL.h is required. Please see README))
AC_CHECK_HEADER([SDL_image.h],, AC_MSG_ERROR(SDL_image.h is required. Please see README))
AC_CHECK_HEADER([ftgl.h],, AC_MSG_ERROR(ftgl.h is required. Please see README))
AC_CHECK_HEADER([pcre.h],, AC_MSG_ERROR(pcre.h is required. Please see README))

#see if ttf-font-dir option is enabled
AC_ARG_ENABLE(ttf-font-dir,[AS_HELP_STRING([--enable-ttf-font-dir=DIR],[directory containing GNU FreeFont TTF fonts])],[gourcefontdir="$enableval"],[gourcefontdir=""])
AM_CONDITIONAL([FONTDIR], [test "x$gourcefontdir" != "x"])
AC_SUBST(gourcefontdir)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
