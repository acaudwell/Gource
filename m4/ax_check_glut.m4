# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_check_glut.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GLUT([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks for GLUT. If a valid GLUT implementation is found, the configure
#   script would export the C preprocessor symbol "HAVE_GLUT=1".
#
#   If either a valid GLUT header or library was not found, by default the
#   configure script would exit on error. This behavior can be overwritten
#   by providing a custom "ACTION-IF-NOT-FOUND" hook.
#
#   If the header, library was found, and been tested for compiling and
#   linking the configuration would export the required compiler flags to
#   "GLUT_CFLAGS" and "GLUT_LIBS" environment variables. These two variables
#   can also be overwritten by defining the environment variables before
#   executing the configure program. If it was predefined, configure would
#   not try to overwrite it, but it would still perform the compile and link
#   test. Only when the tests succeeded does the configure script to export
#   "HAVE_GLUT=1" and to run "ACTION-IF-FOUND" hook.
#
#   If user didn't specify the "ACTION-IF-FOUND" hook, the configuration
#   would prepend "GLUT_CFLAGS" and "GLUT_LIBS" to "CFLAGS" and "LIBS", like
#   many other autoconf macros do.
#
#   If the header "GL/glut.h" is found, "HAVE_GL_GLUT_H" is defined. If the
#   header "GLUT/glut.h" is found, HAVE_GLUT_GLUT_H is defined.
#
#   You should use something like this in your headers:
#
#     # if HAVE_WINDOWS_H && defined(_WIN32)
#     #  include <windows.h>
#     # endif
#     # if defined(HAVE_GL_GLUT_H)
#     #  include <GL/glut.h>
#     # elif defined(HAVE_GLUT_GLUT_H)
#     #  include <GLUT/glut.h>
#     # else
#     #  error no glut.h
#     # endif
#
#   On the OSX platform, you can use the option --with-xquartz-gl to use
#   X11/Xquartz GLUT implementation instead of the system built in GLUT
#   framework.
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

#serial 17

AC_DEFUN([_AX_CHECK_GLUT_SAVE_FLAGS], [
 AX_SAVE_FLAGS_WITH_PREFIX([GLUT],[$1]) dnl defined in ax_check_gl
 AC_LANG_PUSH([C])
])

AC_DEFUN([_AX_CHECK_GLUT_RESTORE_FLAGS], [
 AX_RESTORE_FLAGS_WITH_PREFIX([GLUT],[$1]) dnl defined in ax_check_gl
 AC_LANG_POP([C])
])

dnl Default include : add windows.h
dnl see http://www.opengl.org/wiki/Platform_specifics:_Windows
dnl (acceded 20120801)
AC_DEFUN([_AX_CHECK_GLUT_INCLUDES_DEFAULT],dnl
[
  AC_INCLUDES_DEFAULT
  [
  # if defined(HAVE_WINDOWS_H) && defined(_WIN32)
  #   include <windows.h>
  # endif
  ]
])

