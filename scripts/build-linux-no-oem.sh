#!/bin/bash -e
# don't build all the static 3rdparty libs - use system libs

sudo apt install zlib1g-dev libpng-dev libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libphysfs-dev libglew-dev

pushd data
zip -9r ../resource.dat .
popd

mkdir -p build
pushd build
cmake -DBUILD_SHARED_LIBS=ON ..
cmake --build . --parallel
popd

file ./build/game/critter
