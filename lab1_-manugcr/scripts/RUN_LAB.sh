#!/bin/bash -e

echo ""

# echo "  -> Applying clang-format to all the files..."
# find . -iname *.h -o -iname *.c | xargs clang-format -i

if [ -d "./build" ]; then
    echo "  -> Removing old build directory..."
    rm -rf ./build/*
else
    echo "  -> Creating build directory..."
    mkdir build
fi

echo "  -> Compiling server..."
echo ""
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja lab1_server

echo ""
echo "  -> Compiling client..."
echo ""
ninja lab1_client
