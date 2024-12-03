#!/bin/bash
echo "--- Starting build process ---"


g++ -Iinclude -o YourProgram test/main1.cpp src/background_runner.cpp


if [ $? -ne 0 ]; then
    echo "--- Compilation failed ---"
    exit 1
fi

echo "--- Compilation successful ---"
