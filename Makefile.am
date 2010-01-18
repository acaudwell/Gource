ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = gource

gource_SOURCES = \
	src/action.cpp src/action.h \
	src/apache.cpp src/apache.h \
	src/commitlog.cpp src/commitlog.h \
	src/core/bounds.h \
	src/core/camera.cpp src/core/camera.h \
	src/core/display.cpp src/core/display.h \
	src/core/extensions.cpp src/core/extensions.h \
	src/core/frustum.cpp src/core/frustum.h \
	src/core/fxfont.cpp src/core/fxfont.h \
	src/core/logger.cpp src/core/logger.h \
	src/core/pi.h \
	src/core/plane.cpp src/core/plane.h \
	src/core/quadtree.cpp src/core/quadtree.h \
	src/core/regex.cpp src/core/regex.h \
	src/core/resource.cpp src/core/resource.h \
	src/core/sdlapp.cpp src/core/sdlapp.h \
	src/core/seeklog.cpp src/core/seeklog.h \
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
	src/hg.cpp src/hg.h \
	src/main.cpp src/main.h \
	src/pawn.cpp src/pawn.h \
	src/ppm.cpp src/ppm.h \
	src/slider.cpp src/slider.h \
	src/spline.cpp src/spline.h \
	src/user.cpp src/user.h \
	src/zoomcamera.cpp src/zoomcamera.h

CPPFLAGS = -DSDLAPP_RESOURCE_DIR=\"$(pkgdatadir)\"

man_MANS = data/gource.1

pkgdata_DATA = data/beam.png data/bloom.tga data/file.png data/no_photo.png data/gource.style

if FONTDIR
CPPFLAGS += -DSDLAPP_FONT_DIR=\"$(gourcefontdir)/\"
else
fontsdir = $(pkgdatadir)/fonts
fonts_DATA = data/fonts/README data/fonts/FreeSans.ttf
endif
