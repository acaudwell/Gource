# -*- mode: autoconf -*-
#
# AX_CHECK_GLUT
#
# Check for GLUT.  If GLUT is found, the required compiler and linker flags
# are included in the output variables "GLUT_CFLAGS" and "GLUT_LIBS",
# respectively.  If GLUT is not found, "no_glut" is set to "yes".
#
# If the header "GL/glut.h" is found, "HAVE_GL_GLUT_H" is defined.  If the
# header "GLUT/glut.h" is found, HAVE_GLUT_GLUT_H is defined.  These
# preprocessor definitions may not be mutually exclusive.
#
# version: 2.2
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
AC_DEFUN([AX_CHECK_GLUT],
[AC_REQUIRE([AX_CHECK_GLU])dnl
AC_REQUIRE([AC_PATH_XTRA])dnl

ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GLU_CFLAGS $CPPFLAGS"
AC_CHECK_HEADERS([GL/glut.h GLUT/glut.h])
CPPFLAGS=$ax_save_CPPFLAGS

GLUT_CFLAGS=$GLU_CFLAGS

m4_define([AX_CHECK_GLUT_PROGRAM],
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

AC_CACHE_CHECK([for GLUT library], [ax_cv_check_glut_libglut],
[ax_cv_check_glut_libglut=no
#
# If X is present, assume GLUT depends on it.
#
AC_LANG_PUSH(C)
ax_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$GLUT_CFLAGS $CPPFLAGS"
ax_save_LDFLAGS=$LDFLAGS
ax_save_LIBS=$LIBS
LIBS=""
AS_IF([test X$no_x != Xyes],
      [ax_check_glut_x_libs="$X_PRE_LIBS -lXmu -lXi $X_EXTRA_LIBS"])
ax_check_libs="-lglut32 -lglut"
for ax_lib in $ax_check_libs; do
  AS_IF([test X$ax_compiler_ms = Xyes],
        [ax_try_lib=`echo $ax_lib | $SED -e 's/^-l//' -e 's/$/.lib/'`],
        [ax_try_lib=$ax_lib])
  LIBS="$ax_try_lib $GLUT_LIBS $ax_check_glut_x_libs $ax_save_LIBS"
  AC_LINK_IFELSE([AX_CHECK_GLUT_PROGRAM],
                 [ax_cv_check_glut_libglut=$ax_try_lib; break])
done

LIBS=$ax_save_LIBS
AS_IF([test "X$ax_cv_check_glut_libglut" = Xno -a X$no_x = Xyes],
      [LDFLAGS="$ax_save_LDFLAGS -framework GLUT"
      AC_LINK_IFELSE([AX_CHECK_GLUT_PROGRAM],
                     [ax_cv_check_glut_libglut='-framework GLUT'])])

LDFLAGS=$ax_save_LDFLAGS
CPPFLAGS=$ax_save_CPPFLAGS
AC_LANG_POP(C)])

AS_IF([test "X$ax_cv_check_glut_libglut" = Xno],
      [no_glut=yes; GLUT_CFLAGS=""; GLUT_LIBS=""],
      [GLUT_LIBS="$ax_cv_check_glut_libglut"])

AC_SUBST([GLUT_CFLAGS])
AC_SUBST([GLUT_LIBS])
])dnl
