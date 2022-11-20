# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_check_glu.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GLU([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks for GLUT. If a valid GLU implementation is found, the configure
#   script would export the C preprocessor symbol "HAVE_GLU=1".
#
#   If either a valid GLU header or library was not found, by default the
#   configure script would exit on error. This behavior can be overwritten
#   by providing a custom "ACTION-IF-NOT-FOUND" hook.
#
#   If the header, library was found, and been tested for compiling and
#   linking the configuration would export the required compiler flags to
#   "GLU_CFLAGS" and "GLU_LIBS" environment variables. These two variables
#   can also be overwritten by defining the environment variables before
#   executing the configure program. If it was predefined, configure would
#   not try to overwrite it, but it would still perform the compile and link
#   test. Only when the tests succeeded does the configure script to export
#   "HAVE_GLU=1" and to run "ACTION-IF-FOUND" hook.
#
#   If user didn't specify the "ACTION-IF-FOUND" hook, the configuration
#   would prepend "GLU_CFLAGS" and "GLU_LIBS" to "CFLAGS" and "LIBS", like
#   many other autoconf macros do.
#
#   If the header "GL/glu.h" is found, "HAVE_GL_GLU_H" is defined. If the
#   header "OpenGL/glu.h" is found, HAVE_OPENGL_GLU_H is defined.
#
#   You should use something like this in your headers:
#
#     # if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#     #  include <windows.h>
#     # endif
#     # if defined(HAVE_GL_GLU_H)
#     #  include <GL/glu.h>
#     # elif defined(HAVE_OPENGL_GLU_H)
#     #  include <OpenGL/glu.h>
#     # else
#     #  error no glu.h
#     # endif
#
#   On the OSX platform, you can use the option --with-xquartz-gl to use
#   X11/Xquartz GLU implementation instead of the system built in GLU
#   framework.
#
#   Some implementations (in particular, some versions of Mac OS X) are
#   known to treat the GLU tesselator callback function type as "GLvoid
#   (*)(...)" rather than the standard "GLvoid (*)()". If the former
#   condition is detected, this macro defines "HAVE_VARARGS_GLU_TESSCB".
#
# LICENSE
#
#   Copyright (c) 2009 Braden McDaniel <braden@endoframe.com>
#   Copyright (c) 2013 Bastien Roucaries <roucaries.bastien+autoconf@gmail.com>
#   Copyright (c) 2016 Felix Chern <idryman@gmail.com>
#
#   This program is free software; you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation; either version 2 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <https://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 23

# example program
m4_define([_AX_CHECK_GLU_PROGRAM],
          [AC_LANG_PROGRAM([[
# if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#   include <windows.h>
# endif
# ifdef HAVE_GL_GLU_H
#   include <GL/glu.h>
# elif defined(HAVE_OPENGL_GLU_H)
#   include <OpenGL/glu.h>
# else
#   error no glu.h
# endif
]],[[gluBeginCurve(0)]])])


dnl Default include : add windows.h
dnl see http://www.opengl.org/wiki/Platform_specifics:_Windows
dnl (acceded 20120801)
AC_DEFUN([_AX_CHECK_GLU_INCLUDES_DEFAULT],dnl
[
  AC_INCLUDES_DEFAULT
  [
  # if defined(HAVE_WINDOWS_H) && defined(_WIN32)
  #   include <windows.h>
  # endif
  ]
])

# check tesselation callback function signature.
m4_define([_AX_CHECK_GLU_VARARGS_TESSVB_PROGRAM],
[AC_LANG_PROGRAM([[
# if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#   include <windows.h>
# endif
# ifdef HAVE_GL_GLU_H
#   include <GL/glu.h>
# elif defined(HAVE_OPENGL_GLU_H)
#   include <OpenGL/glu.h>
# else
#   error no glu.h
# endif
]],
[[GLvoid (*func)(...); gluTessCallback(0, 0, func)]])
])


