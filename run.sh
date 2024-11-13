#!/bin/sh
mkdir -p bin
mkdir -p build
cd build
cmake ..
cmake --build .
cp bin/main ../bin/
cd ../bin/
chmod +x main
./main
