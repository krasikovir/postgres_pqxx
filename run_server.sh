#! usr/bin/bash

sleep 60000s
cd app/
mkdir build
cd build
cmake ..
cmake --build .
./asioserver