# _AX_CHECK_GLU_SAVE_FLAGS(LIST-OF-FLAGS,[LANG])
# ----------------------------------------------
# Save the flags to shell variables.
# Example: _AX_CHECK_GLU_SAVE_FLAGS([[CFLAGS],[LIBS]]) expands to
# AC_LANG_PUSH([C])
# glu_saved_flag_cflags=$CFLAGS
# glu_saved_flag_libs=$LIBS
# CFLAGS="$GLU_CFLAGS $CFLAGS"
# LIBS="$GLU_LIBS $LIBS"
#
# Can optionally support other LANG by specifying $2
AC_DEFUN([_AX_CHECK_GLU_SAVE_FLAGS], [
 m4_ifval([$2],
          [AC_LANG_PUSH([$2])],
          [AC_LANG_PUSH([C])])
 AX_SAVE_FLAGS_WITH_PREFIX([GLU],[$1]) dnl defined in ax_check_gl
])

# _AX_CHECK_GLU_RESTORE_FLAGS(LIST-OF-FLAGS)
# Use this marcro to restore the flags you saved using
# _AX_CHECK_GLU_SAVE_FLAGS
#
# Example: _AX_CHECK_GLU_RESTORE_FLAGS([[CFLAGS],[LIBS]]) expands to
# CFLAGS="$glu_saved_flag_cflags"
# LIBS="$glu_saved_flag_libs"
# AC_LANG_POP([C])
AC_DEFUN([_AX_CHECK_GLU_RESTORE_FLAGS], [
 AX_RESTORE_FLAGS_WITH_PREFIX([GLU],[$1]) dnl defined in ax_check_gl
 m4_ifval([$2],
          [AC_LANG_POP([$2])],
          [AC_LANG_POP([C])])
])


# Search headers and export $ax_check_glu_have_headers
AC_DEFUN([_AX_CHECK_GLU_HEADERS], [
  _AX_CHECK_GLU_SAVE_FLAGS([CFLAGS])
  AC_CHECK_HEADERS([$1],
                   [ax_check_glu_have_headers="yes";],
                   [],
                   [_AX_CHECK_GLU_INCLUDES_DEFAULT()])
  _AX_CHECK_GLU_RESTORE_FLAGS([CFLAGS])
])


# _AX_CHECK_GLU_SEARCH_LIBS(LIBS)
# -------------------------------
# Search for a valid GLU lib from $1 and set
# GLU_LIBS respectively
AC_DEFUN([_AX_CHECK_GLU_SEARCH_LIBS], [
 _AX_CHECK_GLU_SAVE_FLAGS([[CFLAGS],[LIBS]])
 AC_SEARCH_LIBS([gluBeginCurve],[$1],
 	        [GLU_LIBS="${GLU_LIBS:-$ac_cv_search_gluBeginCurve}"])
  _AX_CHECK_GLU_RESTORE_FLAGS([[CFLAGS],[LIBS]])
])

# OSX specific GLU checks
AC_DEFUN([_AX_CHECK_DARWIN_GLU], [
  AC_REQUIRE([_WITH_XQUARTZ_GL])
  AS_IF([test "x$with_xquartz_gl" != "xno"],
        [GLU_LIBS="${GLU_LIBS:--lGLU}"],
        [GLU_LIBS="${GLU_LIBS:--framework OpenGL}"])
])

