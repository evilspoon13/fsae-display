#!/bin/bash
set -e

echo "Cleaning CAN Reader..."

cd can-reader && make clean && cd ..

echo "Cleaning Graphics Engine..."
cd graphics-engine && make clean && cd ..

echo "Cleaning Data Logger..."
cd data-logger && make clean && make && cd ..

echo "cleaned!"