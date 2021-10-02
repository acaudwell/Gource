project(gource LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake)

#=== C++ VERSION CHECK ===
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)

#=== Autogen
#autogen.sh

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wshadow")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)


set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include(ExternalProject)
ExternalProject_Add(libpcre
    GIT_REPOSITORY https://github.com/vmg/libpcre.git
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)

find_package(GLEW REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem system)

#add_definitions(-DGOURCE_FONT_FILE=data/fonts/FreeSans.ttf)

add_executable(gource
    src/action.cpp
    src/bloom.cpp
    src/caption.cpp
    src/core/conffile.cpp
    src/core/display.cpp
    src/core/frustum.cpp
    src/core/fxfont.cpp
    src/core/logger.cpp
    src/core/mousecursor.cpp
    src/core/plane.cpp
    src/core/ppm.cpp
    src/core/quadtree.cpp
    src/core/regex.cpp
    src/core/resource.cpp
    src/core/sdlapp.cpp
    src/core/seeklog.cpp
    src/core/settings.cpp
    src/core/shader.cpp
    src/core/shader_common.cpp
    src/core/stringhash.cpp
    src/core/texture.cpp
    src/core/png_writer.cpp
    src/core/timezone.cpp
    src/core/vbo.cpp
    src/core/vectors.cpp
    src/dirnode.cpp
    src/file.cpp
    src/formats/apache.cpp
    src/formats/bzr.cpp
    src/formats/commitlog.cpp
    src/formats/custom.cpp
    src/formats/cvs-exp.cpp
    src/formats/cvs2cl.cpp
    src/formats/git.cpp
    src/formats/gitraw.cpp
    src/formats/hg.cpp
    src/formats/svn.cpp
    src/gource.cpp
    src/gource_shell.cpp
    src/gource_settings.cpp
    src/key.cpp
    src/logmill.cpp
    src/main.cpp
    src/pawn.cpp
    src/slider.cpp
    src/spline.cpp
    src/textbox.cpp
    src/user.cpp
    src/zoomcamera.cpp
    # Use USE_BUNDLED_TINYXML
	src/tinyxml/tinyxmlerror.cpp
	src/tinyxml/tinystr.cpp
	src/tinyxml/tinyxml.cpp
	src/tinyxml/tinyxmlparser.cpp
    )


target_link_libraries(gource SDL2::Main SDL2::Image)

include_directories(gource ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(gource ${FREETYPE_LIBRARIES})

include_directories(gource ${PNG_INCLUDE_DIR})
target_link_libraries(gource ${PNG_LIBRARY})

add_dependencies(gource libpcre)
target_link_libraries(gource pcre)

include_directories(${GLUT_INCLUDE_DIRS} )

target_link_libraries(gource OpenGL::OpenGL ${GLUT_LIBRARY} ${OPENGL_glu_LIBRARY})

include_directories(gource ${GLEW_INCLUDE_DIRS})
target_link_libraries(gource ${GLEW_LIBRARIES})

include_directories(gource ${Boost_INCLUDE_DIR})
target_link_libraries(gource ${Boost_LIBRARIES})
