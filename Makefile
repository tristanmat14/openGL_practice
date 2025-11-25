# Compiler and flags
CXX := x86_64-w64-mingw32-g++
CXX_FLAGS_RELEASE := -Wall -Wextra -Werror -pedantic -std=c++17 -Iinclude
CXX_FLAGS_DEBUG := -g -O0 -Wall -std=c++17 -Iinclude

CC := x86_64-w64-mingw32-gcc
C_FLAGS := -Iinclude

# Linking flags
LD_FLAGS := -Lwinlibs -lglfw3 -lopengl32 -lgdi32

# Default compilation mode
mode ?= release

ifeq ($(mode),debug)
	CXX_FLAGS := $(CXX_FLAGS_DEBUG)
else
	CXX_FLAGS := $(CXX_FLAGS_RELEASE)
endif

# Source Files
SRC_CXX := src/main.cpp src/stb_image.cpp
SRC_C := src/glad.c

# Object Files
OBJ_CXX := $(SRC_CXX:src/%.cpp=build/%.o)
OBJ_C := $(SRC_C:src/%.c=build/%.o)

# All Object Files
OBJS := $(OBJ_CXX) $(OBJ_C)

# Target build exe
TARGET := build/app.exe

# Build rules
all: create_build_dir $(TARGET)

# Create build dir before compilation
create_build_dir:
	mkdir -p build

# Rule for linking object files together into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LD_FLAGS)

# Rule to compile C++ source files to object files
build/%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

# Rule to compile C source files to object files
build/%.o: src/%.c
	$(CC) $(C_FLAGS) -c $< -o $@

# Clean build files
clean: 
	rm -rf build

