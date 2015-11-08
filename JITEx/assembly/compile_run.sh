#!/bin/bash

rm ./bin/*
echo "compiling "$1".s"
# compile the assembly code
gcc $1".s" -o "./bin/"$1
# execute the binary
./bin/$1
