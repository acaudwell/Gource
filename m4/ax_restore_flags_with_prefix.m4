# =================================================================================
#  https://www.gnu.org/software/autoconf-archive/ax_restore_flags_with_prefix.html
# =================================================================================
#
# SYNOPSIS
#
#   AX_RESTORE_FLAGS_WITH_PREFIX(PREFIX, LIST-OF-FLAGS)
#
# DESCRIPTION
#
#   Restore the flags saved by AX_SAVE_FLAGS_WITH_PREFIX.
#
#   Expansion example: AX_RESTORE_FLAGS_WITH_PREFIX([GL], [[CFLAGS],[LIBS]])
#   expands to
#
#     CFLAGS="$gl_saved_flag_cflags"
#     LIBS="$gl_saved_flag_libs"
#
#   One common use case is to define a package specific wrapper macro around
#   this one, and also restore other variables if needed. For example:
#
#     AC_DEFUN([_AX_CHECK_GL_RESTORE_FLAGS], [
#       AX_RESTORE_FLAGS_WITH_PREFIX([GL],[$1])
#       AC_LANG_POP([C])
#     ])
#
#     # Restores CFLAGS, LIBS and language state
#     _AX_CHECK_GL_RESTORE_FLAGS([[CFLAGS],[LIBS]])
#
# LICENSE
#
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

#serial 3

AC_DEFUN([AX_RESTORE_FLAGS_WITH_PREFIX],[
m4_ifval([$2], [
m4_car($2)="$_ax_[]m4_tolower($1)_saved_flag_[]m4_tolower(m4_car($2))"
$0($1, m4_cdr($2))])
])
