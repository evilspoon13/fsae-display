#!/bin/bash
set -e

echo "Building CAN Reader..."
cd can-reader && make clean && make && cd ..

echo "Building Graphics Engine..."
cd graphics-engine && make clean && make && cd ..

echo "Building Data Logger..."
cd data-logger && make clean && make && cd ..

# echo "Installing Node dependencies..."
# cd web-server && npm install && cd ..

echo "built!"