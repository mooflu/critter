#!/bin/bash -e

export PROJ_FOLDER=`pwd`
export OEM="oem"
export BUILD_TYPE="Release"

while getopts 'b:' opt; do
  case "$opt" in
    b)
      export BUILD_TYPE=${OPTARG}
      ;;

    ?|h)
      echo "Usage: $(basename $0) [-b <Release|Debug>]"
      exit 1
      ;;
  esac
done

export BUILD_DIR=build.${OEM}.${BUILD_TYPE}
export INSTALL_DIR=${PROJ_FOLDER}/${OEM}.${BUILD_TYPE}

case "$(uname -sr)" in
   CYGWIN*)
     echo 'MS Windows'
     export PROJ_FOLDER=`cygpath -w ${PROJ_FOLDER}`
     export BUILD_DIR=`cygpath -w ${BUILD_DIR}`
     export INSTALL_DIR=`cygpath -w ${INSTALL_DIR}`
     export INSTALL_CMAKE_BUILD_TYPE="--config ${BUILD_TYPE}" # need config for win but breaks non-win
	   # Due to single-configuration generators?
	   # Error on non win:
       # CMake Error in CMakeLists.txt:
       # IMPORTED_LOCATION not set for imported target "SDL2::SDL2-static".
     ;;

   *)
     ;;
esac

if [ ! -f resource.dat ]; then
    pushd data
    zip -9r ../resource.dat .
    popd
fi

echo Proj folder: ${PROJ_FOLDER}
echo Build type: ${BUILD_TYPE}

mkdir -p 3rdparty
pushd 3rdparty
    if [ ! -d glm ]; then
        git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git
    fi

    if [ ! -d physfs ]; then
        git clone --depth 1 --branch release-3.2.0 https://github.com/icculus/physfs.git
    fi

    if [ ! -d zlib ]; then
        git clone --depth 1 --branch v1.3 https://github.com/madler/zlib.git
    fi

    if [ ! -d libpng ]; then
        if [ ! -f libpng-1.6.40.tar.gz ]; then
            wget https://download.sourceforge.net/libpng/libpng-1.6.40.tar.gz
        fi
        tar xf libpng-1.6.40.tar.gz
        mv libpng-1.6.40 libpng
    fi

    if [ ! -d glew ]; then
        #git clone --depth 1 --branch glew-2.2.0 https://github.com/nigels-com/glew.git
        #pushd glew/auto
        #make
        #popd
        if [ ! -f glew-2.2.0.tgz ]; then
            wget https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
        fi
        tar xf glew-2.2.0.tgz
        mv glew-2.2.0 glew
    fi

    if [ ! -d SDL ]; then
        git clone --depth 1 --branch release-2.28.2 https://github.com/libsdl-org/SDL.git
    fi

    if [ ! -d SDL_image ]; then
        git clone --depth 1 --branch release-2.6.3 https://github.com/libsdl-org/SDL_image.git
    fi

    if [ ! -d SDL_mixer ]; then
        git clone --depth 1 --branch release-2.6.3 https://github.com/libsdl-org/SDL_mixer.git
    fi

    if [ ! -d libogg ]; then
        if [ ! -f libogg-1.3.5.tar.gz ]; then
            wget https://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.gz
        fi
        tar xf libogg-1.3.5.tar.gz
        mv libogg-1.3.5 libogg
    fi

    if [ ! -d libvorbis ]; then
        if [ ! -f libvorbis-1.3.7.tar.gz ]; then
            wget https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.7.tar.gz
        fi
        tar xf libvorbis-1.3.7.tar.gz
        mv libvorbis-1.3.7 libvorbis
    fi
popd

if [ ! -d ${BUILD_DIR} ]; then
    cp -r 3rdparty ${BUILD_DIR}
