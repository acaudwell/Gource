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
# version: 2.6
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
AC_REQUIRE([AX_PTHREAD])dnl

AC_LANG_PUSH([C])
AX_LANG_COMPILER_MS
AS_IF([test X$ax_compiler_ms = Xno],
      [GL_CFLAGS="${PTHREAD_CFLAGS}"; GL_LIBS="${PTHREAD_LIBS}"])

#
# Use x_includes and x_libraries if they have been set (presumably by
# AC_PATH_X).
#
AS_IF([test X$no_x != Xyes -a -n "$x_includes"],
      [GL_CFLAGS="-I$x_includes $GL_CFLAGS"])

AC_CHECK_HEADERS([windows.h])

ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GL_CFLAGS $CPPFLAGS"
AC_CHECK_HEADERS([GL/gl.h OpenGL/gl.h], , , [
# if defined(HAVE_WINDOWS_H) && defined(_WIN32)
#   include <windows.h>
# endif
])
CPPFLAGS=$ax_save_CPPFLAGS

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

m4_define([AX_CHECK_GL_GLX_PROGRAM],
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
                           [[glXQueryVersion(0, 0, 0)]])])

AC_CACHE_CHECK([for OpenGL library], [ax_cv_check_gl_libgl],
[ax_cv_check_gl_libgl=no
case $host_cpu in
  x86_64) ax_check_gl_libdir=lib64 ;;
  *)      ax_check_gl_libdir=lib ;;
esac
ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $GL_CFLAGS"
ax_save_LDFLAGS=$LDFLAGS
AS_IF([test X$no_x != Xyes -a -n "$x_libraries"],
      [LDFLAGS="$LDFLAGS -L$x_libraries"])
ax_save_LIBS=$LIBS
ax_check_libs="-lopengl32 -lGL"
for ax_lib in $ax_check_libs; do
  AS_IF([test X$ax_compiler_ms = Xyes],
        [ax_try_lib=`echo $ax_lib | $SED -e 's/^-l//' -e 's/$/.lib/'`],
        [ax_try_lib=$ax_lib])
  LDFLAGS="$ax_save_LDFLAGS $GL_LIBS"
  LIBS="$ax_try_lib $ax_save_LIBS"
AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
               [ax_cv_check_gl_libgl=$ax_try_lib; break],
               [ax_check_gl_nvidia_flags="-L/usr/$ax_check_gl_libdir/nvidia"
                LDFLAGS="$ax_save_LDFLAGS $GL_LIBS $ax_check_gl_nvidia_flags"
                AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                               [ax_cv_check_gl_libgl="$ax_check_gl_nvidia_flags $ax_try_lib"; break],
                               [ax_check_gl_dylib_flag='-dylib_file /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib'
                                LDFLAGS="$ax_save_LDFLAGS $GL_LIBS $ax_check_gl_dylib_flag"
                                AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                                               [ax_cv_check_gl_libgl="$ax_check_gl_dylib_flag $ax_try_lib"; break])])])
done

#
# If no_x is "yes", we don't want to wind up using a libGL that is
# linked with X11.  Test to see if the found libGL includes GLX
# functions.  If it does and no_x is "yes", we want to reset
# ax_cv_check_gl_libgl back to "no".
#
# Note that LDFLAGS and LIBS should still have whatever values they
# had when we broke out of the test loop above; use that.
#
AS_IF([test "X$ax_cv_check_gl_libgl" != Xno],
      [AC_LINK_IFELSE([AX_CHECK_GL_GLX_PROGRAM],
                      [AS_IF([test X$no_x = Xyes],
                             [ax_cv_check_gl_libgl=no])])])

LIBS=$ax_save_LIBS
AS_IF([test "X$ax_cv_check_gl_libgl" = Xno -a X$no_x = Xyes],
      [LDFLAGS="$ax_save_LDFLAGS -framework OpenGL"
      AC_LINK_IFELSE([AX_CHECK_GL_PROGRAM],
                     [ax_cv_check_gl_libgl='-framework OpenGL'])])

LDFLAGS=$ax_save_LDFLAGS
CPPFLAGS=$ax_save_CPPFLAGS])

AS_IF([test "X$ax_cv_check_gl_libgl" = Xno],
      [no_gl=yes; GL_CFLAGS=""; GL_LIBS=""],
      [GL_LIBS="$ax_cv_check_gl_libgl $GL_LIBS"])
AC_LANG_POP([C])

AC_SUBST([GL_CFLAGS])
AC_SUBST([GL_LIBS])
])dnl
