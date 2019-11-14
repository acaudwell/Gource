#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT(Gource, 0.51, [acaudwell@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/main.h])
AM_INIT_AUTOMAKE([dist-bzip2 foreign subdir-objects])

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_LANG(C++)

AC_CHECK_FUNCS([timegm unsetenv])

#Disable X11 on Macs unless required
AS_IF([test "$with_x" != yes], [
    case "$host_os" in
    darwin*)
        with_x="no"
        LIBS="$LIBS -framework CoreFoundation"
    ;;
    esac
], [])

#GL
AX_CHECK_GL
AX_CHECK_GLU
CXXFLAGS="$CXXFLAGS $GL_CFLAGS $GLU_CFLAGS"
CPPFLAGS="$CPPFLAGS $GL_CFLAGS $GLU_CFLAGS"
LIBS="$LIBS $GL_LIBS $GLU_LIBS"

PKG_CHECK_MODULES([FT2], [freetype2 >= 9.0.3])
PKG_CHECK_MODULES([PCRE], [libpcre])
PKG_CHECK_MODULES([GLEW], [glew])
PKG_CHECK_MODULES([SDL2], [sdl2 SDL2_image]);
PKG_CHECK_MODULES([PNG], [libpng >= 1.2])

CPPFLAGS="${CPPFLAGS} ${FT2_CFLAGS} ${PCRE_CFLAGS} ${GLEW_CFLAGS} ${SDL2_CFLAGS} ${PNG_CFLAGS}"
LIBS="${LIBS} ${FT2_LIBS} ${PCRE_LIBS} ${GLEW_LIBS} ${SDL2_LIBS} ${PNG_LIBS}"

AC_CHECK_FUNCS([IMG_LoadPNG_RW], , AC_MSG_ERROR([SDL2_image with PNG support required. Please see INSTALL]))
AC_CHECK_FUNCS([IMG_LoadJPG_RW], , AC_MSG_ERROR([SDL2_image with JPEG support required. Please see INSTALL]))

#BOOST
AX_BOOST_BASE([1.46], , AC_MSG_ERROR(Boost Filesystem >= 1.46 is required. Please see INSTALL))
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LIBS="$LIBS $BOOST_SYSTEM_LIB $BOOST_FILESYSTEM_LIB"

AC_ARG_ENABLE(font-file,[AS_HELP_STRING([--enable-font-file=FILE],[override the default font])],[gourcefontfile="$enableval"],[gourcefontfile=""])
AM_CONDITIONAL([FONTFILE], [test "x$gourcefontfile" != "x"])
AC_SUBST(gourcefontfile)

#see if ttf-font-dir option is enabled
AC_ARG_ENABLE(ttf-font-dir,[AS_HELP_STRING([--enable-ttf-font-dir=DIR],[directory containing GNU FreeFont TTF fonts])],[gourcefontdir="$enableval"],[gourcefontdir=""])
AM_CONDITIONAL([FONTDIR], [test "x$gourcefontdir" != "x"])
AC_SUBST(gourcefontdir)

#GLM
AC_CHECK_HEADER([glm/glm.hpp],, AC_MSG_ERROR(GLM headers are required. Please see INSTALL))

#see if building against system TinyXML library
use_tinyxml_system_library=no
AC_ARG_WITH(tinyxml, AC_HELP_STRING([--with-tinyxml], [Use system installed TinyXML library]), use_tinyxml_system_library=$withval)
if test "x$use_tinyxml_system_library" = "xyes"; then
    AC_HAVE_LIBRARY(tinyxml,, AC_MSG_ERROR([TinyXML library was requested but not found]))
fi
AM_CONDITIONAL(USE_BUNDLED_TINYXML, test "x$use_tinyxml_system_library" != "xyes")

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
