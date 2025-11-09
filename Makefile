# === Raytracer Makefile ===

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wno-unused-private-field $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs)

# Source and output
SRC := main.cpp
TARGET := raytracer

# Default rule
all: $(TARGET)

# Build the program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean the compiled binary
clean:
	rm -f $(TARGET)

.PHONY: all run clean

