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

#check for GLU
AX_CHECK_GLU

AC_CHECK_LIB([SDL], [main],, AC_MSG_ERROR(SDL is required. Please see README))
AC_CHECK_LIB([SDL_image], [main],, AC_MSG_ERROR(SDL_image is required. Please see README))
AC_CHECK_LIB([pcre], [main],, AC_MSG_ERROR(pcre is required. Please see README))
AC_CHECK_LIB([ftgl], [main],, AC_MSG_ERROR(ftgl is required. Please see README))

#Freetype2
AC_CHECK_FT2([9.0.3],[],[AC_MSG_ERROR([FreeType2 is required by FTGL. Please see README])])

AC_ARG_ENABLE(ttf-font-dir,[AS_HELP_STRING([--enable-ttf-font-dir=DIR],[directory containing FreeSans.ttf])],[gourcefontdir="$enableval"],[gourcefontdir=""])
AC_SUBST(gourcefontdir)

# Add FT2 flags. Also means FTGL header check will work. Yay.
CFLAGS="$CFLAGS $FT2_CFLAGS"
CPPFLAGS="$CPPFLAGS $FT2_CFLAGS"

AC_CHECK_HEADER([FTGL/ftgl.h],, AC_MSG_ERROR(FTGL/ftgl.h is required. Please see README))

# Check for SDL, SDL_Image, FTGL, pcre headers

AC_CHECK_HEADER([SDL/SDL.h],, AC_MSG_ERROR(SDL/SDL.h is required. Please see README))
AC_CHECK_HEADER([SDL/SDL_opengl.h],, AC_MSG_ERROR(SDL/SDL_opengl.h is required. Please see README))
AC_CHECK_HEADER([SDL/SDL_image.h],, AC_MSG_ERROR(SDL/SDL_image.h is required. Please see README))
AC_CHECK_HEADER([pcre.h],, AC_MSG_ERROR(pcre.h is required. Please see README))

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/core/Makefile])
AC_OUTPUT
