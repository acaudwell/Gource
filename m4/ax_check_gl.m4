# ===========================================================================
#       https://www.gnu.org/software/autoconf-archive/ax_check_gl.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CHECK_GL([ACTION-IF-FOUND],[ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   Checks for an OpenGL implementation. If a valid OpenGL implementation is
#   found, this macro would set C preprocessor symbol HAVE_GL to 1.
#
#   If either a valid OpenGL header or library was not found, by default the
#   configuration would exits on error. This behavior can be overwritten by
#   providing a custom "ACTION-IF-NOT-FOUND" hook.
#
#   If the header, library was found, and been tested for compiling and
#   linking the configuration would export the required compiler flags to
#   "GL_CFLAGS" and "GL_LIBS". These two variables can also be overwritten
#   by user from the command line if they want to link against the library
#   they specified instead of having the configuration script to detect the
#   flags automatically. Note that having "GL_CFLAGS" or "GL_LIBS" set
#   doesn't mean it can compile or link with the flags, since it could be
#   overwritten by user. However the "HAVE_GL" symbol and "ACTION-IF-FOUND"
#   hook is always guaranteed to reflect a valid OpenGL implementation.
#
#   If user didn't specify the "ACTION-IF-FOUND" hook, the configuration
#   would prepend "GL_CFLAGS" and "GL_LIBS" to "CFLAGS" and "LIBS", like
#   many other autoconf macros do.
#
#   OpenGL is one of the libraries that has different header names on
#   different platforms. This macro does the header detection, and will
#   export the following symbol: "HAVE_GL_GL_H" for having "GL/gl.h" or
#   "HAVE_OPENGL_GL_H" for having "OpenGL/gl.h". To write a portable OpenGL
#   code, you should include OpenGL header like so:
#
#     #if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#     # include <windows.h>
#     #endif
#     #ifdef HAVE_GL_GL_H
#     # include <GL/gl.h>
#     #elif defined(HAVE_OPENGL_GL_H)
#     # include <OpenGL/gl.h>
#     #else
#     # error no gl.h
#     #endif
#
#   On the OSX platform, there's two possible OpenGL implementation. One is
#   the OpenGL that ships with OSX, the other comes with X11/XQuartz
#   (http://www.xquartz.org). To use the xquartz variant, user can use the
#   option --with-xquartz-gl[=path to xquartz root]. By default the
#   configuration will check "/opt/X11", which is the default X11 install
#   location on OSX.
#
# LICENSE
#
#   Copyright (c) 2009 Braden McDaniel <braden@endoframe.com>
#   Copyright (c) 2012 Bastien Roucaries <roucaries.bastien+autoconf@gmail.com>
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

#serial 22

# example gl program
m4_define([_AX_CHECK_GL_PROGRAM],
          [AC_LANG_PROGRAM([[
# if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#   include <windows.h>
# endif
# ifdef HAVE_GL_GL_H
#   include <GL/gl.h>
# elif defined(HAVE_OPENGL_GL_H)
#   include <OpenGL/gl.h>
# else
#   error no gl.h
# endif
]],[[glBegin(0)]])])

dnl Default include : add windows.h
dnl see http://www.opengl.org/wiki/Platform_specifics:_Windows
dnl (acceded 20120801)
AC_DEFUN([_AX_CHECK_GL_INCLUDES_DEFAULT],dnl
[
  AC_INCLUDES_DEFAULT
  [
  # if defined(HAVE_WINDOWS_H) && defined(_WIN32)
  #   include <windows.h>
  # endif
  ]
])


# _AX_CHECK_GL_SAVE_FLAGS(LIST-OF-FLAGS)
# Use this macro before you modify the flags.
# Restore the flags by _AX_CHECK_GL_RESTORE_FLAGS
#
# Example: _AX_CHECK_GL_SAVE_FLAGS([[CFLAGS],[LIBS]]) expands to
# gl_saved_flag_cflags=$CFLAGS
# gl_saved_flag_libs=$LIBS
# CFLAGS="$GL_CFLAGS $CFLAGS"
# LIBS="$GL_LIBS $LIBS"
AC_DEFUN([_AX_CHECK_GL_SAVE_FLAGS], [
 AX_SAVE_FLAGS_WITH_PREFIX([GL],[$1])
 AC_LANG_PUSH([C])
])

