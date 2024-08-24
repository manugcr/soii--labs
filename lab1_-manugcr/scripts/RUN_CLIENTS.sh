#!/bin/bash

# Clean previous build
if [ -d "./build" ]; then
    echo "  -> Removing old build directory..."
    rm -rf ./build/*
else
    echo "  -> Creating build directory..."
    mkdir build
fi

# Compile server and client
echo "  -> Compiling server and client..."
echo ""
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja lab1_server
ninja lab1_client

cd ..
# Run server in a new terminal
echo ""
echo "  -> Running server in a new terminal..."
gnome-terminal --title="Server" -- ./build/lab1_server 8080

# Run client in a new terminal
echo ""
echo "  -> Running client in a new terminal..."
gnome-terminal --title="Ubuntu" -- ./build/lab1_client ubuntu ubuntu

# Run clients with different hostnames in new terminals
echo ""
echo "  -> Running clients with different hostnames in new terminals..."
for i in {1..4}; do
    gnome-terminal --title="Client $i" -- ./build/lab1_client hostname hostname
done

echo "All processes started successfully."
