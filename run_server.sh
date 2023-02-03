#! usr/bin/bash

cd app/
mkdir -p build
cd build
cmake ..
cmake --build .
hostname -i > temp.txt
hostname -i >> temp.txt
cat temp.txt | ./asioserver