# AX_CHECK_GLU([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
# -----------------------------------------------------
# Checks GLU and provides hooks for success and failures
AC_DEFUN([AX_CHECK_GLU],[
  AC_REQUIRE([AC_CANONICAL_HOST])
  AC_REQUIRE([_WITH_XQUARTZ_GL])
  AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  AC_ARG_VAR([GLU_CFLAGS],[C compiler flags for GLU, overriding system check])
  AC_ARG_VAR([GLU_LIBS],[Linker flags for GLU, overriding system check])

  dnl Setup GLU_CFLAGS and GLU_LIBS
  AS_CASE([${host}],
          [*-darwin*],[_AX_CHECK_DARWIN_GLU],
          [*-cygwin*],[_AX_CHECK_GLU_SEARCH_LIBS([GLU glu MesaGLU glu32])
                       AC_CHECK_HEADERS([windows.h])],
          # try first native
 	  [*-mingw*],[_AX_CHECK_GLU_SEARCH_LIBS([glu32 GLU glu MesaGLU])
                      AC_CHECK_HEADERS([windows.h])],
          [PKG_PROG_PKG_CONFIG
           PKG_CHECK_MODULES([GLU],[glu],
           [],
           [_AX_CHECK_GLU_SEARCH_LIBS([GLU glu MesaGLU])])
          ])

  AS_CASE([$host],
          [*-darwin*],
            [AS_IF([test "X$with_xquartz_gl" = "Xno"],
                   [_AX_CHECK_GLU_HEADERS([OpenGL/glu.h])],
                   [_AX_CHECK_GLU_HEADERS([GL/glu.h])]
                   )],
          [_AX_CHECK_GLU_HEADERS([GL/glu.h])])

  dnl compile test
  AS_IF([test "X$ax_check_glu_have_headers" = "Xyes"],
        [AC_CACHE_CHECK([for compiling a minimal OpenGL Utility (GLU) program],
                        [ax_cv_check_glu_compile],
                        [_AX_CHECK_GLU_SAVE_FLAGS([CFLAGS])
                         AC_COMPILE_IFELSE([_AX_CHECK_GLU_PROGRAM],
                                           [ax_cv_check_glu_compile="yes"],
                                           [ax_cv_check_glu_compile="no"])
                         _AX_CHECK_GLU_RESTORE_FLAGS([CFLAGS])])
         ])

  dnl link test
  AS_IF([test "X$ax_cv_check_glu_compile" = "Xyes"],
        [AC_CACHE_CHECK([for linking a minimal GLU program],
                        [ax_cv_check_glu_link],
                        [_AX_CHECK_GLU_SAVE_FLAGS([[CFLAGS],[LIBS]])
                         AC_LINK_IFELSE([_AX_CHECK_GLU_PROGRAM],
                                        [ax_cv_check_glu_link="yes"],
                                        [ax_cv_check_glu_link="no"])
                         _AX_CHECK_GLU_RESTORE_FLAGS([[CFLAGS],[LIBS]])])
        ])

#
# Some versions of Mac OS X include a broken interpretation of the GLU
# tesselation callback function signature.
  AS_IF([test "X$ax_cv_check_glu_link" = "Xyes"],
        [AC_CACHE_CHECK([if GLU varargs tesselator is using non-standard form],
                        [ax_cv_varargs_glu_tesscb],
                        [_AX_CHECK_GLU_SAVE_FLAGS([CFLAGS],[C++])
                         AC_COMPILE_IFELSE([_AX_CHECK_GLU_VARARGS_TESSVB_PROGRAM],
                                           [ax_cv_varargs_glu_tesscb="yes"],
                                           [ax_cv_varargs_glu_tesscb="no"])
                         _AX_CHECK_GLU_RESTORE_FLAGS([CFLAGS],[C++])])
        AS_IF([test "X$ax_cv_varargs_glu_tesscb" = "yes"],
              [AC_DEFINE([HAVE_VARARGS_GLU_TESSCB], [1],
                         [Use nonstandard varargs form for the GLU tesselator callback])])
        ])

  dnl hook
  AS_IF([test "X$ax_cv_check_glu_link" = "Xyes"],
        [AC_DEFINE([HAVE_GLU],[1],[Defined if a valid GLU implementation is found.])
         m4_ifval([$1],
                  [$1],
                  [CFLAGS="$GLU_CFLAGS $CFLAGS"
                   LIBS="$GLU_LIBS $LIBS"])],
        [m4_ifval([$2],
                  [$2],
                  [AC_MSG_ERROR([Could not find a valid GLU implementation])])
        ])
])
