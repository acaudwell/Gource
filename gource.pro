# Note: this project file currently only implements building on Windows with Mingw-w64
# See the INSTALL file for building instructions

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES -= UNICODE

CONFIG += c++11
CONFIG += object_parallel_to_source

gcc {
    QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-parameter -Wno-reorder
    QMAKE_CXXFLAGS_DEBUG += -DASSERTS_ENABLED
}

mingw {
    QMAKE_CXXFLAGS += -Dmain=SDL_main
    QMAKE_LFLAGS   += -mconsole

    INCLUDEPATH += C:\msys64\mingw64\include\SDL2
    INCLUDEPATH += C:\msys64\mingw64\include\freetype2

    LIBS += -lmingw32 -lSDL2main -lSDL2.dll
    LIBS += -lSDL2_image.dll -lfreetype.dll -lpcre.dll -lpng.dll -lglew32.dll -lboost_system-mt -lboost_filesystem-mt -lopengl32 -lglu32
    LIBS += -static-libgcc -static-libstdc++
    LIBS += -lcomdlg32
}

VPATH += ./src

SOURCES += \
    action.cpp \
    bloom.cpp \
    caption.cpp \
    dirnode.cpp \
    file.cpp \
    gource.cpp \
    gource_settings.cpp \
    gource_shell.cpp \
    key.cpp \
    logmill.cpp \
    main.cpp \
    pawn.cpp \
    slider.cpp \
    spline.cpp \
    textbox.cpp \
    user.cpp \
    zoomcamera.cpp \
    formats/apache.cpp \
    formats/bzr.cpp \
    formats/commitlog.cpp \
    formats/custom.cpp \
    formats/cvs-exp.cpp \
    formats/cvs2cl.cpp \
    formats/git.cpp \
    formats/gitraw.cpp \
    formats/hg.cpp \
    formats/svn.cpp \
    tinyxml/tinystr.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxmlparser.cpp \
    core/conffile.cpp \
    core/display.cpp \
    core/frustum.cpp \
    core/fxfont.cpp \
    core/logger.cpp \
    core/mousecursor.cpp \
    core/plane.cpp \
    core/png_writer.cpp \
    core/ppm.cpp \
    core/quadtree.cpp \
    core/regex.cpp \
    core/resource.cpp \
    core/sdlapp.cpp \
    core/seeklog.cpp \
    core/settings.cpp \
    core/shader.cpp \
    core/shader_common.cpp \
    core/stringhash.cpp \
    core/texture.cpp \
    core/tga.cpp \
    core/timer.cpp \
    core/timezone.cpp \
    core/vbo.cpp \
    core/vectors.cpp

HEADERS += \
    action.h \
    bloom.h \
    caption.h \
    dirnode.h \
    file.h \
    gource.h \
    gource_settings.h \
    gource_shell.h \
    key.h \
    logmill.h \
    main.h \
    pawn.h \
    slider.h \
    spline.h \
    textbox.h \
    user.h \
    zoomcamera.h \
    formats/apache.h \
    formats/bzr.h \
    formats/commitlog.h \
    formats/custom.h \
    formats/cvs-exp.h \
    formats/cvs2cl.h \
    formats/git.h \
    formats/gitraw.h \
    formats/hg.h \
    formats/svn.h \
    tinyxml/tinystr.h \
    tinyxml/tinyxml.h \
    core/bounds.h \
    core/conffile.h \
    core/display.h \
    core/frustum.h \
    core/fxfont.h \
    core/gl.h \
    core/logger.h \
    core/mousecursor.h \
    core/pi.h \
    core/plane.h \
    core/png_writer.h \
    core/ppm.h \
    core/quadtree.h \
    core/regex.h \
    core/resource.h \
    core/sdlapp.h \
    core/seeklog.h \
    core/settings.h \
    core/shader.h \
    core/shader_common.h \
    core/stringhash.h \
    core/texture.h \
    core/tga.h \
    core/timer.h \
    core/timezone.h \
    core/vbo.h \
    core/vectors.h
