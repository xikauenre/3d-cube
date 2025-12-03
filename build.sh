#!/bin/bash

echo "=========================================="
echo "    3D Cube Project Builder"
echo "=========================================="

OS="$(uname -s)"
case "${OS}" in
    Darwin*)    
        OS_NAME="macOS"
        ;;
    Linux*)     
        OS_NAME="Linux"
        ;;
    *)          
        OS_NAME="Unknown"
        ;;
esac

echo "Detected OS: $OS_NAME"

if ! command -v gcc &> /dev/null; then
    echo "Error: gcc not found!"
    exit 1
fi

echo "Compiler: $(gcc --version | head -n1)"

SDL_FOUND=0
if [ "$OS_NAME" = "macOS" ]; then
    if brew list sdl2 &>/dev/null; then
        echo "SDL2 found via Homebrew"
        SDL_FOUND=1
    elif [ -f "/opt/homebrew/include/SDL2/SDL.h" ] || [ -f "/usr/local/include/SDL2/SDL.h" ]; then
        echo "SDL2 found in system"
        SDL_FOUND=1
    fi
else
    if pkg-config --exists sdl2; then
        echo "SDL2 found via pkg-config"
        SDL_FOUND=1
    elif [ -f "/usr/include/SDL2/SDL.h" ]; then
        echo "SDL2 found in system"
        SDL_FOUND=1
    fi
fi

if [ $SDL_FOUND -eq 0 ]; then
    echo "SDL2 not found!"
    echo ""
    echo "Install SDL2:"
    if [ "$OS_NAME" = "macOS" ]; then
        echo "  brew install sdl2"
    elif [ "$OS_NAME" = "Linux" ]; then
        echo "  Ubuntu/Debian: sudo apt install libsdl2-dev"
        echo "  Fedora/RHEL:   sudo dnf install SDL2-devel"
        echo "  Arch:          sudo pacman -S sdl2"
    fi
    exit 1
fi

echo ""
echo "Compiling project..."

if [ "$OS_NAME" = "macOS" ]; then
    gcc -o cube3d src/main.c \
        -I/opt/homebrew/include/SDL2 \
        -L/opt/homebrew/lib \
        -lSDL2 -lm \
        -Wall -Wextra -O2
else
    gcc -o cube3d src/main.c \
        -lSDL2 -lm \
        -Wall -Wextra -O2
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "Successfully compiled!"
    echo ""
    echo "Executable: ./cube3d"
    echo "Size: $(du -h cube3d | cut -f1)"
    echo ""
    echo "To run: ./cube3d"
    echo "=========================================="
else
    echo ""
    echo "=========================================="
    echo "Compilation error!"
    echo "=========================================="
    exit 1
fi
