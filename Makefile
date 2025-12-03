# Makefile for 3D Cube Project
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g

# Detect OS
UNAME_S := $(shell uname -s)

# Settings for macOS
ifeq ($(UNAME_S), Darwin)
    BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /usr/local)
    SDL_CFLAGS = -I$(BREW_PREFIX)/include/SDL2 -D_THREAD_SAFE
    SDL_LIBS = -L$(BREW_PREFIX)/lib -lSDL2 -lm
    ifneq (, $(shell which pkg-config 2>/dev/null))
        ifneq (, $(shell pkg-config --exists sdl2 2>/dev/null && echo yes))
            SDL_CFLAGS = $(shell pkg-config --cflags sdl2)
            SDL_LIBS = $(shell pkg-config --libs sdl2) -lm
        endif
    endif
else
    # Linux and others
    SDL_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2 -D_REENTRANT")
    SDL_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2") -lm
endif

TARGET = cube3d
SRC_DIR = src
SRC = $(SRC_DIR)/main.c
OBJ = $(SRC:.c=.o)

.PHONY: all run clean install-deps check-deps help

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo "Building executable..."
	$(CC) $(OBJ) -o $@ $(SDL_LIBS)
	@echo "Done! Run with: ./$(TARGET)"

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Running program..."
	./$(TARGET)

clean:
	@echo "Cleaning build files..."
	rm -f $(OBJ) $(TARGET)

install-deps:
	@echo "Installing dependencies..."
ifeq ($(UNAME_S), Darwin)
	@if ! command -v brew >/dev/null 2>&1; then \
		echo "Installing Homebrew..."; \
		/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"; \
	fi
	brew install sdl2 pkg-config
else ifeq ($(UNAME_S), Linux)
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y libsdl2-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		sudo yum install -y SDL2-devel; \
	elif command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S sdl2; \
	fi
endif

check-deps:
	@echo "Checking dependencies..."
	@echo "OS: $(UNAME_S)"
ifeq ($(UNAME_S), Darwin)
	@if brew list sdl2 &>/dev/null; then \
		echo "SDL2 found via Homebrew"; \
	else \
		echo "SDL2 not found"; \
		echo "Run: make install-deps"; \
	fi
else
	@if pkg-config --exists sdl2; then \
		echo "SDL2 found"; \
	else \
		echo "SDL2 not found"; \
		echo "Run: make install-deps"; \
	fi
endif

help:
	@echo "Available commands:"
	@echo "  make all     - Build program"
	@echo "  make run     - Build and run"
	@echo "  make clean   - Clean build files"
	@echo "  make install-deps - Install dependencies"
	@echo "  make check-deps   - Check dependencies"
	@echo "  make help    - Show this help"
