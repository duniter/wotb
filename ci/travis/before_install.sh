#!/usr/bin/env bash

echo "OS Name = $TRAVIS_OS_NAME"
if [ $TRAVIS_OS_NAME == "osx" ]
then
    # Since default gcc on osx is just a front-end for LLVM
    if [[ ${CC} = gcc ]]; then
        export CXX=g++-4.8
        export CC=gcc-4.8
    fi
fi