fi
pushd ${BUILD_DIR}
    pushd zlib
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        .
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    # zlib doesn't seem to honour BUILD_SHARED_LIBS false
    rm -f ${INSTALL_DIR}/lib/libz.so*
    rm -f ${INSTALL_DIR}/lib/zlib.dll
    rm -f ${INSTALL_DIR}/lib/zlib.lib
    rm -f ${INSTALL_DIR}/lib/libzd.so*
    rm -f ${INSTALL_DIR}/lib/zlibd.dll
    rm -f ${INSTALL_DIR}/lib/zlibd.lib
    rm -f ${INSTALL_DIR}/lib/libz*dylib
    popd

    pushd libpng
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DPNG_SHARED:BOOL=OFF \
        -DPNG_STATIC:BOOL=ON \
        .
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    popd

    pushd glew/build
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DBUILD_UTILS:BOOL=OFF \
        ./cmake
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    popd

    pushd physfs
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DPHYSFS_BUILD_SHARED=False \
        -DPHYSFS_ARCHIVE_7Z:BOOL=ON \
        -DPHYSFS_ARCHIVE_ZIP=ON \
        -DPHYSFS_ARCHIVE_GRP=OFF \
        -DPHYSFS_ARCHIVE_WAD=OFF \
        -DPHYSFS_ARCHIVE_HOG=OFF \
        -DPHYSFS_ARCHIVE_MVL=OFF \
        -DPHYSFS_ARCHIVE_QPAK=OFF \
        -DPHYSFS_ARCHIVE_SLB=OFF \
        -DPHYSFS_ARCHIVE_ISO9660=OFF \
        -DPHYSFS_ARCHIVE_VDF=OFF \
        .
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    popd

    pushd SDL
        mkdir -p build
        pushd build
        cmake -S .. -B . \
            -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
            -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
            -DBUILD_SHARED_LIBS:BOOL=OFF \
            -DSDL_SHARED:BOOL=OFF
        cmake --build . --config ${BUILD_TYPE} --parallel
        cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
        popd
    popd

    pushd SDL_image
        mkdir -p build
        pushd build
        cmake -S .. -B . \
            -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
            -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
            -DBUILD_SHARED_LIBS:BOOL=OFF \
            -DSDL2IMAGE_AVIF:BOOL=OFF \
            -DSDL2IMAGE_BMP:BOOL=OFF \
            -DSDL2IMAGE_GIF:BOOL=OFF \
            -DSDL2IMAGE_JPG:BOOL=OFF \
            -DSDL2IMAGE_JXL:BOOL=OFF \
            -DSDL2IMAGE_LBM:BOOL=OFF \
            -DSDL2IMAGE_PCX:BOOL=OFF \
            -DSDL2IMAGE_PNM:BOOL=OFF \
            -DSDL2IMAGE_QOI:BOOL=OFF \
            -DSDL2IMAGE_SVG:BOOL=OFF \
            -DSDL2IMAGE_TGA:BOOL=OFF \
            -DSDL2IMAGE_TIF:BOOL=OFF \
            -DSDL2IMAGE_WEBP:BOOL=OFF \
            -DSDL2IMAGE_XCF:BOOL=OFF \
            -DSDL2IMAGE_XPM:BOOL=OFF \
            -DSDL2IMAGE_XV:BOOL=OFF \
            -DSDL2IMAGE_TESTS:BOOL=OFF \
            -DSDL2IMAGE_SAMPLES:BOOL=OFF
        cmake --build . --config ${BUILD_TYPE} --parallel
        cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
        # it installs cmake stuff into framework and then linking thinks that's where the lib is :(
        rm -rf ${INSTALL_DIR}/SDL2_image.framework
        popd
    popd

    pushd SDL_mixer
        mkdir -p build
        pushd build
        cmake -S .. -B . \
            -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
            -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
            -DBUILD_SHARED_LIBS:BOOL=OFF \
            -DSDL2MIXER_OPUS:BOOL=OFF \
            -DSDL2MIXER_MIDI:BOOL=OFF \
            -DSDL2MIXER_MP3:BOOL=OFF \
            -DSDL2MIXER_MOD:BOOL=OFF \
            -DSDL2MIXER_FLAC:BOOL=OFF \
            -DSDL2MIXER_CMD:BOOL=OFF \
            -DSDL2MIXER_SAMPLES:BOOL=OFF
        cmake --build . --config ${BUILD_TYPE} --parallel
        cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
        popd
    popd

    pushd libogg
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        .
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    popd

    pushd libvorbis
    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        .
    cmake --build . --config ${BUILD_TYPE} --parallel
    cmake --install . ${INSTALL_CMAKE_BUILD_TYPE}
    popd

    cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DBUILD_SHARED_LIBS:BOOL=OFF \
        ..
    cmake --build . --config ${BUILD_TYPE} --parallel
popd
