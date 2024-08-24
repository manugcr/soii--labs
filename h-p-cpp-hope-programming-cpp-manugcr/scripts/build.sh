#!/bin/bash

# Set directories
build_dir=build
tmp_dir=tmp

echo "  -> Creating build directory..."
mkdir -p $build_dir

echo "  -> Creating tmp directory..."
mkdir -p $tmp_dir
rm -rf $tmp_dir/*

echo "  -> Compiling server..."
cd $build_dir
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..

echo "  -> Building server..."
ninja HPCPP_server

echo "  -> Building client..."
ninja HPCPP_client

echo "  -> Launching web server..."
xdg-open http://localhost:8082/root

echo "  -> Launching server..."
./HPCPP_server
