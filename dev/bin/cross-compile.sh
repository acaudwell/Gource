#!/bin/sh

set -e

BUILD_AREA="/tmp/build.$$"

MINGW_HOST="i686-w64-mingw32"
MINGW_PREFIX="$BUILD_AREA/mingw"

TARBALLS="/home/$USER/tar";

ICONV_SOURCE="http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz"
SDL2_SOURCE="http://libsdl.org/release/SDL2-2.0.1.tar.gz"

rm -rf $BUILD_AREA
mkdir -p $BUILD_AREA

build_iconv() {

    cd $BUILD_AREA
    
    #curl -O $ICONV_SOURCE 
    cp $TARBALLS/libiconv-1.14.tar.gz .

    tar -xvvzf libiconv*.tar.gz
    cd libiconv*

    ./configure --prefix=${MINGW_PREFIX} \
        --host=${MINGW_HOST} \
        --target=${MINGW_HOST} \
        --enable-static \
        --enable-shared
    
    make
    make install
}

build_sdl2() {
    cd $BUILD_AREA

    #curl -O $SDL2_SOURCE
    cp $TARBALLS/SDL2-2.0.1.tar.gz .

    tar -xvvzf SDL2*.tar.gz
    cd SDL2*
    
    ./configure \
      --prefix=${MINGW_PREFIX} \
      --host=${MINGW_HOST}
    
    make
    make install
}

build_iconv
build_sdl2

