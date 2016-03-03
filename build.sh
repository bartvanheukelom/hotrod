#!/bin/bash

set -e

# --------- building ------------- #

echo "========================== BUILD ===================================="

if [[ -d bin ]]; then
    touch bin/hotrod-filler # otherwise line below might err
    rm bin/hotrod*
else
    mkdir bin
fi

srcFiles=$(find src -name \*.cpp)
echo "SRC:"
echo $srcFiles

warnings="-pedantic -Wall -Wno-unused-result -Wno-missing-field-initializers -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef"
#warnings="$warnings -Weffc++"
#warnings="$warnings -Werror"
#warnings="$warnings -Wpadded"

commonOpts="-std=c++11 -O3 $srcFiles $warnings"

appName=hotrod

echo "++++ linux64"
g++ $commonOpts \
	-I/usr/local/include \
    `# -- libs --` \
    -lSDL2 -lGL -lGLEW \
    `# v8` \
    -isystem./libs/v8 -L./libs/v8/bin \
    -lv8_base -lv8_external_snapshot -lv8_libbase -lv8_libplatform \
    -ldl -lrt -pthread \
	`# bullet` \
	-isystem/home/bart/software/bullet3-2.83.6/src \
	-L/home/bart/software/bullet3-2.83.6/bin -lBulletDynamics_gmake_x64_release -lBulletCollision_gmake_x64_release -lLinearMath_gmake_x64_release \
    `# -- end libs --` \
	-o bin/$appName-linux64 #2>&1 | nl -s ' [g++] '

#echo "++++ win32"
#i686-w64-mingw32-g++ $commonOpts -static -o bin/$appName-win32.exe
#echo "++++ win64"
#x86_64-w64-mingw32-g++ $commonOpts -static -o bin/$appName-win64.exe

echo "compiled"

# --------- testing ------------- #
if [[ "$1" == "test" ]]; then
	echo testing
    cd bin
    ./$appName-linux64
    echo tested
fi
if [[ "$1" == "testh" ]]; then
	echo testing h
    cd bin
    ./$appName-linux64 headless
    echo tested
fi
