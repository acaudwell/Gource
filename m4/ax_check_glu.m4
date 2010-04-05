# -*- mode: autoconf -*-
#
# AX_CHECK_GLU
#
# Check for GLU.  If GLU is found, the required preprocessor and linker flags
# are included in the output variables "GLU_CFLAGS" and "GLU_LIBS",
# respectively.  If no GLU implementation is found, "no_glu" is set to "yes".
#
# If the header "GL/glu.h" is found, "HAVE_GL_GLU_H" is defined.  If the
# header "OpenGL/glu.h" is found, HAVE_OPENGL_GLU_H is defined.  These
# preprocessor definitions may not be mutually exclusive.
#
# Some implementations (in particular, some versions of Mac OS X) are known
# to treat the GLU tesselator callback function type as "GLvoid (*)(...)"
# rather than the standard "GLvoid (*)()".  If the former condition is
# detected, this macro defines "HAVE_VARARGS_GLU_TESSCB".
#
# version: 2.4
# author: Braden McDaniel <braden@endoframe.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#
# As a special exception, the you may copy, distribute and modify the
# configure scripts that are the output of Autoconf when processing
# the Macro.  You need not follow the terms of the GNU General Public
# License when using or distributing such scripts.
#
AC_DEFUN([AX_CHECK_GLU],
[AC_REQUIRE([AX_CHECK_GL])dnl
AC_REQUIRE([AC_PROG_CXX])dnl
GLU_CFLAGS=$GL_CFLAGS

ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GL_CFLAGS $CPPFLAGS"
AC_CHECK_HEADERS([GL/glu.h OpenGL/glu.h], , , [
# if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#   include <windows.h>
# endif
])
CPPFLAGS=$ax_save_CPPFLAGS

m4_define([AX_CHECK_GLU_PROGRAM],
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
# endif]],
                           [[gluBeginCurve(0)]])])

AC_CACHE_CHECK([for OpenGL Utility library], [ax_cv_check_glu_libglu],
[ax_cv_check_glu_libglu=no
ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GL_CFLAGS $CPPFLAGS"
ax_save_LDFLAGS=$LDFLAGS
ax_save_LIBS=$LIBS

#
# First, check for the possibility that everything we need is already in
# GL_LIBS.
#
LDFLAGS="$ax_save_LDFLAGS $GL_LIBS"
#
# libGLU typically links with libstdc++ on POSIX platforms.
# However, setting the language to C++ means that test program
# source is named "conftest.cc"; and Microsoft cl doesn't know what
# to do with such a file.
#
AC_LANG_PUSH([C++])
AS_IF([test X$ax_compiler_ms = Xyes],
      [AC_LANG_PUSH([C])])
AC_LINK_IFELSE([AX_CHECK_GLU_PROGRAM],
               [ax_cv_check_glu_libglu=yes],
               [LIBS=""
                ax_check_libs="-lglu32 -lGLU"
                for ax_lib in ${ax_check_libs}; do
                  AS_IF([test X$ax_compiler_ms = Xyes],
                        [ax_try_lib=`echo $ax_lib | $SED -e 's/^-l//' -e 's/$/.lib/'`],
                        [ax_try_lib=$ax_lib])
                  LIBS="$ax_try_lib $ax_save_LIBS"
                  AC_LINK_IFELSE([AX_CHECK_GLU_PROGRAM],
                                 [ax_cv_check_glu_libglu=$ax_try_lib; break])
                done])
AS_IF([test X$ax_compiler_ms = Xyes],
      [AC_LANG_POP([C])])
AC_LANG_POP([C++])

LIBS=$ax_save_LIBS
LDFLAGS=$ax_save_LDFLAGS
CPPFLAGS=$ax_save_CPPFLAGS])
AS_IF([test "X$ax_cv_check_glu_libglu" = Xno],
      [no_glu=yes; GLU_CFLAGS=""; GLU_LIBS=""],
      [AS_IF([test "X$ax_cv_check_glu_libglu" = Xyes],
             [GLU_LIBS=""],
             [GLU_LIBS="$ax_cv_check_glu_libglu"])])
AC_SUBST([GLU_CFLAGS])
AC_SUBST([GLU_LIBS])

#
# Some versions of Mac OS X include a broken interpretation of the GLU
# tesselation callback function signature when using the C++ compiler.
#
AS_IF([test "X$ax_cv_check_glu_libglu" != Xno],
      [AC_CACHE_CHECK([for varargs GLU tesselator callback function type],
                      [ax_cv_varargs_glu_tesscb],
                      [AC_LANG_PUSH([C++])
                      ax_cv_varargs_glu_tesscb=no
                      ax_save_CXXFLAGS=$CXXFLAGS
                      CXXFLAGS="$GL_CFLAGS $CXXFLAGS"
                      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
# ifdef HAVE_GL_GLU_H
#   include <GL/glu.h>
# else
#   include <OpenGL/glu.h>
# endif]],
                                        [[GLvoid (*func)(...); gluTessCallback(0, 0, func)]])],
                                        [ax_cv_varargs_glu_tesscb=yes])
                      CXXFLAGS=$ax_save_CXXFLAGS
                      AC_LANG_POP([C++])])
      AS_IF([test X$ax_cv_varargs_glu_tesscb = Xyes],
            [AC_DEFINE([HAVE_VARARGS_GLU_TESSCB], [1],
                       [Use nonstandard varargs form for the GLU tesselator callback])])])
])
