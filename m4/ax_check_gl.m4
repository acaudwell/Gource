# -*- mode: autoconf -*-
#
# AX_CHECK_GL
#
# Check for an OpenGL implementation.  If GL is found, the required compiler
# and linker flags are included in the output variables "GL_CFLAGS" and
# "GL_LIBS", respectively.  If no usable GL implementation is found, "no_gl"
# is set to "yes".
#
# If the header "GL/gl.h" is found, "HAVE_GL_GL_H" is defined.  If the header
# "OpenGL/gl.h" is found, HAVE_OPENGL_GL_H is defined.  These preprocessor
# definitions may not be mutually exclusive.
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
AC_DEFUN([AX_CHECK_GL],
[AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_REQUIRE([AC_PATH_X])dnl
AC_REQUIRE([AC_PROG_SED])dnl
AC_REQUIRE([ACX_PTHREAD])dnl

AC_LANG_PUSH([C])
AX_LANG_COMPILER_MS
AS_IF([test X$ax_compiler_ms = Xno],
      [GL_CFLAGS="${PTHREAD_CFLAGS}"; GL_LIBS="${PTHREAD_LIBS} -lm"])

#
# Use x_includes and x_libraries if they have been set (presumably by
# AC_PATH_X).
#
AS_IF([test X$no_x != Xyes],
      [AS_IF([test -n "$x_includes"],
             [GL_CFLAGS="-I$x_includes $GL_CFLAGS"])]
       AS_IF([test -n "$x_libraries"],
             [GL_LIBS="-L$x_libraries -lX11 $GL_LIBS"]))

ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GL_CFLAGS $CPPFLAGS"
AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h])
CPPFLAGS=$ax_save_CPPFLAGS

AC_CHECK_HEADERS([windows.h])

m4_define([AX_CHECK_GL_PROGRAM],
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
# endif]],
                           [[glBegin(0)]])])

AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
[ax_cv_check_gl_libgl=no
case $host_cpu in
  x86_64) ax_check_gl_libdir=lib64 ;;
  *)      ax_check_gl_libdir=lib ;;
esac
ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GL_CFLAGS $CPPFLAGS"
ax_save_LIBS=$LIBS
LIBS=""
ax_check_libs="-lopengl32 -lGL"
for ax_lib in $ax_check_libs; do
  AS_IF([test X$ax_compiler_ms = Xyes],
        [ax_try_lib=`echo $ax_lib | $SED -e 's/^-l//' -e 's/$/.lib/'`],
        [ax_try_lib=$ax_lib])
  LIBS="$ax_try_lib $GL_LIBS $ax_save_LIBS"
AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
               [ax_cv_check_gl_libgl=$ax_try_lib; break],
               [ax_check_gl_nvidia_flags="-L/usr/$ax_check_gl_libdir/nvidia" LIBS="$ax_try_lib $ax_check_gl_nvidia_flags $GL_LIBS $ax_save_LIBS"
               AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                              [ax_cv_check_gl_libgl="$ax_try_lib $ax_check_gl_nvidia_flags"; break],
                              [ax_check_gl_dylib_flag='-dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib' LIBS="$ax_try_lib $ax_check_gl_dylib_flag $GL_LIBS $ax_save_LIBS"
                              AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                                             [ax_cv_check_gl_libgl="$ax_try_lib $ax_check_gl_dylib_flag"; break])])])
done

AS_IF([test "X$ax_cv_check_gl_libgl" = Xno -a X$no_x = Xyes],
      [LIBS='-framework OpenGL'
      AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                     [ax_cv_check_gl_libgl=$LIBS])])

LIBS=$ax_save_LIBS
CPPFLAGS=$ax_save_CPPFLAGS])

AS_IF([test "X$ax_cv_check_gl_libgl" = Xno],
      [no_gl=yes; GL_CFLAGS=""; GL_LIBS=""],
      [GL_LIBS="$ax_cv_check_gl_libgl $GL_LIBS"])
AC_LANG_POP([C])

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl
