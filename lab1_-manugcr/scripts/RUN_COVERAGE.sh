#!/bin/bash -e

echo ""

if [ -d "./build" ]; then
    echo "  -> Removing old build directory..."
    rm -rf ./build/*
else
    echo "  -> Creating build directory..."
    mkdir build
fi

echo "  -> Compiling server..."
echo ""
cd build && cmake -GNinja -DRUN_COVERAGE=1 ..
ninja lab1_server
ninja lab1_client

echo ""
echo "  -> Running tests..."
echo ""
ctest --test-dir tests/ -VV

echo ""
echo "  -> Running coverage..."
echo ""
cd ..
gcovr -r "$PWD"