# _AX_CHECK_GL_RESTORE_FLAGS(LIST-OF-FLAGS)
# Use this marcro to restore the flags you saved using
# _AX_CHECK_GL_SAVE_FLAGS
#
# Example: _AX_CHECK_GL_RESTORE_FLAGS([[CFLAGS],[LIBS]]) expands to
# CFLAGS="$gl_saved_flag_cflags"
# LIBS="$gl_saved_flag_libs"
AC_DEFUN([_AX_CHECK_GL_RESTORE_FLAGS], [
 AX_RESTORE_FLAGS_WITH_PREFIX([GL],[$1])
 AC_LANG_POP([C])
])

# Check if the program compiles
AC_DEFUN([_AX_CHECK_GL_COMPILE],
[dnl
 _AX_CHECK_GL_SAVE_FLAGS([CFLAGS])
 AC_COMPILE_IFELSE([_AX_CHECK_GL_PROGRAM],
                   [ax_check_gl_compile_opengl="yes"],
                   [ax_check_gl_compile_opengl="no"])
 _AX_CHECK_GL_RESTORE_FLAGS([CFLAGS])
])

# Compile the example program (cache)
AC_DEFUN([_AX_CHECK_GL_COMPILE_CV],
[dnl
 AC_CACHE_CHECK([for compiling a minimal OpenGL program],[ax_cv_check_gl_compile_opengl],
                [_AX_CHECK_GL_COMPILE()
                 ax_cv_check_gl_compile_opengl="${ax_check_gl_compile_opengl}"])
 ax_check_gl_compile_opengl="${ax_cv_check_gl_compile_opengl}"
])

# Link the example program
AC_DEFUN([_AX_CHECK_GL_LINK],
[dnl
 _AX_CHECK_GL_SAVE_FLAGS([[CFLAGS],[LIBS],[LDFLAGS]])
 AC_LINK_IFELSE([_AX_CHECK_GL_PROGRAM],
                [ax_check_gl_link_opengl="yes"],
                [ax_check_gl_link_opengl="no"])
 _AX_CHECK_GL_RESTORE_FLAGS([[CFLAGS],[LIBS],[LDFLAGS]])
])

# Link the example program (cache)
AC_DEFUN([_AX_CHECK_GL_LINK_CV],
[dnl
 AC_CACHE_CHECK([for linking a minimal OpenGL program],[ax_cv_check_gl_link_opengl],
                [_AX_CHECK_GL_LINK()
                 ax_cv_check_gl_link_opengl="${ax_check_gl_link_opengl}"])
 ax_check_gl_link_opengl="${ax_cv_check_gl_link_opengl}"
])


# _AX_CHECK_GL_MANUAL_LIBS_GENERIC(LIBRARIES-TO-SEARCH)
# Searches library provided in $1, and output the flag
# $ax_check_gl_lib_opengl
AC_DEFUN([_AX_CHECK_GL_MANUAL_LIBS_GENERIC], [
  AS_IF([test -n "$GL_LIBS"],[], [
    ax_check_gl_manual_libs_generic_extra_libs="$1"
    AS_IF([test "X$ax_check_gl_manual_libs_generic_extra_libs" = "X"],
          [AC_MSG_ERROR([AX_CHECK_GL_MANUAL_LIBS_GENERIC argument must no be empty])])

    _AX_CHECK_GL_SAVE_FLAGS([CFLAGS])
    AC_SEARCH_LIBS([glBegin],[$ax_check_gl_manual_libs_generic_extra_libs], [
                   ax_check_gl_lib_opengl="yes"
                   break
                   ])
    AS_IF([test "X$ax_check_gl_lib_opengl"="Xyes"],
          [GL_LIBS="${ac_cv_search_glBegin}"])
    _AX_CHECK_GL_RESTORE_FLAGS([CFLAGS])
 ])
])

