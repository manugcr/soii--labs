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
cmake -GNinja -DRUN_COVERAGE=1 ..

echo "  -> Building server..."
ninja HPCPP_server

echo "  -> Running tests..."
ctest --test-dir tests/ -VV --output-junit ./../reports/ctest_report.xml

echo "  -> Running coverage..."
cd ..
gcovr -r "$PWD" --xml-pretty --output=build/reports/coverage.xml --exclude "build/_deps/*" --exclude "external/*" --exclude "tests/*"
