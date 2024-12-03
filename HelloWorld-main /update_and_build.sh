#!/bin/bash
git pull origin main
mkdir -p build
cd build
cmake ..
cmake --build .
