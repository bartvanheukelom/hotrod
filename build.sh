#!/bin/bash

set -e
appName=hotrod

################# CURRENTLY OBSOLETE BECAUSE OF Makefile ##############
echo
echo
echo '#$@#(&$@(#^@!)*^&^#)$@!*#)@*$&!)_%#*$&@!)#!@&#(*@&()%!#'
echo "This script is currently not the way to build, use make"
echo '#$@#(&$@(#^@!)*^&^#)$@!*#)@*$&!)_%#*$&@!)#!@&#(*@&()%!#'
echo
echo
echo
################# CURRENTLY OBSOLETE BECAUSE OF Makefile ##############

#phases=$1
p1=$1
if [ "$p1" == "" ]; then
    p1=build
fi
p2=$2
function hasPhase {
    if [[ "$p1" == "$1" || "$p2" == "$1" ]]; then
        return 0
    else
        return 1
    fi
}

if hasPhase build; then
    
    echo "========================== BUILD ===================================="
    
    mkdir -p bin/cmake
    pushd bin/cmake
        cmake ../../
        make -j16
        mv $appName ../
    popd
fi

if hasPhase test; then

    echo "========================== TEST ===================================="

    cd bin
    ./$appName
    echo "Test done"

fi
