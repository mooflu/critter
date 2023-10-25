#!/bin/bash -xe
if [[ -z "${EMSDK}" ]]; then
    echo EMSDK not set

    if [ -f ../emsdk/emsdk_env.sh ]; then
        source ../emsdk/emsdk_env.sh
    else
        exit
    fi
fi

export PROJ_FOLDER=`pwd`
export OEM="oem.emscripten"
#export BUILD_TYPE="Debug"
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

ln -sfT ${BUILD_DIR} build

case "$(uname -sr)" in
   CYGWIN*)
     echo 'MS Windows'
     export PROJ_FOLDER=`cygpath -w ${PROJ_FOLDER}`
     export BUILD_DIR=`cygpath -w ${BUILD_DIR}`
     export INSTALL_DIR=`cygpath -w ${INSTALL_DIR}`
     ;;

   *)
     ;;
esac

if [ ! -f resource.dat ]; then
    pushd data
    zip -9r ../resource.dat .
    popd
fi

echo proj folder: ${PROJ_FOLDER}

mkdir -p 3rdparty
pushd 3rdparty
    if [ ! -d glm ]; then
        git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git
    fi

    if [ ! -d physfs ]; then
        git clone --depth 1 --branch release-3.2.0 https://github.com/icculus/physfs.git
    fi
popd

if [ ! -d ${BUILD_DIR} ]; then
    cp -r 3rdparty ${BUILD_DIR}
fi
pushd ${BUILD_DIR}
    pushd physfs
    emcmake cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        -DPHYSFS_BUILD_SHARED=False \
        -DCMAKE_BUILD_TYPE=MinSizeRel \
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
    cmake --install .
    popd

    emcmake cmake \
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DIR} \
        -DCMAKE_PREFIX_PATH:PATH=${INSTALL_DIR} \
        ..
    cmake --build . --config ${BUILD_TYPE} --parallel
    mkdir -p webapp
    cp game/critter.* webapp
popd

