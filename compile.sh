#!/bin/bash
echo "Compiling 3D Cube..."
gcc -o cube3d src/main.c -lSDL2 -lm
echo "Done! Run: ./cube3d"