m4_define([_AX_CHECK_GLUT_PROGRAM],
          [AC_LANG_PROGRAM([[
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# ifdef HAVE_GL_GLUT_H
#   include <GL/glut.h>
# elif defined(HAVE_GLUT_GLUT_H)
#   include <GLUT/glut.h>
# else
#   error no glut.h
# endif]],
[[glutMainLoop()]])])


# _AX_CHECK_GLUT_MANUAL_LIBS_GENERIC(LIST-OF-LIBS)
# ------------------------------------------------
# Searches libraries provided in $1, and export variable
# $ax_check_glut_lib_glut
AC_DEFUN([_AX_CHECK_GLUT_MANUAL_LIBS_GENERIC],
[
 _AX_CHECK_GLUT_SAVE_FLAGS([[CFLAGS],[LIBS]])
 AC_SEARCH_LIBS([glutMainLoop],[$1],
                [GLUT_LIBS="${GLUT_LIBS:-$ac_cv_search_glutMainLoop}"])
 _AX_CHECK_GLUT_RESTORE_FLAGS([[CFLAGS],[LIBS]])
])

# Wrapper macro to check GLUT header
AC_DEFUN([_AX_CHECK_GLUT_HEADER],[
  _AX_CHECK_GLUT_SAVE_FLAGS([CFLAGS])
  AC_CHECK_HEADERS([$1],
                   [ax_check_glut_have_headers=yes])
  _AX_CHECK_GLUT_RESTORE_FLAGS([CFLAGS])
])


# AX_CHECK_GLUT_LIB([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
# Checks GLUT headers and library and provides hooks for success and failures.
AC_DEFUN([AX_CHECK_GLUT],
[AC_REQUIRE([AC_CANONICAL_HOST])
 AC_REQUIRE([_WITH_XQUARTZ_GL])
 AC_ARG_VAR([GLUT_CFLAGS],[C compiler flags for GLUT, overriding configure script defaults])
 AC_ARG_VAR([GLUT_LIBS],[Linker flags for GLUT, overriding configure script defaults])

 AS_CASE([${host}],
         [*-darwin*],[AS_IF([test "x$with_xquartz_gl" != "xno"],
                            [GLUT_LIBS="${GLUT_LIBS:--lGLUT}"],
                            [GLUT_LIBS="${GLUT_LIBS:--framework GLUT}"])],
         [*-cygwin*|*-mingw*],[
            _AX_CHECK_GLUT_MANUAL_LIBS_GENERIC([glut32 glut])
            AC_CHECK_HEADERS([windows.h])
          ],
         [_AX_CHECK_GLUT_MANUAL_LIBS_GENERIC([glut])
         ]) dnl host specific checks

 dnl checks header
 AS_CASE([${host}],
   [*-darwin*],[AS_IF([test "x$with_xquartz_gl" = "xno"],
                      [_AX_CHECK_GLUT_HEADER([GLUT/glut.h])],
                      [_AX_CHECK_GLUT_HEADER([GL/glut.h])]
                      )],
   [_AX_CHECK_GLUT_HEADER([GL/glut.h])])

 dnl compile
 AS_IF([test "X$ax_check_glut_have_headers" = "Xyes"],
       [AC_CACHE_CHECK([for compiling a minimal GLUT program],
                       [ax_cv_check_glut_compile],
                       [_AX_CHECK_GLUT_SAVE_FLAGS([CFLAGS])
                        AC_COMPILE_IFELSE([_AX_CHECK_GLUT_PROGRAM],
                                          [ax_cv_check_glut_compile="yes"],
                                          [ax_cv_check_glut_compile="no"])
                        _AX_CHECK_GLUT_RESTORE_FLAGS([CFLAGS])
                       ])
      ])

 dnl link
 AS_IF([test "X$ax_cv_check_glut_compile" = "Xyes"],
       [AC_CACHE_CHECK([for linking a minimal GLUT program],
                       [ax_cv_check_glut_link],
                       [_AX_CHECK_GLUT_SAVE_FLAGS([[CFLAGS],[LIBS]])
                        AC_LINK_IFELSE([_AX_CHECK_GLUT_PROGRAM],
                                       [ax_cv_check_glut_link="yes"],
                                       [ax_cv_check_glut_link="no"])
                        _AX_CHECK_GLUT_RESTORE_FLAGS([[CFLAGS],[LIBS]])
                       ])
       ])

 dnl hook
 AS_IF([test "X$ax_cv_check_glut_link" = "Xyes"],
   [AC_DEFINE([HAVE_GLUT], [1], [Defined if a valid GLUT implementation is found])
    m4_ifval([$1],
     [$1],
     [CFLAGS="$GLUT_CFLAGS $CFLAGS"
      LIBS="$GLUT_LIBS $LIBS"])
   ],
   [m4_ifval([$2],
     [$2],
     [AC_MSG_ERROR([Could not find a valid GLUT implementation])]
     )
   ])

])
