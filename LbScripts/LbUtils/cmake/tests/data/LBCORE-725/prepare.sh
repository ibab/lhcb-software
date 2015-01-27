#!/bin/bash -ex

# prepare directories
rm -rf tmp dest
mkdir -pv tmp dest

# work in the temporary build area
export CMAKE_PREFIX_PATH=$(pwd)/tmp:$(pwd)/../../..

cp -a A B DBASE tmp
mkdir -p tmp/A/build
(cd tmp/A/build && cmake .. && make install)

mkdir -p tmp/B/build
(cd tmp/B/build && cmake .. && make install)

# work in the destination (release) area
export CMAKE_PREFIX_PATH=$(pwd)/dest:$(pwd)/../../..

mv tmp/* dest
cp -a C dest
mkdir -p dest/C/build
(cd dest/C/build && cmake ..)
