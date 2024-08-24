#!/bin/bash -e

echo ""

# Search for the build directory and remove it if it exists
if [ -d "./build" ]; then
    echo "  -> Removing old build directory..."
    rm -rf ./build/*
else
    echo "  -> Creating build directory..."
    mkdir ./build
fi

echo ""
echo "  -> Compiling server..."
cd build && cmake -GNinja -DRUN_COVERAGE=1 ..
ninja lab1_server

echo ""
echo "  -> Get ctest_report.xml..."
cd ..
echo "$PWD"
ctest --test-dir ./build/tests -VV --output-junit ./../reports/ctest_report.xml

echo ""
echo "  -> Get coverage.xml..."
gcovr -r . ./build --cobertura ./build/reports/coverage.xml

echo ""
echo "  -> Get cpp_report.xml..."
cppcheck --enable=all --suppress=missingIncludeSystem -I include --xml src lib 2> ./build/reports/cpp_report.xml

echo ""
echo "  -> Get valgrind_report.xml..."
valgrind --xml=yes --xml-file=./build/reports/valgrind_report.xml ./build/lab1_server 8080

echo ""
echo "  -> Executing pipeline.cmake..."
cmake -P path/to/pipeline.cmake
