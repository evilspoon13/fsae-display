#!/bin/bash
set -e
cd "$(dirname "$0")/.."

echo "Building CAN Reader..."
make -C can-reader clean && make -C can-reader

# echo "Building Graphics Engine..."
# make -C graphics-engine clean && make -C graphics-engine

# echo "Building Data Logger..."
# make -C data-logger clean && make -C data-logger

# echo "Installing Node dependencies..."
# cd web-server && npm install

echo "Building Tests..."
make -C tests clean && make -C tests

echo "built!"
