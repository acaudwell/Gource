ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = gource

gource_CXXFLAGS = -std=gnu++0x -Wall -Wno-sign-compare -Wno-reorder -Wno-unused-but-set-variable -Wno-unused-variable

gource_SOURCES = \
	src/action.cpp \
	src/bloom.cpp \
	src/caption.cpp \
	src/core/conffile.cpp \
	src/core/display.cpp \
	src/core/frustum.cpp \
	src/core/fxfont.cpp \
	src/core/logger.cpp \
	src/core/mousecursor.cpp \
	src/core/plane.cpp \
	src/core/ppm.cpp \
	src/core/quadtree.cpp \
	src/core/regex.cpp \
	src/core/resource.cpp \
	src/core/sdlapp.cpp \
	src/core/seeklog.cpp \
	src/core/settings.cpp \
	src/core/shader.cpp \
	src/core/shader_common.cpp \
	src/core/stringhash.cpp \
	src/core/texture.cpp \
	src/core/png_writer.cpp \
	src/core/timezone.cpp \
	src/core/vbo.cpp \
	src/core/vectors.cpp \
	src/dirnode.cpp \
	src/file.cpp \
	src/formats/apache.cpp \
	src/formats/bzr.cpp \
	src/formats/commitlog.cpp \
	src/formats/custom.cpp \
	src/formats/cvs-exp.cpp \
	src/formats/cvs2cl.cpp \
	src/formats/git.cpp \
	src/formats/gitraw.cpp \
	src/formats/hg.cpp \
	src/formats/svn.cpp \
	src/gource.cpp \
	src/gource_shell.cpp \
	src/gource_settings.cpp \
	src/key.cpp \
	src/logmill.cpp \
	src/main.cpp \
	src/pawn.cpp \
	src/slider.cpp \
	src/spline.cpp \
	src/textbox.cpp \
	src/user.cpp \
	src/zoomcamera.cpp

AM_CPPFLAGS = -DSDLAPP_RESOURCE_DIR=\"$(pkgdatadir)\"

dist_pkgdata_DATA = data/beam.png data/bloom.tga data/bloom_alpha.tga data/file.png data/user.png data/gource.style

shadersdir = $(pkgdatadir)/shaders
dist_shaders_DATA = data/shaders/shadow.vert data/shaders/shadow.frag data/shaders/bloom.vert data/shaders/bloom.frag data/shaders/text.vert data/shaders/text.frag

install-data-hook:
	$(MKDIR_P) $(DESTDIR)$(mandir)/man1
	gzip -cf9 $(srcdir)/data/gource.1 > $(DESTDIR)$(mandir)/man1/gource.1.gz

uninstall-hook:
	rm -f $(DESTDIR)$(mandir)/man1/gource.1.gz


if USE_BUNDLED_TINYXML
gource_SOURCES += \
	src/tinyxml/tinyxmlerror.cpp \
	src/tinyxml/tinystr.cpp \
	src/tinyxml/tinyxml.cpp \
	src/tinyxml/tinyxmlparser.cpp
endif

if FONTFILE
AM_CPPFLAGS += -DGOURCE_FONT_FILE=\"$(gourcefontfile)\"
else
if FONTDIR
AM_CPPFLAGS += -DSDLAPP_FONT_DIR=\"$(gourcefontdir)\"
else
fontsdir = $(pkgdatadir)/fonts
dist_fonts_DATA = data/fonts/README data/fonts/FreeSans.ttf
endif
endif