# _WITH_XQUARTZ_GL
# ----------------
# Provides an option in command line to specify the XQuartz installation
# path on OSX, so that user can link to it instead of using the default
# OSX OpenGL framework. (Mac OSX only)
AC_DEFUN_ONCE([_WITH_XQUARTZ_GL],[
  AC_ARG_WITH([xquartz-gl],
   [AS_HELP_STRING([--with-xquartz-gl@<:@=DIR@:>@],
                   [On Mac OSX, use opengl provided by X11/XQuartz instead of the built-in framework.
                    If enabled, the default location is @<:@DIR=/opt/X11@:>@.
                    This option is default to false.])],
   [AS_IF([test "X$with_xquartz_gl"="Xyes"],
          [with_xquartz_gl="/opt/X11"])],
   [with_xquartz_gl=no])
  AS_IF([test "X$with_xquartz_gl" != "Xno"],
        [AC_MSG_CHECKING([OSX X11 path])
         AS_IF([test -e "$with_xquartz_gl"],
               [AC_MSG_RESULT(["$with_xquartz_gl"])
                CFLAGS="-I$with_xquartz_gl/include $CFLAGS"
                LIBS="-L$with_xquartz_gl/lib $LIBS"
               ],
               [with_xquartz_gl=no
                AC_MSG_RESULT([no])
                AC_MSG_WARN([--with-xquartz-gl was given, but test for X11 failed. Fallback to system framework])
               ])
        ])
])

# OSX specific setup for OpenGL check
AC_DEFUN([_AX_CHECK_DARWIN_GL], [
 AC_REQUIRE([_WITH_XQUARTZ_GL])
 AS_IF([test "x$with_xquartz_gl" != "xno"],
       [GL_LIBS="${GL_LIBS:--lGL}"],
       [GL_LIBS="${GL_LIBS:--framework OpenGL}"])
])


# AX_CHECK_GL_LIB([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
# Checks OpenGL headers and library and provides hooks for success and failures.
# When $1 is not set, this macro would modify CFLAGS and LIBS environment variables.
# However, user can override this behavior by providing their own hooks.
# The CFLAGS and LIBS flags required by OpenGL is always exported in
# GL_CFLAGS and GL_LIBS environment variable.
#
# In other words, the default behavior of AX_CHECK_GL_LIB() is equivalent to
# AX_CHECK_GL_LIB(
#   [CFLAGS="$GL_CFLAGS $CFLAGS"
#    LIBS="$GL_LIBS $LIBS"]
# )
AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_CANONICAL_HOST])
 AC_REQUIRE([PKG_PROG_PKG_CONFIG])
 AC_ARG_VAR([GL_CFLAGS],[C compiler flags for GL, overriding configure script defaults])
 AC_ARG_VAR([GL_LIBS],[Linker flags for GL, overriding configure script defaults])

 dnl --with-gl or not can be implemented outside of check-gl
 AS_CASE([${host}],
         [*-darwin*],[_AX_CHECK_DARWIN_GL],
         dnl some windows may support X11 opengl, and should be able to linked
         dnl by -lGL. However I have no machine to test it.
         [*-cygwin*|*-mingw*],[
          _AX_CHECK_GL_MANUAL_LIBS_GENERIC([opengl32 GL gl])
          AC_CHECK_HEADERS([windows.h])
          ],
         [PKG_PROG_PKG_CONFIG
          PKG_CHECK_MODULES([GL],[gl],
          [],
          [_AX_CHECK_GL_MANUAL_LIBS_GENERIC([GL gl])])
         ]) dnl host specific checks

 dnl this was cache
 _AX_CHECK_GL_SAVE_FLAGS([CFLAGS])
 AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h],
   [ax_check_gl_have_headers="yes";break])
 _AX_CHECK_GL_RESTORE_FLAGS([CFLAGS])

 AS_IF([test "X$ax_check_gl_have_headers" = "Xyes"],
       [_AX_CHECK_GL_COMPILE_CV()],
       [no_gl=yes])
 AS_IF([test "X$ax_check_gl_compile_opengl" = "Xyes"],
       [_AX_CHECK_GL_LINK_CV()],
       [no_gl=yes])
 AS_IF([test "X$no_gl" = "X"],
   [AC_DEFINE([HAVE_GL], [1], [Defined if a valid OpenGL implementation is found.])
    m4_ifval([$1],
      [$1],
      [CFLAGS="$GL_CFLAGS $CFLAGS"
       LIBS="$GL_LIBS $LIBS"])
   ],
   [m4_ifval([$2],
     [$2],
     [AC_MSG_ERROR([Could not find a valid OpenGL implementation])])
   ])
])
