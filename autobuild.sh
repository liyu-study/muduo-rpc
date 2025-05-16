# !bin/bash

set -e
rm -rf `pwd`/build/*
cd `pwd`/build &&
    cmake..&&
    make
cp -r `pwd`/src/include `pwd`/lib