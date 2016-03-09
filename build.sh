#!/bin/bash

set -e
appName=hotrod
# --------- building ------------- #

echo "========================== BUILD ===================================="

pushd bin

if [[ ! -d cmake ]]; then
    mkdir cmake
fi

#warnings="-pedantic -Wall -Wno-unused-result -Wno-missing-field-initializers -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef"
##warnings="$warnings -Weffc++"
##warnings="$warnings -Werror"
##warnings="$warnings -Wpadded"

pushd cmake
cmake ../../
make -j16
popd

mv cmake/$appName .

popd

# --------- testing ------------- #
if [[ "$1" == "test" ]]; then
	echo testing
    cd bin
    ./$appName
    echo tested
fi
if [[ "$1" == "testh" ]]; then
	echo testing h
    cd bin
    ./$appName headless
    echo tested
fi
