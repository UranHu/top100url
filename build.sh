#!/bin/bash

echo "Currently work in $(pwd)"
pwd="$(pwd)"
test="$pwd/test"

if [ ! -d "./build" ]; then
    mkdir build
fi

cd build

cmake ..
make

cd $test
make
mv turl_test $pwd
