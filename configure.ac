#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT(Gource, 0.34-rc2, [acaudwell@gmail.com])
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
AX_CHECK_GL
AX_CHECK_GLU
CXXFLAGS="$CXXFLAGS $GL_CFLAGS $GLU_CFLAGS"
CPPFLAGS="$CPPFLAGS $GL_CFLAGS $GLU_CFLAGS"
LIBS="$LIBS $GL_LIBS $GLU_LIBS"

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
AC_CHECK_FT2([9.0.3],[],[AC_MSG_ERROR([FreeType2 is required. Please see INSTALL])])
CXXFLAGS="$CXXFLAGS $FT2_CFLAGS"
CPPFLAGS="$CPPFLAGS $FT2_CFLAGS"
LIBS="$LIBS $FT2_LIBS"

#SDL_image library with PNG support
AC_CHECK_LIB(SDL_image, IMG_LoadPNG_RW, LIBS="$LIBS", AC_MSG_ERROR([SDL_image with PNG support required. Please see INSTALL]))

#SDL_image library with JPG support
AC_CHECK_LIB(SDL_image, IMG_LoadJPG_RW, LIBS="$LIBS", AC_MSG_ERROR([SDL_image with JPG support required. Please see INSTALL]))

LIBS="$LIBS -lSDL_image"

#PCRE
AC_CHECK_LIB([pcre], [pcre_compile],, AC_MSG_ERROR(PCRE is required. Please see INSTALL))

#GLEW
AC_CHECK_LIB(GLEW, glewInit, LIBS="$LIBS -lGLEW", AC_MSG_ERROR([GLEW required. Please see INSTALL]))

#Check for required headers
AC_CHECK_HEADER([SDL.h],, AC_MSG_ERROR(SDL.h is required. Please see INSTALL))
AC_CHECK_HEADER([SDL_image.h],, AC_MSG_ERROR(SDL_image.h is required. Please see INSTALL))
AC_CHECK_HEADER([pcre.h],, AC_MSG_ERROR(pcre.h is required. Please see INSTALL))
AC_CHECK_HEADER([GL/glew.h],, AC_MSG_ERROR(glew.h is required. Please see INSTALL))

#see if ttf-font-dir option is enabled
AC_ARG_ENABLE(ttf-font-dir,[AS_HELP_STRING([--enable-ttf-font-dir=DIR],[directory containing GNU FreeFont TTF fonts])],[gourcefontdir="$enableval"],[gourcefontdir=""])
AM_CONDITIONAL([FONTDIR], [test "x$gourcefontdir" != "x"])
AC_SUBST(gourcefontdir)


#see if building against system TinyXML library
use_tinyxml_system_library=no
AC_ARG_WITH(tinyxml, AC_HELP_STRING([--with-tinyxml], [Use system installed TinyXML library]), use_tinyxml_system_library=$withval)
if test "x$use_tinyxml_system_library" = "xyes"; then
    AC_HAVE_LIBRARY(tinyxml,, AC_MSG_ERROR([TinyXML library was requested but not found]))
fi
AM_CONDITIONAL(USE_BUNDLED_TINYXML, test "x$use_tinyxml_system_library" != "xyes")

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
