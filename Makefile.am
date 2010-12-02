ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = gource

gource_SOURCES = \
	src/action.cpp src/action.h \
	src/apache.cpp src/apache.h \
        src/bzr.cpp src/bzr.h \
	src/commitlog.cpp src/commitlog.h \
	src/core/bounds.h \
	src/core/camera.cpp src/core/camera.h \
	src/core/conffile.cpp src/core/conffile.h \
	src/core/display.cpp src/core/display.h \
	src/core/frustum.cpp src/core/frustum.h \
	src/core/fxfont.cpp src/core/fxfont.h \
	src/core/logger.cpp src/core/logger.h \
	src/core/mousecursor.cpp src/core/mousecursor.h \
	src/core/pi.h \
	src/core/plane.cpp src/core/plane.h \
	src/core/ppm.cpp src/core/ppm.h \
	src/core/quadtree.cpp src/core/quadtree.h \
	src/core/regex.cpp src/core/regex.h \
	src/core/resource.cpp src/core/resource.h \
	src/core/sdlapp.cpp src/core/sdlapp.h \
	src/core/seeklog.cpp src/core/seeklog.h \
	src/core/settings.cpp src/core/settings.h \
	src/core/stringhash.cpp src/core/stringhash.h \
	src/core/texture.cpp src/core/texture.h \
	src/core/utf8/checked.h \
	src/core/utf8/core.h \
	src/core/utf8/unchecked.h \
	src/core/utf8/utf8.h \
	src/core/vectors.h \
	src/custom.cpp src/custom.h \
	src/cvs-exp.cpp src/cvs-exp.h \
	src/dirnode.cpp src/dirnode.h \
	src/file.cpp src/file.h \
	src/git.cpp src/git.h \
	src/gitraw.cpp src/gitraw.h \
	src/gource.cpp src/gource.h \
	src/gource_shell.cpp src/gource_shell.h \
	src/gource_settings.cpp src/gource_settings.h \
	src/hg.cpp src/hg.h \
	src/main.cpp src/main.h \
	src/pawn.cpp src/pawn.h \
	src/slider.cpp src/slider.h \
	src/spline.cpp src/spline.h \
	src/svn.cpp src/svn.h \
	src/tinyxml/tinyxmlerror.cpp \
	src/tinyxml/tinystr.cpp src/tinyxml/tinystr.h \
	src/tinyxml/tinyxml.cpp src/tinyxml/tinyxml.h \
	src/tinyxml/tinyxmlparser.cpp src/tinyxml/tinyxmlparser.h \
	src/user.cpp src/user.h \
	src/zoomcamera.cpp src/zoomcamera.h

CPPFLAGS = -DSDLAPP_RESOURCE_DIR=\"$(pkgdatadir)\"

dist_pkgdata_DATA = data/beam.png data/bloom.tga data/bloom_alpha.tga data/cursor.png data/file.png data/no_photo.png data/gource.style

install-data-hook:
	mkdir -p -m 755 ${DESTDIR}/$(mandir)/man1
	gzip -cf9 data/gource.1 > $(DESTDIR)$(mandir)/man1/gource.1.gz

uninstall-hook:
	rm -f $(DESTDIR)$(mandir)/man1/gource.1.gz

if FONTDIR
CPPFLAGS += -DSDLAPP_FONT_DIR=\"$(gourcefontdir)\"
else
fontsdir = $(pkgdatadir)/fonts
dist_fonts_DATA = data/fonts/README data/fonts/FreeSans.ttf
endif

